#pragma once

#include <glad/glad.h>
#include <memory>
#include <string>
#include <glm/vec2.hpp>

class Texture
{
public:
	~Texture();

	static std::unique_ptr<Texture> Load(std::string_view Path);

	glm::ivec2 Dimensions() { return m_Dimensions; }

private:
	GLuint m_Texture = 0;
	glm::ivec2 m_Dimensions = {};

	explicit Texture(GLuint Texture, glm::ivec2 Dimensions)
		: m_Texture(Texture)
		, m_Dimensions(Dimensions)
	{}

	GLuint GetNativeHandle() const { return m_Texture; }

	friend class Shader;
};
