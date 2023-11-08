#pragma once

#include "Renderer/Shader.h"

class Brush
{
public:
	virtual ~Brush() = default;

	virtual void Prepare() const {}

	virtual const Shader& GetShader() const = 0;
};
