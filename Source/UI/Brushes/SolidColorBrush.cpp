#include "SolidColorBrush.h"

std::unique_ptr<SolidColorBrush> SolidColorBrush::Create(glm::vec4 Color)
{
	return std::unique_ptr<SolidColorBrush>(new SolidColorBrush(Color));
}

void SolidColorBrush::Prepare() const
{
	Brush::Prepare();

	m_Shader->SetUniform("u_Color", m_Color);
}

const Shader& SolidColorBrush::GetShader() const
{
	return *m_Shader;
}

SolidColorBrush::SolidColorBrush(glm::vec4 Color)
	: m_Color(Color)
{
	m_Shader = Shader::Create("Resources/Shaders/SolidColorBrush.vert", "Resources/Shaders/SolidColorBrush.frag");
	BD_ASSERT(m_Shader);
}
