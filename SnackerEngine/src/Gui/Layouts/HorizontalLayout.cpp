#include "Gui/Layouts/HorizontalLayout.h"
#include "Core/Log.h"
#include "Gui/GuiManager.h"
#include "Core/Keys.h"

namespace SnackerEngine
{

	void HorizontalLayout::addChild(const GuiID& guiID, const LayoutOptions& options)
	{
		GuiLayout::addChild(guiID, {});
		layoutOptions.push_back(options);
		totalWeight += options.weight;
		enforceLayout();
	}

	std::size_t HorizontalLayout::removeChild(GuiElement& guiElement)
	{
		std::size_t index = GuiLayout::removeChild(guiElement);
		layoutOptions.erase(layoutOptions.begin() + index);
		return index;
	}

	void HorizontalLayout::enforceLayout()
	{
		if (!guiManager) return;
		Vec2i parentSize = getSize(parentID);
		unsigned int remainder = parentSize.x;
		for (unsigned int i = 0; i < children.size(); ++i) {
			Vec2i position = Vec2i(parentSize.x - remainder, 0);
			Vec2i size = (i == children.size() - 1) ? Vec2i(remainder, parentSize.y)
				: Vec2i(static_cast<int>(static_cast<float>(parentSize.x) * layoutOptions[i].weight / totalWeight), parentSize.y);
			remainder -= size.x;
			setPosition(children[i], position);
			notifyPositionChange(children[i]);
			setSize(children[i], size);
			notifySizeChange(children[i]);
			enforceLayouts(children[i]);
		}
	}

	std::optional<std::pair<unsigned int, int>> HorizontalLayout::getCollidingBorderAndOffset(const Vec2i& position)
	{
		int parentWidth = getSize(parentID).x;
		// Start from the left
		int currentX = 0;
		for (unsigned int i = 0; i < children.size() - 1; ++i) {
			currentX += static_cast<int>(static_cast<float>(parentWidth) * layoutOptions[i].weight / totalWeight);
			if (currentX - resizeAreaWidth <= position.x && position.x <= currentX + resizeAreaWidth) {
				return std::make_pair(i, position.x - currentX);
			}
		}
		return {};
	}

	HorizontalLayout::IsCollidingResult HorizontalLayout::isColliding(const Vec2i& position)
	{
		return getCollidingBorderAndOffset(position) ? IsCollidingResult::STRONG_COLLIDING : IsCollidingResult::WEAK_COLLIDING;
	}

	void HorizontalLayout::onRegister()
	{
		GuiLayout::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}

	void HorizontalLayout::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
			signOffEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
		}
	}

	void HorizontalLayout::callbackMouseMotion(const Vec2d& position)
	{
		int borderPos = getMouseOffset(parentID).x - mouseOffset;
		if (borderPos < getPosition(children[resizeBorder]).x) {
			borderPos = getPosition(children[resizeBorder]).x;
		}
		else if (resizeBorder + 2 < children.size() && borderPos > getPosition(children[resizeBorder + 2]).x) {
			borderPos = getPosition(children[resizeBorder + 2]).x;
		}
		// Compute new weights. Start with the weight of the left element
		layoutOptions[resizeBorder].weight = totalWeight * static_cast<float>(borderPos - getPosition(children[resizeBorder]).x) / static_cast<float>(getSize(parentID).x);
		if (resizeBorder + 2 < children.size()) {
			layoutOptions[resizeBorder + 1].weight = totalWeight * static_cast<float>(getPosition(children[resizeBorder + 2]).x - borderPos) / static_cast<float>(getSize(parentID).x);
		}
		else {
			// Border is the most right possible
			layoutOptions[resizeBorder + 1].weight = totalWeight * static_cast<float>(getSize(parentID).x - borderPos) / static_cast<float>(getSize(parentID).x);
		}
		enforceLayout();
	}

	void HorizontalLayout::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			Vec2i offset = getMouseOffset(parentID);
			auto result = getCollidingBorderAndOffset(offset);
			if (result) {
				resizeBorder = result->first;
				mouseOffset = result->second;
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_MOTION);
				signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON);
			}
		}
	}

	HorizontalLayout::HorizontalLayout(const bool& forceHeight)
		: GuiLayout(), totalWeight(0.0f), forceHeight(forceHeight), resizeAreaWidth(20), mouseOffset(0), resizeBorder(0), layoutOptions{} {}

}