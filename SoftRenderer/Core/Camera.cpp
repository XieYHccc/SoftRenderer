#include "Camera.h"

static const float s_fov = to_radians(60.f);
static const float s_near = 0.1f;
static const float s_far = 1000.f;
static const Vec3f s_world_up = Vec3f(0.f, 1.f, 0.f);

Camera::Camera(const Vec3f& position, const Vec3f target, float aspect)
    : m_position(position), m_target(target), m_aspect(aspect)
{

}

Vec3f Camera::get_position()
{
    return m_position;
}

Vec3f Camera::get_target()
{
    return m_target;
}

Vec3f Camera::get_forward()
{
    Vec3f from_camera = m_target - m_position;
    return from_camera.normalize();
}

mat4 Camera::get_view_matrix()
{
    return lookat(m_position, m_target, s_world_up);
}

mat4 Camera::get_proj_matrix()
{
    return perspective(s_fov, m_aspect, s_near, s_far);
}

void Camera::set_transform(const Vec3f& position, const Vec3f& target)
{
    m_position = position;
    m_target = target;
}

void Camera::pan(Vec2f pan)
{
    Vec3f forward = get_forward();
	Vec3f left = cross(s_world_up, forward).normalize();
	Vec3f up = cross(forward, left).normalize();

    float distance = (m_position - m_target).norm();
    float factor = 2 * distance * std::tan(s_fov / 2.f);
    Vec3f delta = (left * pan[0] + up * pan[1]) * factor;
    m_position = m_position + delta;
    m_target = m_target + delta;
}

void Camera::orbit(Vec2f orbit)
{
    Vec3f from_target = m_position - m_target;
    float radius = from_target.norm();
    float theta = std::atan2(from_target.x, from_target.z); /* azimuth: (-pi, pi) */ 
    float phi = std::acos(from_target.y / radius);          /* polar: (0, pi) */
    float factor = 2 * PI;

    theta += orbit[0] * factor;
    phi += orbit[1] * factor;
    if (phi > PI) phi = PI - 0.01f;
    if (phi < 0) phi = 0.01f;

    m_position.x = m_target.x + radius * std::sin(phi) * std::sin(theta);
    m_position.y = m_target.y + radius * std::cos(phi);
    m_position.z = m_target.z + radius * std::sin(phi) * std::cos(theta);

}

void Camera::zoom(float zoom)
{
    Vec3f forward = get_forward();
	Vec3f delta = forward * zoom;
	m_position = m_position + delta;
}

