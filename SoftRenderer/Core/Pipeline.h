#pragma once
#include "Texture.h"

#define MAX_VERTEX_ATTRIBS_SIZE 256
#define MAX_VARYINGS_SIZE 256
#define MAX_UNIFORMS_SIZE 1024

using VertexShaderFunc = Vec4f(*)(void* attribs, void* varyings, void* uniforms);
using FragmentShaderFunc = Vec4f(*)(void* varyings, void* uniforms);

class Pipeline {
public:
    Pipeline();

    // pipeline status setting
    void set_depth_test(bool test, bool write);
    void set_blend(bool blend);
    void set_cull_face(bool cull_face);
    void set_vertex_attribs(void* attribs, uint32_t size);
    void set_uniforms(void* uniforms, uint32_t size);
    void set_vertex_shader(VertexShaderFunc shader);
    void set_fragment_shader(FragmentShaderFunc shader);
    void set_resource_layout(uint32_t attribs_size, uint32_t varyings_size, uint32_t uniforms_size); // currently, not surpport layout with offset
    bool is_valid();

    // draw call
    void draw_triangle(Texture* color_attachment, Texture* depth_attachment);

private:
    uint32_t m_attribs_size;
    uint32_t m_varyings_size;
    uint32_t m_uniforms_size;

    bool m_enable_depth_test;
    bool m_enable_depth_write;
    bool m_enable_blend;
    bool m_enable_cull_face;

    uint8_t m_shader_attribs[MAX_VERTEX_ATTRIBS_SIZE * 3];
    uint8_t m_shader_varyings[MAX_VARYINGS_SIZE];
    uint8_t m_shader_uniforms[MAX_UNIFORMS_SIZE];

    VertexShaderFunc m_vertex_shader;
    FragmentShaderFunc m_fragment_shader;

};