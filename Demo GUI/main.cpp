#include "Core/Engine.h"
#include "Core/Log.h"
#include "Core/Keys.h"
#include "Gui/GuiStyle.h"
#include "Gui/Text/Unicode.h"
#include "Graphics/Renderer.h"

#include "Gui/GuiManager.h"
#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/Layouts/HorizontalLayout.h"
#include "Gui/Layouts/VerticalLayout.h"
#include "Gui/GuiElements/GuiWindow.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiElements/GuiTextVariable.h"
#include "Gui/GuiElements/GuiEditVariable.h"
#include "Gui/GuiElements/GuiSlider.h"

SnackerEngine::GuiVariableHandleInt2 intHandle2(42);

class HelloEventHandle : public SnackerEngine::GuiEventHandle
{
protected:
	void onEvent() override
	{
		SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN <<
			"event happened!" << SnackerEngine::LOGGER::ENDL;
		reset();
	}
};

class TextureDemo : public SnackerEngine::Scene
{
	SnackerEngine::GuiManager guiManager;
	SnackerEngine::GuiWindow parentWindow;
	SnackerEngine::GuiVariableHandle<int> intHandle;

	SnackerEngine::GuiVariableHandle<int> intSliderHandle;
	SnackerEngine::GuiVariableHandle<float> floatSliderHandle;
	SnackerEngine::GuiVariableHandle<double> doubleSliderHandle;

	unsigned int counter;
	HelloEventHandle helloEventHandle;
public:

	TextureDemo()
		: guiManager(3), counter(0)
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

		parentWindow = SnackerEngine::GuiWindow(style);
		guiManager.registerElement(parentWindow);
		parentWindow.setPosition({ 200, 200 });

		SnackerEngine::HorizontalLayout layout;
		parentWindow.registerChild(layout);
		{
			// main horizontal layout
			SnackerEngine::GuiPanel tempPanel;

			tempPanel = SnackerEngine::GuiPanel({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 1.0f, 1.0f, 1.0f });
			layout.registerChild(tempPanel, 1.0);
			guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
			
			SnackerEngine::VerticalLayout verticalLayout;
			layout.registerChild(verticalLayout, 1.0);
			{
				// vertical layout
				tempPanel = SnackerEngine::GuiPanel({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 1.0f, 0.0f, 0.0f });
				verticalLayout.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));

				SnackerEngine::GuiEditTextBox editBox("Edit me!", style);
				editBox.setSingleLine(false);
				editBox.setTextBoxMode(SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE);
				verticalLayout.registerChild(editBox, 1.0);
				editBox.setEventHandleTextWasEdited(helloEventHandle);
				guiManager.moveElement<SnackerEngine::GuiEditTextBox>(std::move(editBox));
				
				SnackerEngine::GuiDynamicTextBox textBox("Hello TexBox!", style);
				verticalLayout.registerChild(textBox, 1.0);
				guiManager.moveElement<SnackerEngine::GuiDynamicTextBox>(std::move(textBox));

				tempPanel = SnackerEngine::GuiPanel({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
				verticalLayout.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
			}
			guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(verticalLayout));

			tempPanel = SnackerEngine::GuiPanel({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.5f, 0.5f, 0.5f });
			layout.registerChild(tempPanel, 1.0);
			guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));

			SnackerEngine::VerticalLayout VerticalLayout;
			layout.registerChild(VerticalLayout, 1.0);
			{
				// second vertical layout
				tempPanel = SnackerEngine::GuiPanel({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.3f, 0.3f, 0.8f });
				VerticalLayout.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));

				SnackerEngine::ListLayout listLayout(style);
				VerticalLayout.registerChild(listLayout, 1.0);
				{
					// List layout
					SnackerEngine::GuiTextVariable<int> textVariable("test int: ", intHandle2, style);
					listLayout.registerChild(textVariable);
					guiManager.moveElement<SnackerEngine::GuiTextVariable<int>>(std::move(textVariable));

					SnackerEngine::GuiEditVariable<int> editVariable("edit int: ", style);
					editVariable.setVariableHandle(intHandle);
					listLayout.registerChild(editVariable);
					guiManager.moveElement<SnackerEngine::GuiEditVariable<int>>(std::move(editVariable));

					SnackerEngine::GuiButton button("test Button", style);
					listLayout.registerChild(button);
					guiManager.moveElement<SnackerEngine::GuiButton>(std::move(button));

					SnackerEngine::GuiSlider<float> floatSlider("float slider: ", -1.5f, 7.3f, style);
					floatSlider.setVariableHandle(floatSliderHandle);
					listLayout.registerChild(floatSlider);
					guiManager.moveElement<SnackerEngine::GuiSlider<float>>(std::move(floatSlider));

					SnackerEngine::GuiSlider<int> intSlider("int slider: ", 0, 3, style);
					intSlider.setVariableHandle(intSliderHandle);
					listLayout.registerChild(intSlider);
					guiManager.moveElement<SnackerEngine::GuiSlider<int>>(std::move(intSlider));

					SnackerEngine::GuiSlider<double> doubleSlider("double slider: ", 42.0, 99.999999999999, style);
					doubleSlider.setVariableHandle(doubleSliderHandle);
					listLayout.registerChild(doubleSlider);
					guiManager.moveElement<SnackerEngine::GuiSlider<double>>(std::move(doubleSlider));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 60 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 1.0f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 200, 100 }, SnackerEngine::GuiPanel::ResizeMode::DO_NOT_RESIZE, { 0.8f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 100, 300 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.6f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 20, 40 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel>(std::move(tempPanel));
				}
				guiManager.moveElement<SnackerEngine::ListLayout>(std::move(listLayout));
			}
			guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(VerticalLayout));

		}
		guiManager.moveElement<SnackerEngine::HorizontalLayout>(std::move(layout));
		guiManager.moveElement<SnackerEngine::GuiWindow>(std::move(parentWindow));

		//guiManager.registerAndMoveElement(std::move(parentPanel));
		
		/*
		SnackerEngine::GuiWindow parentWindow(style);
		parentWindow.setSize({ 100, 100 });
		auto listLayoutRef = guiManager.registerLayout(parentWindow, SnackerEngine::ListLayout(style));

		SnackerEngine::GuiTextVariable<int> intVariable(SnackerEngine::encodeUTF8(u8"test int: "), intHandle, style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiTextVariable<int>, SnackerEngine::ListLayout>(parentWindow, std::move(intVariable), listLayoutRef, {});
		SnackerEngine::GuiTextVariable<float> floatVariable(SnackerEngine::encodeUTF8(u8"test float: "), floatHandle, style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiTextVariable<float>, SnackerEngine::ListLayout>(parentWindow, std::move(floatVariable), listLayoutRef, {});
		SnackerEngine::GuiTextVariable<double> doubleVariable(SnackerEngine::encodeUTF8(u8"test double: "), doubleHandle, style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiTextVariable<double>, SnackerEngine::ListLayout>(parentWindow, std::move(doubleVariable), listLayoutRef, {});

		SnackerEngine::GuiButton increaseButton(increaseIntHandle, "increase variables (WARNING: this will increase all variables!)", style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiButton, SnackerEngine::ListLayout>(parentWindow, std::move(increaseButton), listLayoutRef, {});
		SnackerEngine::GuiButton decreaseButton(decreaseIntHandle, "decrease variables", style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiButton, SnackerEngine::ListLayout>(parentWindow, std::move(decreaseButton), listLayoutRef, {});
		SnackerEngine::GuiButton textButton(logTextFromTextBox, "log text from EditTextBox", style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiButton, SnackerEngine::ListLayout>(parentWindow, std::move(textButton), listLayoutRef, {});

		SnackerEngine::GuiInputUnsignedInt inputUnsignedInt("input unsigned int: ", unsignedIntHandle, style);
		guiManager.registerAndMoveElement<SnackerEngine::GuiInputUnsignedInt, SnackerEngine::ListLayout>(parentWindow, std::move(inputUnsignedInt), listLayoutRef, {});


		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 1", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 2", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 3", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 4", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 5", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox(":)", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 6", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 7", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 8", style)), listLayoutRef, {});

		guiManager.registerAndMoveElement<SnackerEngine::GuiDynamicTextBox, SnackerEngine::ListLayout>(parentWindow, std::move(SnackerEngine::GuiDynamicTextBox("text box 1", style)), listLayoutRef, {});
		guiManager.registerAndMoveElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
		*/

		/*
		SnackerEngine::GuiWindow parentWindow2(style);
		parentWindow2.setPosition({ 500, 500 });
		parentWindow2.setSize({ 700, 300 });
		auto horizontalLayoutRef = guiManager.registerLayout(parentWindow2, SnackerEngine::HorizontalLayout(true));

		SnackerEngine::GuiPanel childPanel1({ 0, 0 }, { 0, 0 }, { 1.0f, 0.0f, 0.0f });
		guiManager.registerAndMoveElement<SnackerEngine::GuiPanel, SnackerEngine::HorizontalLayout>(parentWindow2, std::move(childPanel1), horizontalLayoutRef, { 1.0f });
		textBox = SnackerEngine::GuiEditTextBox("Hello Textbox!", style);
		guiManager.registerElement<SnackerEngine::HorizontalLayout>(parentWindow2, textBox, horizontalLayoutRef, { 2.0f });
		SnackerEngine::GuiPanel childPanel2({ 0, 0 }, { 0, 0 }, { 0.0f, 0.0f, 1.0f });
		guiManager.registerAndMoveElement<SnackerEngine::GuiPanel, SnackerEngine::HorizontalLayout>(parentWindow2, std::move(childPanel2), horizontalLayoutRef, { 0.5f });

		guiManager.registerAndMoveElement<SnackerEngine::GuiWindow>(std::move(parentWindow2));
		*/

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
		if (intHandle.isActive()) 
		{
			SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << "Int changed to " << intHandle.get() << SnackerEngine::LOGGER::ENDL;
			intHandle.reset();
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