#pragma once

class Texture
{
public:
	Texture() = default;
	Texture(const char* path);

	Texture(const Texture&) = delete;
	Texture(Texture&& other) noexcept;

	~Texture();

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& other) noexcept;

	unsigned int GlId() const { return glId; }

private:
	unsigned int glId = 0;
};
