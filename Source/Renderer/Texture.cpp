#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Core/Logger.h"

std::unique_ptr<Texture> Texture::Load(const std::string& Path)
{
	int Width, Height, Channels;
	auto* Image = stbi_load(Path.c_str(), &Width, &Height, &Channels, 0);

	if (!Image)
		return nullptr;

	if (Channels != 3 && Channels != 4)
	{
		BD_LOG_ERROR("Texture::Load: Unsupported number of channels: {0}", Channels);
		return nullptr;
	}

	GLuint Texture;
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);

	GLenum Format = (Channels == 3 ? GL_RGB : GL_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(Image);

	return std::unique_ptr<class Texture>(new class Texture(Texture, { Width, Height} ));
}
