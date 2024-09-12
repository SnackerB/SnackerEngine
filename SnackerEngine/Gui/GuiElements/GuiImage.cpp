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

	GuiImage::GuiImage(Texture texture, GuiImageMode imageMode)
		: texture(texture), guiImageMode(guiImageMode)
	{
		onSizeChange();
	}

	GuiImage::GuiImage(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiPanel(json, data, parameterNames)
	{
		parseJsonOrReadFromData(texture, "texture", json, data, parameterNames);
		parseJsonOrReadFromData(imageShader, "imageShader", json, data, parameterNames);
		parseJsonOrReadFromData(guiImageMode, "guiImageMode", json, data, parameterNames);
		onSizeChange();
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
		}
		default:
			break;
		}
	}

}