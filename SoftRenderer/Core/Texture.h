#pragma once
#include "Maths.h"

#include <vector>
#include <array>
#include <string_view>

enum class TexutreFormat
{
	RGBA8, // ldr
	RGBA32, // hdr
};

// can be used as render target, samplerImage, depth buffer
class Texture
{
public:
	Texture(uint32_t width, uint32_t height, TexutreFormat format, const char* data = nullptr);

	uint32_t get_width() const { return m_width; }
	uint32_t get_height() const { return m_height; }
	uint32_t get_bytespp() const { return m_bytespp; }
	TexutreFormat get_format() const { return m_format; }
	const std::vector<Vec4f>& get_data() const { return m_data; }

	void set_pixel(uint32_t x, uint32_t y, const Vec4f& color);
	Vec4f get_pixel(uint32_t x, uint32_t y);

	void clear(const Vec4f& color);

	// texture sampling
	Vec4f repeat_sample(Vec2f uvf);
	Vec4f clamp_sample(Vec2f uvf);
	Vec4f sample(Vec2f uvf);

	// color space conversion. only call these two functions when data represent colors.
	void linear_to_srgb();
	void srgb_to_linear();

	void to_uchar(unsigned char* out_buffer);

private:
	std::vector<Vec4f> m_data;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_bytespp;
	TexutreFormat m_format;
};

class CubeTexture
{
public:
	CubeTexture(uint32_t width, uint32_t height, TexutreFormat format);
	CubeTexture(std::string_view positive_x, std::string_view negative_x,
		std::string_view positive_y, std::string_view negative_y,
		std::string_view positive_z, std::string_view negative_z);
	~CubeTexture();

	std::array<Texture, 6> m_textures;
};