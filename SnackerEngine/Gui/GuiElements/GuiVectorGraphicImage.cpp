#include "Gui\GuiElements\GuiVectorGraphicImage.h"
#include "Gui\GuiManager.h"
#include "Graphics\Renderer.h"

namespace SnackerEngine
{

	Shader GuiVectorGraphicImage::defaultVectorGraphicImageShader{};

	GuiVectorGraphicImage::GuiVectorGraphicImage(Color4f color, Texture texture, GuiImageMode imageMode, GuiImageSizeHintMode sizeHintMode)
		: GuiImage(texture, imageMode, sizeHintMode)
	{
		imageShader = defaultVectorGraphicImageShader;
	}

	GuiVectorGraphicImage::GuiVectorGraphicImage(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiImage(json, parameterNameSet)
	{
		if (!json.contains("imageShader")) imageShader = defaultVectorGraphicImageShader;
		parseJson(color, "color", json, parameterNameSet);
	}

	void GuiVectorGraphicImage::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		// Draw background
		if (getRotationAngle() == 0.0f) GuiPanel::draw(worldPosition); // TODO: Currently we draw noe background if the image is rotated. Fix this in the future by also drawing a rotated background!
		// Draw image
		imageShader.bind();
		guiManager->setUniformViewAndProjectionMatrices(imageShader);
		imageShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixImage);
		imageShader.setUniform<Color4f>("u_color", color);
		imageShader.setUniform<float>("u_pxRange", 4.0);
		imageShader.setUniform<int>("u_msdf", 0);
		texture.bind();
		Renderer::draw(guiManager->getModelSquare());
	}

}