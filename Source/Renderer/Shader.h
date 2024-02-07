#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string_view>

#include "Core/Assert.h"
#include "Renderer/Texture.h"

class Shader
{
public:
	~Shader();

	static std::unique_ptr<Shader> Create(std::string_view VertexShaderPath, std::string_view FragmentShaderPath);

	void Bind() const;

	template<typename DataType>
	void SetUniform(std::string_view Name, const DataType& Value);

	template<typename DataType>
	void SetUniform(uint32_t Index, const DataType& Value);

private:
	GLuint m_Program = 0;

	explicit Shader(GLuint Program);
};

template<typename DataType>
void Shader::SetUniform(std::string_view Name, const DataType& Value)
{
	auto Location = glGetUniformLocation(m_Program, Name.data());
	if (Location < 0)
		return;

	SetUniform(static_cast<uint32_t>(Location), Value);
}

template<typename DataType>
void Shader::SetUniform(uint32_t Index, const DataType& Value)
{
	BD_UNREACHABLE();
}

/*
 * SCALARS
 */
template<>
inline void Shader::SetUniform(uint32_t Index, const float& Value)
{
	Bind();
	glUniform1fv(Index, 1, &Value);
}

/*
 * VECTORS
 */
template<>
inline void Shader::SetUniform(uint32_t Index, const glm::vec2& Value)
{
	Bind();
	glUniform2fv(Index, 1, glm::value_ptr(Value));
}

template<>
inline void Shader::SetUniform(uint32_t Index, const glm::vec3& Value)
{
	Bind();
	glUniform3fv(Index, 1, glm::value_ptr(Value));
}

template<>
inline void Shader::SetUniform(uint32_t Index, const glm::vec4& Value)
{
	Bind();
	glUniform4fv(Index, 1, glm::value_ptr(Value));
}

/*
 * MATRICES
 */
template<>
inline void Shader::SetUniform(uint32_t Index, const glm::mat4& Value)
{
	Bind();
	glUniformMatrix4fv(Index, 1, false, glm::value_ptr(Value));
}

/*
 * TEXTURES
 */
template<>
inline void Shader::SetUniform(uint32_t Index, const Texture& Value)
{
	static constexpr uint32_t MaxTextureUnitCount = 16;
	static uint32_t NextTextureUnit = 0;

	glActiveTexture(NextTextureUnit);
	glBindTexture(GL_TEXTURE_2D, Value.GetNativeHandle());

	Bind();
	glUniform1ui(Index, GL_TEXTURE0 + NextTextureUnit);

	NextTextureUnit = (NextTextureUnit + 1) % MaxTextureUnitCount;
}
