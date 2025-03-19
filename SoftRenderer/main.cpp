#include "./Core/TGAImage.h"
#include "./Core/Model.h"
#include "./Core/Graphics.h"
#include "./Core/Texture.h"
#include "./Core/Camera.h"

#include "./Platform/Win32.h"

const int WIDTH = 800;
const int HEIGHT = 800;
Model* model = nullptr;
unsigned char* framebuffer = nullptr;

Vec3f light_position = Vec3f(1, 1, 1);
Vec3f light_dir = Vec3f(1, 1, 1).normalize();
Vec3f       eye(0.f, 0.f, 3.f);
Vec3f    center(0, 0, 0);
Vec3f        up(0, 1, 0);
Camera camera(eye, center, WIDTH / HEIGHT);
mat4 Viewport;

struct Shader : public IShader {
    Matrix<2, 3, float> varying_vUv;              // same as above
    Matrix<3, 3, float> varying_vPos;
    Matrix<3, 3, float> varying_vNorm;

    Matrix<4, 4, float> uniform_lightMatrix;   // only a ortho projection matrix here
    Matrix<4, 4, float> uniform_mvp;             // Projection*ModelView
    Matrix<4, 4, float> uniform_model;
    Matrix<4, 4, float> uniform_model_invtran;     // (Projection*ModelView).invert_transpose()

    Texture* inputAttachment_shadowMap;

    float shadowCalculation(Vec4f fragPosLightSpace, Vec3f normal, Vec3f lightDir)
    {
        normal.normalize();
        lightDir.normalize();
        Vec3f projCoords = proj<3>(fragPosLightSpace / fragPosLightSpace[3]);
        projCoords = projCoords * 0.5f + Vec3f(0.5f, 0.5f, 0.5f);
        float current_depth = projCoords.z;
        int x = projCoords.x * inputAttachment_shadowMap->get_width();
        int y = projCoords.y * inputAttachment_shadowMap->get_height();
        float sampled_depth = inputAttachment_shadowMap->get_pixel(x, y)[0];

        float bias = std::max(0.1f * (1.0 - (normal * lightDir)), 0.005);
        float shadow = current_depth - 0.05 > sampled_depth ? 0.8f : 0.0;

        return shadow;
    }

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f in_vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        Vec4f gl_Vertex = Viewport * uniform_mvp * in_vertex;     // transform it to screen coordinates
        varying_vUv.set_col(nthvert, model->uv(iface, nthvert));
        varying_vPos.set_col(nthvert, proj<3>(uniform_model * embed<4>(in_vertex, 1.f)));
        varying_vNorm.set_col(nthvert, proj<3>(uniform_model_invtran * embed<4>(model->normal(iface, nthvert), 0.f)));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& out_color) {
        // interpolation
        Vec2f uv = varying_vUv * bar;                 // interpolate uv for the current pixel
        Vec3f fragPos = varying_vPos * bar;       // interpolate world space position for the current pixel
        Vec3f fragNorm = (varying_vNorm * bar).normalize(); // interpolate normal for the current pixel

        // calculate tangent
        Matrix<3, 3, float> A;
        A[0] = varying_vPos.col(1) - varying_vPos.col(0);
        A[1] = varying_vPos.col(2) - varying_vPos.col(0);
        A[2] = fragNorm;
        Matrix<3, 3, float> AI = A.invert();
        Vec3f i = AI * Vec3f(varying_vUv[0][1] - varying_vUv[0][0], varying_vUv[0][2] - varying_vUv[0][0], 0);
        Vec3f j = AI * Vec3f(varying_vUv[1][1] - varying_vUv[1][0], varying_vUv[1][2] - varying_vUv[1][0], 0);
        Matrix<3, 3, float> B;
        B.set_col(0, i.normalize());
        B.set_col(1, j.normalize());
        B.set_col(2, fragNorm);

        // light calculation     !!caution!! vector can't be translated, set w to 0.
        Vec3f n = (B * model->normal_tangent(uv)).normalize();
        Vec3f l = light_dir;
        Vec3f r = reflect(n, l).normalize();
        Vec3f viewPos = eye;
        Vec3f viewDir = (viewPos - fragPos).normalize();
        TGAColor color = model->diffuse(uv);
        // ambient
        TGAColor ambient = color * 0.15;
        // diffuse
        float diff = std::max(0.f, n * l);
        TGAColor diffuse = color * diff;
        // specular !! not sure if the specular map is correct
        float shininess = std::max(4.f, model->specular(uv) / 255.0f * 64.0f);
        float spec = std::pow(std::max(0.f, viewDir * r), shininess);
        TGAColor specular = TGAColor(255, 255, 255) * spec * 0.3f;

        // shadow calculation
        Vec4f fragPosLight = uniform_lightMatrix * embed<4>(fragPos, 1.0f);
        float shadow = shadowCalculation(fragPosLight, n, l);

        for (int i = 0; i < 3; i++) out_color[i] = std::min<int>(ambient[i] + ((diffuse[i] + specular[i]) * (1 - shadow)), 255);
        return false;                              // no, we do not discard this pixel
    }
};

struct DepthShader : public IShader {
    Matrix<3, 3, float> varying_vPos;
    Matrix<4, 4, float> uniform_lightSpaceMatrix; // mvp
    Matrix<4, 4, float> uniform_model; // mvp

    DepthShader() : varying_vPos() {}

    virtual Vec4f vertex(int iface, int nthvert) {
        Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
        gl_Vertex = Viewport * uniform_lightSpaceMatrix * uniform_model * gl_Vertex;          // transform it to screen coordinates
        varying_vPos.set_col(nthvert, proj<3>(gl_Vertex / gl_Vertex[3]));
        return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor& color) {
        Vec3f p = varying_vPos * bar;
        float frag_depth = std::max(0.f, std::min(1.f, (0.5f * p.z + .5f)));
        color = TGAColor(255, 255, 255) * frag_depth;
        return false;
    }
};

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("../Assets/african_head/african_head.obj");
    }

    // initialize window
    window_init(WIDTH, HEIGHT, "SoftRenderer");

    // lighting pass framebuffer
    framebuffer = new unsigned char[WIDTH * HEIGHT * 4];
    Texture color_buffer(WIDTH, HEIGHT, TexutreFormat::RGBA8);
    Texture depth_buffer(WIDTH, HEIGHT, TexutreFormat::RGBA32);

    // depth pass framebuffer
    Texture shadow_buffer(WIDTH, HEIGHT, TexutreFormat::RGBA8);  // TODO: remove this redundent buffer
    Texture shadow_zbuffer(WIDTH, HEIGHT, TexutreFormat::RGBA32); 

    mat4 lightSpaceMatrix = ortho(-2, 2, -2, 2, 0.1, 5.f) * lookat(light_position, center, up);

    {   // rendering the shadow buffer
        Viewport = viewport(0, 0, WIDTH, HEIGHT);
        shadow_buffer.clear(Vec4f(0.1f, 0.1f, 0.1f, 0.1f));
        shadow_zbuffer.clear(Vec4f(1.f, 0.f, 0.f, 0.f));

        DepthShader depthshader;
        depthshader.uniform_lightSpaceMatrix = lightSpaceMatrix;
        depthshader.uniform_model = mat4::identity();
        Vec4f screen_coords[3];
        for (int i = 0; i < model->nfaces(); i++) 
        {
            for (int j = 0; j < 3; j++) 
                screen_coords[j] = depthshader.vertex(i, j);
            triangle(screen_coords, depthshader, shadow_buffer, shadow_zbuffer);
        }
    }

    float start_time = 0.f;
    float end_time = 0.f;
    while (!window->is_close)
    {
        start_time = platform_get_time();
        {
            // update camera
            camera.orbit(Vec2f(0.01f, 0.f));
            // lighting pass
            depth_buffer.clear(Vec4f(1.f, 0.f, 0.f, 0.f));
            color_buffer.clear(Vec4f(0.f, 0.f, 0.f, 1.f));
            mat4 model_view = camera.get_view_matrix();
            mat4 proj = camera.get_proj_matrix();

            Viewport = viewport(0, 0, WIDTH, HEIGHT);

            Shader shader;
            shader.uniform_mvp = proj * model_view;
            shader.uniform_model = mat4::identity();
            shader.uniform_model_invtran = shader.uniform_model.invert_transpose();
            shader.uniform_lightMatrix = lightSpaceMatrix;
            shader.inputAttachment_shadowMap = &shadow_zbuffer;

            for (int i = 0; i < model->nfaces(); i++) {
                Vec4f screen_coords[3];
                for (int j = 0; j < 3; j++)
                    screen_coords[j] = shader.vertex(i, j);
                triangle(screen_coords, shader, color_buffer, depth_buffer);
            }

        }

        color_buffer.to_uchar(framebuffer);
        window_draw(framebuffer);
        msg_dispatch();

        end_time = platform_get_time();
        std::cout << "fps " << 1.f / (end_time - start_time) << std::endl;
    }

    delete[] framebuffer;
    delete model;

    window_destroy();
    return 0;
}