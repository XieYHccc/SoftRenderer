#pragma once
#include "Maths.h"

class Camera
{
public:
	Camera(const Vec3f& position, const Vec3f target, float aspect);

	Vec3f get_position();
	Vec3f get_target();
	Vec3f get_forward();

	mat4 get_view_matrix();
	mat4 get_proj_matrix();

	// transform
	void set_transform(const Vec3f& position, const Vec3f& target);
	void pan(Vec2f pan); // 
	void orbit(Vec2f orbit);
	void zoom(float zoom);

private:
	Vec3f m_position;
	Vec3f m_target;
	float m_aspect;
};