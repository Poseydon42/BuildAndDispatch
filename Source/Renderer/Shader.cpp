#include "Shader.h"

#include "Core/Assert.h"
#include "Core/Logger.h"

static std::optional<GLuint> LoadAndCompileShader(GLenum Type, std::string_view Path)
{
	auto MaybeSource = FileSystem::ReadFileAsString(Path);
	if (!MaybeSource)
	{
		BD_LOG_ERROR("Could not load shader; file {} is unaccessible", Path);
		return std::nullopt;
	}
	auto Source = MaybeSource.value().c_str();

	auto Shader = glCreateShader(Type);
	BD_ASSERT(Shader);
	glShaderSource(Shader, 1, &Source, nullptr);

	glCompileShader(Shader);
	GLint CompileStatus;
	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);
	if (CompileStatus)
		return Shader;

	char ErrorMessage[2048];
	glGetShaderInfoLog(Shader, sizeof(ErrorMessage) - 1, nullptr, ErrorMessage);
	BD_LOG_ERROR("Failed to compile shader loaded from path {}:", Path);
	BD_LOG_ERROR("{}", ErrorMessage);

	return std::nullopt;
}

Shader::~Shader()
{
	glDeleteProgram(m_Program);
}

std::unique_ptr<Shader> Shader::Create(std::string_view VertexShaderPath, std::string_view FragmentShaderPath)
{
	auto MaybeVertexShader = LoadAndCompileShader(GL_VERTEX_SHADER, VertexShaderPath);
	if (!MaybeVertexShader.has_value())
		return nullptr;
	auto VertexShader = MaybeVertexShader.value();

	auto MaybeFragmentShader = LoadAndCompileShader(GL_FRAGMENT_SHADER, FragmentShaderPath);
	if (!MaybeFragmentShader.has_value())
		return nullptr;
	auto FragmentShader = MaybeFragmentShader.value();

	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	GLint LinkStatus;
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &LinkStatus);
	if (LinkStatus)
		return std::unique_ptr<Shader>(new Shader(ShaderProgram));

	char ErrorMessage[2048];
	glGetProgramInfoLog(ShaderProgram, sizeof(ErrorMessage) - 1, nullptr, ErrorMessage);
	BD_LOG_ERROR("Failed to link shader program (vertex shader {}, fragment shader {})", VertexShaderPath, FragmentShaderPath);
	BD_LOG_ERROR("{}", ErrorMessage);

	return nullptr;
}

void Shader::Bind() const
{
	glUseProgram(m_Program);
}


Shader::Shader(GLuint Program)
	: m_Program(Program)
{
}
