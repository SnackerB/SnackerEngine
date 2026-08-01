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

	void GuiImage::computeSizeHints()
	{
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
			const Vec2i& preferredSize = getPreferredSize();
			Vec2i textureSize = texture.getSize();
			if (preferredSize.x == SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
				if (size.x == 0) return;
				double preferredHeightFactor = static_cast<double>(textureSize.y) / static_cast<double>(textureSize.x);
				int preferredHeight = std::ceil(preferredHeightFactor * static_cast<double>(size.x));
				if (preferredHeight > size.y) {
					if (preferredSize.y == size.y) setPreferredSize(Vec2i(size.x - 1, SIZE_HINT_AS_LARGE_AS_POSSIBLE));
					else {
						double preferredWidthFactor = static_cast<double>(textureSize.x) / static_cast<double>(textureSize.y);
						int preferredWidth = std::ceil(preferredWidthFactor * static_cast<double>(size.y));
						if (preferredWidth == 0) preferredWidth = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
						setPreferredSize(Vec2i(preferredWidth, SIZE_HINT_AS_LARGE_AS_POSSIBLE));
					}
				}
				else {
					setPreferredSize(Vec2i(SIZE_HINT_AS_LARGE_AS_POSSIBLE, preferredHeight));
				}
			}
			else if (preferredSize.y == SIZE_HINT_AS_LARGE_AS_POSSIBLE) {
				if (size.y == 0) return;
				double preferredWidthFactor = static_cast<double>(textureSize.x) / static_cast<double>(textureSize.y);
				int preferredWidth = std::ceil(preferredWidthFactor * static_cast<double>(size.y));
				if (preferredWidth > size.x) {
					if (preferredSize.x == size.x) setPreferredSize(Vec2i(SIZE_HINT_AS_LARGE_AS_POSSIBLE, size.y - 1));
					else {
						double preferredHeightFactor = static_cast<double>(textureSize.y) / static_cast<double>(textureSize.x);
						int preferredHeight = std::ceil(preferredHeightFactor * static_cast<double>(size.x));
						if (preferredHeight == 0) preferredHeight = SIZE_HINT_AS_LARGE_AS_POSSIBLE;
						setPreferredSize(Vec2i(SIZE_HINT_AS_LARGE_AS_POSSIBLE, preferredHeight));
					}
				}
				else {
					setPreferredSize(Vec2i(preferredWidth, SIZE_HINT_AS_LARGE_AS_POSSIBLE));
				}
			}
			else {
				if (texture.getSize().x > texture.getSize().y) setPreferredWidth(-2);
				else setPreferredHeight(-2);
			}
			break;
		}
		default:
			break;
		}
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

	GuiImage::GuiImage(const nlohmann::json& json, ParameterNameSet* parameterNameSet)
		: GuiPanel(json, parameterNameSet)
	{
		parseJson(texture, "texture", json, parameterNameSet);
		parseJson(imageShader, "imageShader", json, parameterNameSet);
		parseJson(guiImageMode, "guiImageMode", json, parameterNameSet);
		parseJson(guiImageSizeHintMode, "guiImageSizeHintMode", json, parameterNameSet);
		parseJson(rotationAngle, "rotationAngle", json, parameterNameSet);
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

	void GuiImage::setRotationAngle(double rotationAngle)
	{
		if (this->rotationAngle != rotationAngle) {
			this->rotationAngle = rotationAngle;
			onSizeChange();
		}
	}

	void GuiImage::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;;
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		// Draw background
		if (rotationAngle == 0.0f) GuiPanel::draw(worldPosition); // TODO: Currently we draw no background if the image is rotated. Fix this in the future by also drawing a rotated background!
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
		Vec2f translateVector{};
		Vec2f scaleVector{};
		switch (guiImageMode)
		{
		case SnackerEngine::GuiImage::GuiImageMode::RESIZE_TO_IMAGE_SIZE:
		{
			setMinSize(texture.getSize());
			setMaxSize(texture.getSize());
			setPreferredSize(texture.getSize());
			scaleVector = static_cast<Vec2f>(getSize());
			translateVector = Vec2f(0.0f, -scaleVector.y);
			break;
		}
		case SnackerEngine::GuiImage::GuiImageMode::FIT_IMAGE_TO_SIZE:
		{
			const Vec2i& size = getSize();
			scaleVector = texture.getSize();
			// Make texture as large as possible in x direction
			float factor = static_cast<float>(size.x) / static_cast<float>(scaleVector.x);
			scaleVector.y = scaleVector.y * factor;
			scaleVector.x = scaleVector.x * factor;
			// Make texture fit in y direction
			if (scaleVector.y > size.y) {
				factor = static_cast<float>(size.y) / static_cast<float>(scaleVector.y);
				scaleVector.x = scaleVector.x * factor;
				scaleVector.y = static_cast<float>(size.y);
			}
			// Clip
			scaleVector.x = std::min(static_cast<float>(size.x), scaleVector.x);
			scaleVector.y = std::min(static_cast<float>(size.y), scaleVector.y);
			translateVector = Vec2f(static_cast<float>(size.x) / 2.0f - scaleVector.x / 2.0f, -std::min(std::ceil(static_cast<float>(size.y) / 2.0f) + std::ceil(scaleVector.y / 2.0f), static_cast<float>(size.y)));
			// Set size hints
			computeSizeHints();
			break;
		}
		default:
			break;
		}
		modelMatrixImage = Mat4f::TranslateAndScale(Vec3f(translateVector), Vec3f(scaleVector));
		if (rotationAngle != 0.0) {
			// To rotate correctly, first center, then rotate, then move back (anchor is top left corner)
			modelMatrixImage =
				Mat4f::Translate(Vec3f(scaleVector.x / 2.0f, -scaleVector.y / 2.0f, 0.0f)) *
				Mat4f::RotateZ(rotationAngle) *
				Mat4f::Translate(Vec3f(-scaleVector.x / 2.0f, scaleVector.y / 2.0f, 0.0f)) *
				modelMatrixImage;
		}
	}

	std::unique_ptr<GuiElementAnimatable> GuiImage::animateRotationAngle(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiImageRotationAngleAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiImage*>(element)->setRotationAngle(currentVal); };
		public:
			GuiImageRotationAngleAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiImageRotationAngleAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}

}