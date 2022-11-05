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
#include "Gui/Layouts/HorizontalLayout.h"
#include "Gui/GuiElements/GuiInputVariable.h"

#include "Gui/GuiManager2.h"
#include "Gui/GuiElements/GuiPanel2.h"
#include "Gui/Layouts/HorizontalLayout2.h"
#include "Gui/Layouts/VerticalLayout2.h"
#include "Gui/GuiElements/GuiWindow2.h"
#include "Gui/GuiElements/GuiTextBox2.h"
#include "Gui/GuiElements/GuiTextVariable2.h"
#include "Gui/GuiElements/GuiEditVariable.h"

SnackerEngine::GuiVariableHandleInt intHandle(2);
SnackerEngine::GuiVariableHandleFloat floatHandle(2.5f);
SnackerEngine::GuiVariableHandleDouble doubleHandle(10.3333);
SnackerEngine::GuiVariableHandle<SnackerEngine::Vec2f> vec2fHandle(SnackerEngine::Vec2f(1.0f, 2.0f));
SnackerEngine::GuiVariableHandle<unsigned int> unsignedIntHandle(42);

SnackerEngine::GuiVariableHandleInt2 intHandle2(42);

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

class HelloEventHandle : public SnackerEngine::GuiEventHandle2
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
	SnackerEngine::GuiManager2 guiManager;
	SnackerEngine::GuiWindow2 parentWindow;
	SnackerEngine::GuiEventHandle increaseIntHandle;
	SnackerEngine::GuiEventHandle decreaseIntHandle;
	SnackerEngine::GuiEventHandle logTextFromTextBox;
	SnackerEngine::GuiEditTextBox textBox;
	//SnackerEngine::GuiEditVariable<int> editVariable;
	unsigned int counter;
	HelloEventHandle helloEventHandle;
public:

	TextureDemo()
		: guiManager(3), increaseIntHandle{}, decreaseIntHandle{}, counter(0), textBox{}
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

		parentWindow = SnackerEngine::GuiWindow2(style);
		guiManager.registerElement(parentWindow);
		parentWindow.setPosition({ 200, 200 });

		SnackerEngine::HorizontalLayout2 layout;
		parentWindow.registerChild(layout);
		{
			// main horizontal layout
			SnackerEngine::GuiPanel2 tempPanel;

			tempPanel = SnackerEngine::GuiPanel2({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 1.0f, 1.0f, 1.0f });
			layout.registerChild(tempPanel, 1.0);
			guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
			
			SnackerEngine::VerticalLayout2 verticalLayout;
			layout.registerChild(verticalLayout, 1.0);
			{
				// vertical layout
				tempPanel = SnackerEngine::GuiPanel2({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 1.0f, 0.0f, 0.0f });
				verticalLayout.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));

				SnackerEngine::GuiEditTextBox2 editBox("Edit me!", style);
				editBox.setSingleLine(false);
				editBox.setTextBoxMode(SnackerEngine::GuiDynamicTextBox2::TextBoxMode::FORCE_SIZE);
				verticalLayout.registerChild(editBox, 1.0);
				editBox.setEventHandleTextWasEdited(helloEventHandle);
				guiManager.moveElement<SnackerEngine::GuiEditTextBox2>(std::move(editBox));
				
				SnackerEngine::GuiDynamicTextBox2 textBox("Hello TexBox!", style);
				verticalLayout.registerChild(textBox, 1.0);
				guiManager.moveElement<SnackerEngine::GuiDynamicTextBox2>(std::move(textBox));

				tempPanel = SnackerEngine::GuiPanel2({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
				verticalLayout.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
			}
			guiManager.moveElement<SnackerEngine::VerticalLayout2>(std::move(verticalLayout));

			tempPanel = SnackerEngine::GuiPanel2({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.5f, 0.5f, 0.5f });
			layout.registerChild(tempPanel, 1.0);
			guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));

			SnackerEngine::VerticalLayout2 verticalLayout2;
			layout.registerChild(verticalLayout2, 1.0);
			{
				// second vertical layout
				tempPanel = SnackerEngine::GuiPanel2({ 10, 10 }, { 10, 10 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.3f, 0.3f, 0.8f });
				verticalLayout2.registerChild(tempPanel, 1.0);
				guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));

				SnackerEngine::ListLayout2 listLayout(style);
				verticalLayout2.registerChild(listLayout, 1.0);
				SnackerEngine::infoLogger << "list layout has guiID " << listLayout.getGuiID() << SnackerEngine::LOGGER::ENDL;
				{
					// List layout
					SnackerEngine::GuiTextVariable2<int> textVariable("test int: ", intHandle2, style);
					listLayout.registerChild(textVariable);
					guiManager.moveElement<SnackerEngine::GuiTextVariable2<int>>(std::move(textVariable));
					SnackerEngine::GuiEditVariable<int>editVariable("edit int: ", style);
					listLayout.registerChild(editVariable);
					guiManager.moveElement<SnackerEngine::GuiEditVariable<int>>(std::move(editVariable));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 60 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 1.0f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 200, 100 }, SnackerEngine::GuiPanel2::ResizeMode::DO_NOT_RESIZE, { 0.8f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 100, 300 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.6f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 20, 40 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
					//tempPanel = SnackerEngine::GuiPanel2({ 0, 0 }, { 50, 100 }, SnackerEngine::GuiPanel2::ResizeMode::RESIZE_RANGE, { 0.4f, 0.0f, 0.0f });
					//listLayout.registerChild(tempPanel);
					//guiManager.moveElement<SnackerEngine::GuiPanel2>(std::move(tempPanel));
				}
				guiManager.moveElement<SnackerEngine::ListLayout2>(std::move(listLayout));
			}
			guiManager.moveElement<SnackerEngine::VerticalLayout2>(std::move(verticalLayout2));

		}
		guiManager.moveElement<SnackerEngine::HorizontalLayout2>(std::move(layout));
		guiManager.moveElement<SnackerEngine::GuiWindow2>(std::move(parentWindow));

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
		if (logTextFromTextBox.isActive())
		{
			SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << "text from textBox: \""
				<< textBox.getText() << SnackerEngine::LOGGER::ENDL;
			logTextFromTextBox.reset();
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