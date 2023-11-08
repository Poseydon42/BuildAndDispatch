#pragma once

#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string_view>

#include "Core/Assert.h"

class Shader
{
public:
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
 * VECTORS
 */
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
