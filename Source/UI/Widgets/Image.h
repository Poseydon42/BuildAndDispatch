#pragma once

#include "Renderer/Texture.h"
#include "UI/Widget.h"

class Image : public Widget
{
public:
	static std::unique_ptr<Image> LoadFromFile(std::string_view Path);

	virtual glm::vec2 ComputePreferredSize() const override;

	virtual void Render(RenderBuffer& Buffer) const override;

private:
	std::unique_ptr<Texture> m_Texture;

	explicit Image(std::unique_ptr<Texture> Texture);
};