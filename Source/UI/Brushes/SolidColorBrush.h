#pragma once

#include "UI/Brush.h"

class SolidColorBrush : public Brush
{
public:
	static std::unique_ptr<SolidColorBrush> Create(glm::vec4 Color);

	virtual void Prepare() const override;

	virtual const Shader& GetShader() const override;

private:
	glm::vec4 m_Color;

	std::unique_ptr<Shader> m_Shader;

	explicit SolidColorBrush(glm::vec4 Color);
};