#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Utility\Keys.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiVerticalScrollingListLayout::defaultBackgroundColor = Color4f(0.0f, 0.0f);
	unsigned GuiVerticalScrollingListLayout::defaultScrollBarWidth = 23;
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarColor = Color4f(0.0f, 0.0f, 1.0f, 1.0f);
	Color4f GuiVerticalScrollingListLayout::defaultScrollbarBackgroundColor = Color4f(0.0f, 1.0f);
	unsigned GuiVerticalScrollingListLayout::defaultScrollbarBorderRight = 10;
	unsigned GuiVerticalScrollingListLayout::defaultScrollbarBorderTop = 10;
	unsigned GuiVerticalScrollingListLayout::defaultScrollbarBorderBottom = 10;
	float GuiVerticalScrollingListLayout::defaultScrollSpeed = 50.0f;
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(Color4f backgroundColor, Color4f scrollbarColor, Color4f scrollbarBackgroundColor)
		: GuiVerticalListLayout(backgroundColor), scrollbarColor(scrollbarColor), scrollbarBackgroundColor(scrollbarBackgroundColor) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiVerticalListLayout(json, data, parameterNames)
	{
		parseJsonOrReadFromData(scrollSpeed, "scrollSpeed", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBackgroundColor, "scrollbarBackgroundColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarColor, "scrollbarColor", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarWidth, "scrollbarWidth", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBorderRight, "scrollbarBorderRight", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBorderTop, "scrollbarBorderTop", json, data, parameterNames);
		parseJsonOrReadFromData(scrollbarBorderBottom, "scrollbarBorderBottom", json, data, parameterNames);
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(const GuiVerticalScrollingListLayout& other) noexcept
		: GuiVerticalListLayout(other), currentVerticalHeight(other.currentVerticalHeight), currentVerticalOffset(other.currentVerticalOffset),
		scrollSpeed(other.scrollSpeed), drawScrollBar(other.drawScrollBar), visiblePercentage(other.visiblePercentage), 
		offsetPercentage(other.offsetPercentage), scrollbarBackgroundColor(other.scrollbarBackgroundColor), 
		scrollbarColor(other.scrollbarColor), modelMatrixScrollbar{}, modelMatrixScrollbarBackground{},
		scrollbarWidth(other.scrollbarWidth), 
		scrollbarBorderRight(other.scrollbarBorderRight), scrollbarBorderTop(other.scrollbarBorderTop),
		scrollbarBorderBottom(other.scrollbarBorderBottom), mouseOffsetFromScrollBar(0.0f),
		firstVisibleElement(other.firstVisibleElement), lastVisibleElement(other.lastVisibleElement) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout& GuiVerticalScrollingListLayout::operator=(const GuiVerticalScrollingListLayout& other) noexcept
	{
		GuiVerticalListLayout::operator=(other);
		currentVerticalHeight = other.currentVerticalHeight;
		currentVerticalOffset = other.currentVerticalOffset;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = other.drawScrollBar;
		visiblePercentage = other.visiblePercentage;
		offsetPercentage = other.offsetPercentage; 
		scrollbarBackgroundColor = other.scrollbarBackgroundColor;
		scrollbarColor = other.scrollbarColor;
		modelMatrixScrollbar = Mat4f();
		modelMatrixScrollbarBackground = Mat4f();
		scrollbarWidth = other.scrollbarWidth;
		scrollbarBorderRight = other.scrollbarBorderRight;
		scrollbarBorderTop = other.scrollbarBorderTop;
		scrollbarBorderBottom = other.scrollbarBorderBottom;
		mouseOffsetFromScrollBar = 0.0f; 
		firstVisibleElement = other.firstVisibleElement;
		lastVisibleElement = other.lastVisibleElement;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout::GuiVerticalScrollingListLayout(GuiVerticalScrollingListLayout&& other) noexcept
		: GuiVerticalListLayout(std::move(other)), currentVerticalHeight(other.currentVerticalHeight), currentVerticalOffset(other.currentVerticalOffset),
		scrollSpeed(other.scrollSpeed), drawScrollBar(other.drawScrollBar), visiblePercentage(other.visiblePercentage),
		offsetPercentage(other.offsetPercentage), scrollbarBackgroundColor(other.scrollbarBackgroundColor),
		scrollbarColor(other.scrollbarColor), modelMatrixScrollbar(other.modelMatrixScrollbar), 
		modelMatrixScrollbarBackground(other.modelMatrixScrollbarBackground),
		scrollbarWidth(other.scrollbarWidth), 
		scrollbarBorderRight(other.scrollbarBorderRight), scrollbarBorderTop(other.scrollbarBorderTop),
		scrollbarBorderBottom(other.scrollbarBorderBottom), mouseOffsetFromScrollBar(0.0f),
		firstVisibleElement(other.firstVisibleElement), lastVisibleElement(other.lastVisibleElement) {}
	//--------------------------------------------------------------------------------------------------
	GuiVerticalScrollingListLayout& GuiVerticalScrollingListLayout::operator=(GuiVerticalScrollingListLayout&& other) noexcept
	{
		GuiVerticalListLayout::operator=(std::move(other));
		currentVerticalHeight = other.currentVerticalHeight;
		currentVerticalOffset = other.currentVerticalOffset;
		scrollSpeed = other.scrollSpeed;
		drawScrollBar = other.drawScrollBar;
		visiblePercentage = other.visiblePercentage;
		offsetPercentage = other.offsetPercentage;
		scrollbarBackgroundColor = other.scrollbarBackgroundColor;
		scrollbarColor = other.scrollbarColor;
		modelMatrixScrollbar = other.modelMatrixScrollbar;
		modelMatrixScrollbarBackground = other.modelMatrixScrollbarBackground;
		scrollbarWidth = other.scrollbarWidth;
		scrollbarBorderRight = other.scrollbarBorderRight;
		scrollbarBorderTop = other.scrollbarBorderTop;
		scrollbarBorderBottom = other.scrollbarBorderBottom;
		mouseOffsetFromScrollBar = 0.0f;
		firstVisibleElement = other.firstVisibleElement;
		lastVisibleElement = other.lastVisibleElement;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::setTotalOffsetPercentage(float totalOffsetPercentage)
	{
		float newOffsetPercentage = totalOffsetPercentage * (1.0f - visiblePercentage);
		if (offsetPercentage != newOffsetPercentage) {
			offsetPercentage = newOffsetPercentage;
			computeScrollBar();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::setScrollbarBorderRight(unsigned scrollbarBorderRight)
	{
		if (this->scrollbarBorderRight != scrollbarBorderRight) {
			this->scrollbarBorderRight = scrollbarBorderRight;
			computeScrollbarModelMatrices();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::setScrollbarBorderTop(unsigned scrollbarBorderTop)
	{
		if (this->scrollbarBorderTop != scrollbarBorderTop) {
			this->scrollbarBorderTop = scrollbarBorderTop;
			computeScrollbarModelMatrices();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::setScrollbarBorderBottom(unsigned scrollbarBorderBottom)
	{
		if (this->scrollbarBorderBottom != scrollbarBorderBottom) {
			this->scrollbarBorderBottom = scrollbarBorderBottom;
			computeScrollbarModelMatrices();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollSpeed(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollSpeedAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollSpeed(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollSpeedAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollSpeedAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateTotalOffsetPercentage(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutTotalOffsetPercentageAnimatable : public GuiElementValueAnimatable<float>
		{
			virtual void onAnimate(const float& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setTotalOffsetPercentage(currentVal); }
		public:
			GuiVerticalScrollingListLayoutTotalOffsetPercentageAnimatable(GuiElement& element, const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<float>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutTotalOffsetPercentageAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollbarBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollbarBackgroundColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollbarBackgroundColor(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollbarBackgroundColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollbarBackgroundColorAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollbarColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollbarColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollbarColor(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollbarColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollbarColorAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollbarBorderRight(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollBarBorderRightAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollbarBorderRight(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollBarBorderRightAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollBarBorderRightAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollbarBorderTop(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollBarBorderTopAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollbarBorderTop(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollBarBorderTopAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollBarBorderTopAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::animateScrollbarBorderBottom(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiVerticalScrollingListLayoutScrollBarBorderBottomAnimatable : public GuiElementValueAnimatable<unsigned>
		{
			virtual void onAnimate(const unsigned& currentVal) override { if (element) static_cast<GuiVerticalScrollingListLayout*>(element)->setScrollbarBorderBottom(currentVal); }
		public:
			GuiVerticalScrollingListLayoutScrollBarBorderBottomAnimatable(GuiElement& element, const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<unsigned>(element, startVal, stopVal, duration, animationFunction) {}
		};
		animate(std::make_unique<GuiVerticalScrollingListLayoutScrollBarBorderBottomAnimatable>(*this, startVal, stopVal, duration, animationFunction));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::computeScrollBar()
	{
		if (static_cast<int>(currentVerticalHeight) <= getHeight()) {
			visiblePercentage = 1.0f;
			offsetPercentage = 0.0f;
			currentVerticalOffset = 0;
			drawScrollBar = false;
			firstVisibleElement = 0;
			lastVisibleElement = static_cast<int>(getChildren().size()) - 1;
		}
		else {
			visiblePercentage = float(getHeight()) / float(currentVerticalHeight);
			offsetPercentage = std::max(0.0f, std::min(offsetPercentage, float(currentVerticalHeight - getHeight()) / float(currentVerticalHeight)));
			currentVerticalOffset = static_cast<int>(std::roundf(offsetPercentage * static_cast<float>(currentVerticalHeight)));
			drawScrollBar = true;
			computeScrollbarModelMatrices();
			computeFirstAndLastVisibleElements();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::computeScrollbarModelMatrices()
	{
		float scrollbarPositionX = static_cast<float>(getWidth() - scrollbarBorderRight - scrollbarWidth);
		float scrollbarBackgroundPositionY = -static_cast<float>(getHeight()) + static_cast<float>(scrollbarBorderBottom);
		float scrollbarBackgroundHeight = std::max(0.0f, static_cast<float>(getHeight() - static_cast<int>(scrollbarBorderTop) - static_cast<int>(scrollbarBorderBottom)));
		modelMatrixScrollbarBackground = Mat4f::TranslateAndScale(Vec2f(scrollbarPositionX, scrollbarBackgroundPositionY), Vec2f(static_cast<float>(scrollbarWidth), scrollbarBackgroundHeight));
		modelMatrixScrollbar = Mat4f::TranslateAndScale(Vec2f(scrollbarPositionX, scrollbarBackgroundPositionY + (1.0f - offsetPercentage) * scrollbarBackgroundHeight - visiblePercentage * scrollbarBackgroundHeight), Vec2f(static_cast<float>(scrollbarWidth), visiblePercentage * scrollbarBackgroundHeight));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::computeFirstAndLastVisibleElements()
	{
		/// Compute first visible element
		const auto& children = getChildren();
		firstVisibleElement = static_cast<int>(children.size() - 1);
		for (unsigned i = 0; i < children.size(); ++i) {
			GuiElement* element = getElement(children[i]);
			if (!element) continue;
			if (element->getPositionY() + element->getHeight() >= currentVerticalOffset) {
				firstVisibleElement = i;
				break;
			}
		}
		/// Compute last visible element
		lastVisibleElement = static_cast<int>(children.size() - 1);
		for (unsigned i = firstVisibleElement + 1; i < children.size(); ++i) {
			GuiElement* element = getElement(children[i]);
			if (!element) continue;
			if (element->getPositionY() > currentVerticalOffset + getHeight()) {
				lastVisibleElement = i - 1;
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		drawBackground(worldPosition);
		pushClippingBox(worldPosition);
		// Draw children
		const auto& children = getChildren();
		for (unsigned int i = firstVisibleElement; i < std::min(static_cast<std::size_t>(lastVisibleElement + 1), children.size()); ++i) {
			GuiElement* child = getElement(children[i]);
			if (child) drawElement(children[i], worldPosition + child->getPosition() - Vec2i(0, static_cast<int>(currentVerticalOffset)));
		}
		// Draw scrollBar
		if (drawScrollBar) {
			Shader& scrollbarShader = getBackgroundShader();
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			scrollbarShader.bind();
			guiManager->setUniformViewAndProjectionMatrices(scrollbarShader);
			scrollbarShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollbarBackground);
			scrollbarShader.setUniform<Color4f>("u_color", scrollbarBackgroundColor);
			Renderer::draw(guiManager->getModelSquare());
			scrollbarShader.setUniform<Mat4f>("u_model", translationMatrix * modelMatrixScrollbar);
			scrollbarShader.setUniform<Color4f>("u_color", scrollbarColor);
			Renderer::draw(guiManager->getModelSquare());
		}
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::onRegister()
	{
		GuiVerticalListLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_SCROLL_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::enforceLayout()
	{
		GuiVerticalLayout::enforceLayout();
		// Go through all children, positioning them and resizing them to their preferredHeights/minHeights!
		int currentYOffset = getVerticalBorder();
		const auto& children = getChildren();
		for (GuiID childID : children) {
			GuiElement* child = getElement(childID);
			if (!child) continue;			
			int childHeight = child->getPreferredHeight();
			if (childHeight == -1) childHeight = child->getMinHeight();
			if (childHeight == -1) childHeight = 0;
			// Set position
			setPositionYOfChild(childID, currentYOffset);
			// Resize
			setHeightOfChild(childID, childHeight);
			// Advance to next element
			currentYOffset += childHeight + getVerticalBorder();
		}
		currentVerticalHeight = currentYOffset;
		computeScrollBar();
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiVerticalScrollingListLayout::getChildOffset(const GuiID& childID) const
	{
		GuiElement* child = getElement(childID);
		if (!child) return Vec2i{};
		Vec2i result = child->getPosition();
		result.y -= static_cast<int>(std::floor(currentVerticalOffset));
		return result;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiVerticalScrollingListLayout::isColliding(const Vec2i& offset) const
	{
		if (offset.x >= 0 && offset.x <= getWidth() &&
			offset.y >= 0 && offset.y <= getHeight()) {
			if (drawScrollBar) {
				// Check if we are colliding with the scroll bar
				Vec2i size = getSize();
				int scrollBarBackgroundHeight = size.y - static_cast<int>(scrollbarBorderTop + scrollbarBorderBottom);
				if (offset.x >= size.x - static_cast<int>(scrollbarBorderRight) - static_cast<int>(scrollbarWidth) &&
					offset.x <= size.x - static_cast<int>(scrollbarBorderRight) &&
					offset.y >= static_cast<int>(scrollbarBorderTop) &&
					offset.y <= static_cast<int>(scrollbarBorderTop) + scrollBarBackgroundHeight) {
					return IsCollidingResult::COLLIDE_STRONG;
				}
			}
			return IsCollidingResult::COLLIDE_CHILD;
		}
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::GuiID GuiVerticalScrollingListLayout::getCollidingChild(const Vec2i& offset) const
	{
		return GuiLayout::getCollidingChild(Vec2i(offset.x, offset.y + static_cast<int>(std::floor(currentVerticalOffset))));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::callbackMouseMotion(const Vec2d& position)
	{
		Vec2i mouseOffset = getMouseOffset();
		Vec2i size = getSize();
		int scrollBarBackgroundHeight = size.y - static_cast<int>(scrollbarBorderTop + scrollbarBorderBottom);
		if (scrollBarBackgroundHeight == 0.0f) return;
		offsetPercentage = (mouseOffset.y - mouseOffsetFromScrollBar - scrollbarBorderTop) / scrollBarBackgroundHeight;
		currentVerticalOffset = static_cast<int>(currentVerticalHeight * offsetPercentage);
		computeScrollBar();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i mouseOffset = getMouseOffset();
			Vec2i size = getSize();
			int scrollBarBackgroundHeight = size.y - static_cast<int>(scrollbarBorderTop + scrollbarBorderBottom);
			if (mouseOffset.y >= scrollbarBorderTop + scrollBarBackgroundHeight * offsetPercentage &&
				mouseOffset.y <= scrollbarBorderTop + scrollBarBackgroundHeight * (offsetPercentage + visiblePercentage)) {
				mouseOffsetFromScrollBar = mouseOffset.y - scrollbarBorderTop - scrollBarBackgroundHeight * offsetPercentage;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
			else {
				offsetPercentage = static_cast<float>(mouseOffset.y - scrollbarBorderTop) / scrollBarBackgroundHeight - visiblePercentage / 2.0f;
				computeScrollBar();
				mouseOffsetFromScrollBar = mouseOffset.y - scrollbarBorderTop - scrollBarBackgroundHeight * offsetPercentage;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiVerticalScrollingListLayout::callbackMouseScrollOnElement(const Vec2d& offset)
	{
		currentVerticalOffset -= static_cast<int>(offset.y * static_cast<double>(scrollSpeed));
		if (currentVerticalHeight == 0.0f) offsetPercentage = 0.0f;
		else offsetPercentage = currentVerticalOffset / static_cast<float>(currentVerticalHeight);
		computeScrollBar();
	}
	//--------------------------------------------------------------------------------------------------
}