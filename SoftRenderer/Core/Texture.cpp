#include "Texture.h"

#include <algorithm>

Texture::Texture(uint32_t width, uint32_t height, TexutreFormat format, const char* data)
	: m_width(width), m_height(height), m_format(format)
{
	m_bytespp = (format == TexutreFormat::RGBA8) ? 4 : 16;
	m_data.resize(width * height);
	if (data)
	{
		memcpy(m_data.data(), data, width * height * m_bytespp);
	}
}

void Texture::set_pixel(uint32_t x, uint32_t y, const Vec4f& color)
{
	assert(x < m_width && y < m_height);
	m_data[y * m_width + x] = color;
}

Vec4f Texture::get_pixel(uint32_t x, uint32_t y)
{
	assert(x < m_width && y < m_height);
	return m_data[y * m_width + x];
}

void Texture::clear(const Vec4f& color)
{
	Vec4f clamped_color = color;
	if (m_format == TexutreFormat::RGBA8)
	{
		for (int i = 0; i < 4; i++)
			clamped_color[i] = std::clamp(clamped_color[i], 0.f, 1.f);
	}

	for (auto& c : m_data)
		c = clamped_color;
}

Vec4f Texture::repeat_sample(Vec2f uv)
{
	return Vec4f();
}

Vec4f Texture::clamp_sample(Vec2f uv)
{
	return Vec4f();
}

Vec4f Texture::sample(Vec2f uvf)
{
	int x = uvf[0] * m_width;
	int y = uvf[1] * m_height;
	return m_data[x + y * m_width];
}

void Texture::to_unsigned_char(unsigned char* out_buffer)
{
	for (int i = 0; i < m_width; i++) {
		for (int j = 0; j < m_height; j++) {
			Vec4f color = get_pixel(i, j);
			uint32_t index = ((m_height -1 - j) * m_width + i) * 4;
			out_buffer[index + 0] = color[0] * 255;
			out_buffer[index + 1] = color[1] * 255;
			out_buffer[index + 2] = color[2] * 255;
			out_buffer[index + 3] = color[3] * 255;
		}
	}
}

