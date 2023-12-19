#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Core/Logger.h"

std::unique_ptr<Texture> Texture::Load(std::string_view Path)
{
	int Width, Height, Channels;
	auto* Image = stbi_load(Path.data(), &Width, &Height, &Channels, 0);

	if (!Image)
		return nullptr;

	if (Channels != 3 && Channels != 4)
	{
		BD_LOG_ERROR("Texture::Load: Unsupported number of channels: {0}", Channels);
		return nullptr;
	}

	GLuint Texture;
	glGenTextures(1, &Texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);

	GLenum Format = (Channels == 3 ? GL_RGB : GL_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(Image);

	return std::unique_ptr<class Texture>(new class Texture(Texture, { Width, Height} ));
}
