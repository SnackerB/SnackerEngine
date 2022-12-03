#include "Gui/GuiElements/GuiImage.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{
	void GuiImage::draw(const Vec2i& parentPosition)
	{
		if (!guiManager) return;;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(parentPosition.x), static_cast<float>(-parentPosition.y), 0.0f));
		if (backgroundColor.alpha > 0.0f && guiImageMode != GuiImageMode::RESIZE_TO_IMAGE_SIZE) {
			// Draw background
			backgroundShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(backgroundShader);
			backgroundShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixBackground);
			backgroundShader.setUniform<Color4f>("u_color", backgroundColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		// Draw image
		imageShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(imageShader);
		imageShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixImage);
		texture.bind();
		Renderer::draw(guiManager->getModelSquare());
		// Draw children
		pushClippingBox(parentPosition);
		GuiElement::draw(parentPosition);
		popClippingBox();
	}
	
	void GuiImage::onSizeChange()
	{
		switch (guiImageMode)
		{
		case SnackerEngine::GuiImage::GuiImageMode::RESIZE_TO_IMAGE_SIZE: 
		{
			setSizeInternal(texture.getSize());
			const Vec2i& position = getPosition();
			const Vec2i& size = getSize();
			modelMatrixImage = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f),
				Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		}
		case SnackerEngine::GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE:
		{
			const Vec2i& position = getPosition();
			const Vec2i& size = getSize();
			modelMatrixBackground = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(position.x), static_cast<float>(-position.y - size.y), 0.0f),
				Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
			Vec2i textureSize = texture.getSize();
			// Make texture as large as possible in x direction
			float factor = static_cast<float>(size.x) / static_cast<float>(textureSize.x);
			textureSize.y *= factor;
			textureSize.x = size.x;
			// Make texture fit in y direction
			if (textureSize.y > size.y) {
				factor = static_cast<float>(size.y) / static_cast<float>(textureSize.y);
				textureSize.x *= factor;
				textureSize.y = size.y;
			}
			modelMatrixImage = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(position.x + size.x / 2 - textureSize.x / 2), static_cast<float>(-position.y - std::min(std::ceil(size.y / 2) + std::ceil(textureSize.y / 2), static_cast<double>(size.y))), 0.0f),
				Vec3f(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y), 0.0f));
		}
		default:
			break;
		}
	}
	
	GuiImage::GuiImage(const Vec2i& position, const Vec2i& size, const ResizeMode& resizeMode, const Color4f& backgroundColor, const Texture& texture, const GuiImageMode& guiImageMode)
		: GuiElement(position, size, resizeMode), modelMatrixBackground{}, modelMatrixImage{}, 
		backgroundShader("shaders/gui/simpleTransparentColor.shader"), imageShader("shaders/gui/basicTexture.shader"),
		backgroundColor(backgroundColor), texture(texture),
		guiImageMode(guiImageMode)
	{
		onSizeChange();
	}
	
	GuiImage::GuiImage(const GuiStyle& style, const Texture& texture)
		: GuiImage(Vec2i(), Vec2i(), ResizeMode::DO_NOT_RESIZE, style.guiImageBackgroundColor, texture, style.guiImageGuiImageMode) {}
	
	const Color4f& GuiImage::getBackgroundColor() const
	{
		return backgroundColor;
	}
	
	const Texture& GuiImage::getTexture() const
	{
		return texture;
	}
	
	const GuiImage::GuiImageMode& GuiImage::getGuImageMode() const
	{
		return guiImageMode;
	}

	void GuiImage::setBackgroundColor(const Color4f& backgroundColor)
	{
		this->backgroundColor = backgroundColor;
	}
	
	void GuiImage::setTexture(const Texture& texture)
	{
		this->texture = texture;
		onSizeChange();
	}
	
	void GuiImage::setGuiImageMode(const GuiImageMode& guiImageMode)
	{
		if (this->guiImageMode != guiImageMode) {
			this->guiImageMode = guiImageMode;
			onSizeChange();
		}
	}
	
	GuiImage::GuiImage(const GuiImage& other) noexcept
		: GuiElement(other), modelMatrixBackground(other.modelMatrixBackground), 
		modelMatrixImage(other.modelMatrixImage), backgroundShader(other.backgroundShader), 
		imageShader(other.imageShader), backgroundColor(other.backgroundColor), 
		texture(other.texture), guiImageMode(other.guiImageMode)
	{
	}
	
	GuiImage& GuiImage::operator=(const GuiImage& other) noexcept
	{
		GuiElement::operator=(other);
		modelMatrixImage = other.modelMatrixBackground;
		modelMatrixImage = other.modelMatrixImage;
		backgroundShader = other.backgroundShader;
		imageShader = other.imageShader;
		backgroundColor = other.backgroundColor;
		texture = other.texture;
		guiImageMode = other.guiImageMode;
		return *this;
	}
	
	GuiImage::GuiImage(GuiImage&& other) noexcept
		: GuiElement(std::move(other)), modelMatrixBackground(other.modelMatrixBackground),
		modelMatrixImage(other.modelMatrixImage), backgroundShader(std::move(other.backgroundShader)),
		imageShader(std::move(other.imageShader)), backgroundColor(other.backgroundColor),
		texture(std::move(other.texture)), guiImageMode(other.guiImageMode)
	{
	}
	
	GuiImage& GuiImage::operator=(GuiImage&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		modelMatrixImage = other.modelMatrixBackground;
		modelMatrixImage = other.modelMatrixImage;
		backgroundShader = std::move(other.backgroundShader);
		imageShader = std::move(other.imageShader);
		backgroundColor = other.backgroundColor;
		texture = std::move(other.texture);
		guiImageMode = other.guiImageMode;
		return *this;
	}
	
	GuiImage::~GuiImage()
	{
	}

}