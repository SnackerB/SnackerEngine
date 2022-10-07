#include "Core/Engine.h"
#include "Core/Log.h"
#include "Gui/GuiManager.h"
#include "Gui/GuiElement.h"
#include "Gui/GuiElements/GuiPanel.h"
#include "Core/Keys.h"
#include "Gui/GuiEventHandles/GuiEventHandle.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/Layouts/GridLayout.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiElements/GuiTextVariable.h"
#include "Gui/Layouts/ListLayout.h"
#include "Gui/GuiStyle.h"
#include "Gui/Text/Unicode.h"
#include "Graphics/Renderer.h"

SnackerEngine::GuiVariableHandleInt intHandle(2);
SnackerEngine::GuiVariableHandleFloat floatHandle(2.5f);
SnackerEngine::GuiVariableHandleDouble doubleHandle(10.3333);
SnackerEngine::GuiVariableHandle<SnackerEngine::Vec2f> vec2fHandle(SnackerEngine::Vec2f(1.0f, 2.0f));

class IncreaseVariableEventHandle : public SnackerEngine::GuiEventHandle
{
protected:
	void onEvent() override
	{
		SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN <<
			"event happened: button clicked :)" << SnackerEngine::LOGGER::ENDL;
		intHandle = intHandle + 1;
		floatHandle = floatHandle * 2.0f;
		doubleHandle = doubleHandle * doubleHandle;
		reset();
	}
};

class TextureDemo : public SnackerEngine::Scene
{
	SnackerEngine::GuiManager guiManager;
	SnackerEngine::GuiEventHandle increaseIntHandle;
	SnackerEngine::GuiEventHandle decreaseIntHandle;
	unsigned int counter;

public:

	TextureDemo()
		: guiManager(3), increaseIntHandle{}, decreaseIntHandle{}, counter(0)
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned char>(253, 152, 51)));

		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();
		/*
		SnackerEngine::GuiWindow parentWindow({ 500, 500 }, { 500, 300 }, { 1.0f, 0.5f, 0.5f });
		auto gridLayoutRef = guiManager.registerLayout(parentWindow, SnackerEngine::GridLayout(3, 3));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 1.0f, 0.0f, 0.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 1, 1 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 0.0f, 1.0f, 0.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 1, 2 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 0.0f, 0.0f, 1.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 1, 3 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 0.0f, 1.0f, 0.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 2, 1 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 0.0f, 0.0f, 1.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 2, 2 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 1.0f, 0.0f, 0.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 2, 3 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 0.0f, 0.0f, 1.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 3, 1 }));
		guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::GridLayout>(parentWindow, SnackerEngine::GuiPanel({ 20, 20 }, { 50, 50 }, { 1.0f, 0.0f, 0.0f }), gridLayoutRef, SnackerEngine::GridLayoutOptions({ 3, 2 }));
		guiManager.registerElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
		
		SnackerEngine::GuiWindow parentWindow2({ 10, 10 }, { 500, 300 }, { 1.0f, 0.5f, 0.5f });
		
		auto gridLayoutRef2 = guiManager.registerLayout(parentWindow2, SnackerEngine::GridLayout(3, 3));
		guiManager.registerElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiDynamicTextBox({ 10, 10 }, { 1000, 100 }, "Hello Textbox! ggI|", SnackerEngine::Font("fonts/arial.ttf"), 15, { 1.0f, 1.0f, 1.0f, 1.0f }, {0.2f, 0.2, 0.2f, 1.0f}, SnackerEngine::StaticText::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText::Alignment::LEFT, SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 1, 1 }));														  
		guiManager.registerElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiDynamicTextBox({ 10, 10 }, { 1000, 100 }, "Hello Textbox! ggI|", SnackerEngine::Font("fonts/arial.ttf"), 15, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::StaticText::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText::Alignment::CENTER, SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 2, 2 }));														  
		guiManager.registerElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiDynamicTextBox({ 10, 10 }, { 1000, 100 }, "Hello Textbox! ggI|", SnackerEngine::Font("fonts/arial.ttf"), 15, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::StaticText::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText::Alignment::RIGHT, SnackerEngine::GuiDynamicTextBox::TextBoxMode::SHRINK_HEIGHT_TO_FIT),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 3, 3 }));
		//guiManager.registerElement<SnackerEngine::GuiStaticTextBox>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 260 }, { 1000, 100 }, u8"Hello Textbox!", SnackerEngine::Font("fonts/arial.ttf"), 24, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::TextParseMode::CHARACTERS, SnackerEngine::Alignment::CENTER));
		//guiManager.registerElement<SnackerEngine::GuiStaticTextBox>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 410 }, { 1000, 100 }, u8"Hello Textbox!", SnackerEngine::Font("fonts/arial.ttf"), 11, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::TextParseMode::CHARACTERS, SnackerEngine::Alignment::CENTER));
		guiManager.registerElement<SnackerEngine::GuiWindow>(std::move(parentWindow2));
		*/
		SnackerEngine::GuiWindow parentWindow(style);
		auto listLayoutRef = guiManager.registerLayout(parentWindow, SnackerEngine::ListLayout(20.0, 20.0));

		SnackerEngine::GuiTextVariable<int> intVariable(SnackerEngine::encodeUTF8(u8"test int: "), intHandle, style);
		guiManager.registerElement<SnackerEngine::GuiTextVariable<int>, SnackerEngine::ListLayout>(parentWindow, std::move(intVariable), listLayoutRef, {});
		SnackerEngine::GuiTextVariable<float> floatVariable(SnackerEngine::encodeUTF8(u8"test float: "), floatHandle, style);
		guiManager.registerElement<SnackerEngine::GuiTextVariable<float>, SnackerEngine::ListLayout>(parentWindow, std::move(floatVariable), listLayoutRef, {});
		SnackerEngine::GuiTextVariable<double> doubleVariable(SnackerEngine::encodeUTF8(u8"test double: "), doubleHandle, style);
		guiManager.registerElement<SnackerEngine::GuiTextVariable<double>, SnackerEngine::ListLayout>(parentWindow, std::move(doubleVariable), listLayoutRef, {});

		SnackerEngine::GuiButton increaseButton(increaseIntHandle, "increase variables (WARNING: this will increase all variables!)", style);
		guiManager.registerElement<SnackerEngine::GuiButton, SnackerEngine::ListLayout>(parentWindow, std::move(increaseButton), listLayoutRef, {});
		SnackerEngine::GuiButton decreaseButton(decreaseIntHandle, "decrease variables", style);
		guiManager.registerElement<SnackerEngine::GuiButton, SnackerEngine::ListLayout>(parentWindow, std::move(decreaseButton), listLayoutRef, {});

		guiManager.registerElement<SnackerEngine::GuiWindow>(std::move(parentWindow));

		//SnackerEngine::GuiWindow parentWindow({ 500, 500 }, { 500, 300 }, { 1.0f, 0.5f, 0.5f });
		//auto listLayoutRef = guiManager.registerLayout(parentWindow, SnackerEngine::ListLayout(20.0, 20.0));
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 50, 50 }, { 1.0f, 0.0f, 0.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 200, 240 }, { 0.0f, 1.0f, 0.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 500, 10 }, { 0.0f, 0.0f, 1.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 123, 54 }, { 0.0f, 1.0f, 0.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 50, 92 }, { 0.0f, 0.0f, 1.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 10, 150 }, { 1.0f, 0.0f, 0.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 52, 40 }, { 0.0f, 0.0f, 1.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiPanel, SnackerEngine::ListLayout>(parentWindow, SnackerEngine::GuiPanel({}, { 520, 88 }, { 1.0f, 0.0f, 0.0f }), listLayoutRef, {});
		//guiManager.registerElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
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
		if (increaseIntHandle.isActive())
		{
			intHandle = intHandle + 1;
			floatHandle = floatHandle * 2.0f;
			doubleHandle = doubleHandle * doubleHandle;
			increaseIntHandle.reset();
		}
		if (decreaseIntHandle.isActive())
		{
			intHandle = intHandle - 1;
			floatHandle = floatHandle / 2.0f;
			doubleHandle = sqrt(doubleHandle);
			decreaseIntHandle.reset();
		}
		guiManager.update(dt);
	}

};

int main(int argc, char** argv)
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Demo: GUI")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
	}

	{
		TextureDemo scene;
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();

}