#include "Core/Engine.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiStyle.h"

#include "Gui/GuiElements/GuiPanel.h";
#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/GuiElements/GuiTextVariable.h"

class ClippingDemo : public SnackerEngine::Scene
{

	SnackerEngine::GuiManager guiManager;

public:

	ClippingDemo()
	{
		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();

		SnackerEngine::GuiWindow window(style);
		window.setPosition({ 100, 100 });
		window.setSize({ 670, 500 });
		guiManager.registerElement(window);

		/*
		SnackerEngine::GuiPanel panel2({ 100, 100 }, { 400, 50 }, SnackerEngine::GuiElement::ResizeMode::DO_NOT_RESIZE, SnackerEngine::Color3f(0.0f, 1.0f, 0.0f));
		window.registerChild(panel2);
		guiManager.moveElement(std::move(panel2));
		*/

		SnackerEngine::GuiTextVariable<int> textVar("test int: ", style);
		textVar.setPosition({ 100, 200 });
		window.registerChild(textVar);
		guiManager.moveElement(std::move(textVar));
		
		guiManager.moveElement(std::move(window));
	}

	void draw() override
	{
		guiManager.draw();
	}

	void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override
	{
		guiManager.callbackKeyboard(key, scancode, action, mods);
	}

	virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override
	{
		guiManager.callbackMouseButton(button, action, mods);
	}

	virtual void callbackMouseMotion(const SnackerEngine::Vec2d& position) override
	{
		guiManager.callbackMouseMotion(position);
	}

	void callbackWindowResize(const SnackerEngine::Vec2i& screenDims) override
	{
		guiManager.callbackWindowResize(screenDims);
	}

	void callbackMouseScroll(const SnackerEngine::Vec2d& offset) override
	{
		guiManager.callbackMouseScroll(offset);
	}

	virtual void callbackCharacterInput(const unsigned int& codepoint) override
	{
		guiManager.callbackCharacterInput(codepoint);
	}

	virtual void update(const double& dt) override
	{
		guiManager.update(dt);
	}

};

int main(int argc, char** argv)
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Demo: Clipping GUI")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
	}

	{
		ClippingDemo scene;
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();

}