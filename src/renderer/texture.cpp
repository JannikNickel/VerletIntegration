#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad/glad.h"
#include <exception>
#include <format>

Texture::Texture(const char* path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, channels;
	unsigned char* img = stbi_load(path, &width, &height, &channels, 0);
	if(img == nullptr)
	{
		stbi_image_free(img);
		throw std::exception(std::format("Could not load image data! (%s)", path).c_str());
	}

	glGenTextures(1, &glId);
	glBindTexture(GL_TEXTURE_2D, glId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(img);
}

Texture::Texture(Texture&& other) noexcept
{
	this->glId = other.glId;
	other.glId = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if(this != &other)
	{
		this->glId = other.glId;
		other.glId = 0;
	}
	return *this;
}

Texture::~Texture()
{
	glDeleteTextures(1, &glId);
}
