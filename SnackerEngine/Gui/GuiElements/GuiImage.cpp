#include "GuiImage.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Utility\Json.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Shader GuiImage::defaultImageShader{};
	Texture GuiImage::defaultTexture{};
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiImage::GuiImageMode> tag)
	{
		if (!json.is_string()) return false;
		std::string temp = static_cast<std::string>(json);
		if (temp == "RESIZE_TO_IMAGE_SIZE"
			|| temp == "FIT_IMAGE_TO_SIZE") return true;
		return false;
	}
	//------------------------------------------------------------------------------------------------------
	template<> GuiImage::GuiImageMode parseJSON(const nlohmann::json& json, JsonTag<GuiImage::GuiImageMode> tag)
	{
		std::string temp = static_cast<std::string>(json);
		if (temp == "RESIZE_TO_IMAGE_SIZE") return GuiImage::GuiImageMode::RESIZE_TO_IMAGE_SIZE;
		else if (temp == "FIT_IMAGE_TO_SIZE") return GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE;
		return GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE;
	}
	//------------------------------------------------------------------------------------------------------
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiImage::GuiImageSizeHintMode> tag)
	{
		if (!json.is_string()) return false;
		std::string temp = static_cast<std::string>(json);
		if (temp == "ARBITRARY"
			|| temp == "COMPUTE_PREFERRED_HEIGHT_FROM_WIDTH"
			|| temp == "COMPUTE_PREFERRED_WIDTH_FROM_HEIGHT"
			|| temp == "DYNAMIC") return true;
		return false;
	}
	//------------------------------------------------------------------------------------------------------
	template<> GuiImage::GuiImageSizeHintMode parseJSON(const nlohmann::json& json, JsonTag<GuiImage::GuiImageSizeHintMode> tag)
	{
		std::string temp = static_cast<std::string>(json);
		if (temp == "ARBITRARY") return GuiImage::GuiImageSizeHintMode::ARBITRARY;
		else if (temp == "COMPUTE_PREFERRED_HEIGHT_FROM_WIDTH") return GuiImage::GuiImageSizeHintMode::COMPUTE_PREFERRED_HEIGHT_FROM_WIDTH;
		else if (temp == "COMPUTE_PREFERRED_WIDTH_FROM_HEIGHT") return GuiImage::GuiImageSizeHintMode::COMPUTE_PREFERRED_WIDTH_FROM_HEIGHT;
		else if (temp == "DYNAMIC") return GuiImage::GuiImageSizeHintMode::DYNAMIC;
		return GuiImage::GuiImageSizeHintMode::ARBITRARY;
	}

	GuiImage::GuiImage(Texture texture, GuiImageMode imageMode, GuiImageSizeHintMode sizeHintMode)
		: texture(texture), guiImageMode(guiImageMode), guiImageSizeHintMode(sizeHintMode)
	{
		if (guiImageSizeHintMode == GuiImageSizeHintMode::DYNAMIC) {
			if (texture.getSize().x > texture.getSize().y) tryForLargestWidth = true;
			else tryForLargestWidth = false;
		}
		onSizeChange();
	}

	GuiImage::GuiImage(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiPanel(json, data, parameterNames)
	{
		parseJsonOrReadFromData(texture, "texture", json, data, parameterNames);
		parseJsonOrReadFromData(imageShader, "imageShader", json, data, parameterNames);
		parseJsonOrReadFromData(guiImageMode, "guiImageMode", json, data, parameterNames);
		parseJsonOrReadFromData(guiImageSizeHintMode, "guiImageSizeHintMode", json, data, parameterNames);
		if (guiImageSizeHintMode == GuiImageSizeHintMode::DYNAMIC) {
			if (texture.getSize().x > texture.getSize().y) tryForLargestWidth = true;
			else tryForLargestWidth = false;
		}
		onSizeChange();
	}

	void GuiImage::setImageTexture(const Texture& texture)
	{
		this->texture = texture; 
		if (guiImageSizeHintMode == GuiImageSizeHintMode::DYNAMIC) {
			if (texture.getSize().x > texture.getSize().y) tryForLargestWidth = true;
			else tryForLargestWidth = false;
		}
		onSizeChange();
	}

	void GuiImage::setGuiImageSizeHintMode(GuiImageSizeHintMode guiImageSizeHintMode)
	{
		if (this->guiImageSizeHintMode != guiImageSizeHintMode) {
			this->guiImageSizeHintMode = guiImageSizeHintMode;
			if (guiImageSizeHintMode == GuiImageSizeHintMode::DYNAMIC) {
				if (texture.getSize().x > texture.getSize().y) tryForLargestWidth = true;
				else tryForLargestWidth = false;
			}
			onSizeChange();
		}
	}

	void GuiImage::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		// Draw background
		GuiPanel::draw(worldPosition);
		// Draw image
		imageShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(imageShader);
		imageShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixImage);
		texture.bind();
		Renderer::draw(guiManager->getModelSquare());
	}

	void GuiImage::onSizeChange()
	{
		GuiPanel::onSizeChange();
		switch (guiImageMode)
		{
		case SnackerEngine::GuiImage::GuiImageMode::RESIZE_TO_IMAGE_SIZE:
		{
			setMinSize(texture.getSize());
			setMaxSize(texture.getSize());
			setPreferredSize(texture.getSize());
			const Vec2i& position = getPosition();
			const Vec2i& size = getSize();
			modelMatrixImage = Mat4f::TranslateAndScale(
				Vec3f(0.0f, static_cast<float>(-size.y), 0.0f),
				Vec3f(static_cast<float>(size.x), static_cast<float>(size.y), 0.0f));
		}
		case SnackerEngine::GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE:
		{
			const Vec2i& position = getPosition();
			const Vec2i& size = getSize();
			Vec2i textureSize = texture.getSize();
			// Make texture as large as possible in x direction
			float factor = static_cast<float>(size.x) / static_cast<float>(textureSize.x);
			textureSize.y = static_cast<int>(textureSize.y * factor);
			textureSize.x = static_cast<int>(textureSize.x * factor);
			// Make texture fit in y direction
			if (textureSize.y > size.y) {
				factor = static_cast<float>(size.y) / static_cast<float>(textureSize.y);
				textureSize.x = static_cast<int>(textureSize.x * factor);
				textureSize.y = size.y;
			}
			// Clip
			textureSize.x = std::min(size.x, textureSize.x);
			textureSize.y = std::min(size.y, textureSize.y);
			modelMatrixImage = Mat4f::TranslateAndScale(
				Vec3f(static_cast<float>(size.x) / 2.0f - static_cast<float>(textureSize.x) / 2.0f, -std::min(std::ceil(static_cast<float>(size.y) / 2.0f) + std::ceil(static_cast<float>(textureSize.y) / 2.0f), static_cast<float>(size.y)), 0.0f),
				Vec3f(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y), 0.0f));
			// Set size hints
			switch (guiImageSizeHintMode)
			{
			case SnackerEngine::GuiImage::GuiImageSizeHintMode::ARBITRARY:
				break;
			case SnackerEngine::GuiImage::GuiImageSizeHintMode::COMPUTE_PREFERRED_HEIGHT_FROM_WIDTH:
			{
				const Vec2i& size = getSize();
				Vec2i textureSize = texture.getSize();
				if (textureSize.x == 0) return;
				double factor = static_cast<double>(textureSize.y) / static_cast<double>(textureSize.x);
				setPreferredHeight(std::ceil(factor * static_cast<double>(size.x)));
				break; 
			}
			case SnackerEngine::GuiImage::GuiImageSizeHintMode::COMPUTE_PREFERRED_WIDTH_FROM_HEIGHT:
			{
				const Vec2i& size = getSize();
				Vec2i textureSize = texture.getSize();
				if (textureSize.y == 0) return;
				double factor = static_cast<double>(textureSize.x) / static_cast<double>(textureSize.y);
				setPreferredWidth(std::ceil(factor * static_cast<double>(size.y)));
				break;
			}
			case SnackerEngine::GuiImage::GuiImageSizeHintMode::DYNAMIC:
			{
				const Vec2i& size = getSize();
				Vec2i textureSize = texture.getSize();
				if (textureSize.x == 0 || textureSize.y == 0) return;
				if (tryForLargestWidth) {
					// We are currently trying to get the image to be as large as possible along the x direction
					// Start by checking if the image size is now bottlenecked by the y direction.
					double preferredHeightFactor = static_cast<double>(textureSize.y) / static_cast<double>(textureSize.x);
					int preferredHeight = std::ceil(preferredHeightFactor * static_cast<double>(size.x));
					if (preferredHeight > size.y) {
						// We are bottlenecked by the y direction. We therefore need to switch to get the image to
						// be as large as possible along the y direction!
						tryForLargestWidth = false;
						double preferredWidthFactor = static_cast<double>(textureSize.x) / static_cast<double>(textureSize.y);
						int preferredWidth = std::ceil(preferredWidthFactor * static_cast<double>(size.y));
						setPreferredWidth(preferredWidth);
						setPreferredHeight(-2);
						registerEnforceLayoutUp();
					}
					else {
						// We are still bottlenecked by the x direction
						setPreferredWidth(-2);
						setPreferredHeight(preferredHeight);
					}
				}
				else {
					// We are currently trying to get the image to be as large as possible along the y direction
					// Start by checking if the image size is now bottlenecked by the x direction.
					double preferredWidthFactor = static_cast<double>(textureSize.x) / static_cast<double>(textureSize.y);
					int preferredWidth = std::ceil(preferredWidthFactor * static_cast<double>(size.y));
					if (preferredWidth > size.x) {
						// We are bottlenecked by the x direction. We therefore need to switch to get the image to
						// be as large as possible along the x direction!
						tryForLargestWidth = true;
						double preferredHeightFactor = static_cast<double>(textureSize.y) / static_cast<double>(textureSize.x);
						int preferredHeight = std::ceil(preferredHeightFactor * static_cast<double>(size.x));
						setPreferredWidth(-2);
						setPreferredHeight(preferredHeight);
						registerEnforceLayoutUp();
					}
					else {
						// We are still bottlenecked by the x direction
						setPreferredWidth(preferredWidth);
						setPreferredHeight(-2);
					}
				}
				break;
			}
			default:
				break;
			}
		}
		default:
			break;
		}
	}

}