#include "Image.h"

std::unique_ptr<Image> Image::LoadFromFile(std::string_view Path)
{
	auto Texture = Texture::Load(Path);
	BD_ASSERT(Texture); // FIXME: handle this case more nicely
	return std::unique_ptr<Image>(new Image(std::move(Texture)));
}

glm::vec2 Image::ComputePreferredSize() const
{
	return { m_Texture->Dimensions() };
}

void Image::Render(RenderBuffer& Buffer) const
{
	Widget::Render(Buffer);

	float ExpectedAspectRatio = static_cast<float>(m_Texture->Dimensions().x) / static_cast<float>(m_Texture->Dimensions().y);
	float ActualAspectRatio = BoundingBox().Width() / BoundingBox().Height();

	Rect2D Rect = BoundingBox();
	if (ActualAspectRatio > ExpectedAspectRatio)
	{
		float ActualTextureWidth = Rect.Height() * ExpectedAspectRatio;
		float Slack = BoundingBox().Width() - ActualTextureWidth;
		Rect.Left() += Slack / 2.0f;
		Rect.Right() -= Slack / 2.0f;
	}
	if (ActualAspectRatio < ExpectedAspectRatio)
	{
		float ActualTextureHeight = Rect.Width() / ExpectedAspectRatio;
		float Slack = BoundingBox().Height() - ActualTextureHeight;
		Rect.Top() -= Slack / 2.0f;
		Rect.Bottom() += Slack / 2.0f;
	}

	Buffer.TextureRect(Rect, *m_Texture);
}

Image::Image(std::unique_ptr<Texture> Texture)
	: m_Texture(std::move(Texture))
{
}
