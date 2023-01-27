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
#include "Gui/GuiElements/GuiImage.h"
#include "Gui/GuiStyle.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Utility/Alignment.h"

class HelloEventHandle : public SnackerEngine::GuiEventHandle
{
protected:
	void onEvent() override
	{
		SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN <<
			"text was edited!" << SnackerEngine::LOGGER::ENDL;
		reset();
	}
};

class TextureDemo : public SnackerEngine::Scene
{
	SnackerEngine::GuiManager guiManager;
	SnackerEngine::GuiWindow parentWindow;
	SnackerEngine::GuiVariableHandleInt intHandle;

	SnackerEngine::GuiVariableHandleFloat floatSliderHandle;
	SnackerEngine::GuiVariableHandleDouble doubleSliderHandle;

	unsigned int counter;
	HelloEventHandle helloEventHandle;
	SnackerEngine::GuiEventHandle eventHandle;
	SnackerEngine::GuiEditTextBox editBoxUTF8;
public:

	TextureDemo()
		: guiManager(3), counter(0)
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned char>(253, 152, 51)));

		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();

		enum class DebugVariant
		{
			TEXT_DEBUG,
			WINDOW_DEBUG,
			VERTICAL_LIST_LAYOUT_DEBUG,
			UTF8_DEBUG,
		};
		DebugVariant debugVariant = DebugVariant::VERTICAL_LIST_LAYOUT_DEBUG;

		switch (debugVariant)
		{
		case DebugVariant::WINDOW_DEBUG:
		{
			parentWindow = SnackerEngine::GuiWindow(style);
			guiManager.registerElement(parentWindow);
			parentWindow.setPosition({ 10, 10 });
			parentWindow.setSize({ 1180, 680 });
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
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_DOWN);
					verticalLayout.registerChild(editBox, 1.0);
					editBox.setEventHandleTextWasEdited(helloEventHandle);
					guiManager.moveElement<SnackerEngine::GuiEditTextBox>(std::move(editBox));

					SnackerEngine::GuiDynamicTextBox textBox("Hello TexBox!", style);
					verticalLayout.registerChild(textBox, 1.0);
					guiManager.moveElement<SnackerEngine::GuiDynamicTextBox>(std::move(textBox));

					SnackerEngine::GuiImage guiImage(style, style.defaultFont.getMsdfTexture());
					verticalLayout.registerChild(guiImage, 1.0);
					guiManager.moveElement<SnackerEngine::GuiImage>(std::move(guiImage));
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

					SnackerEngine::VerticalScrollingListLayout listLayout(style);
					VerticalLayout.registerChild(listLayout, 1.0);
					{
						SnackerEngine::GuiPanel panel;
						panel.setPreferredSize({ 200, 100 });
						panel.setMinSize({ 100, 50 });
						panel.setMaxSize({ 200, 150 });
						listLayout.registerChild(panel);
						guiManager.moveElement(std::move(panel));

						panel = SnackerEngine::GuiPanel();
						panel.setPreferredSize({ 200, 100 });
						panel.setMinSize({ 100, 50 });
						panel.setMaxSize({ 200, 150 });
						listLayout.registerChild(panel);
						guiManager.moveElement(std::move(panel));
						/*
						// List layout
						SnackerEngine::GuiTextVariable<int> textVariable("test int: ", intHandle, style);
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
						intSlider.setVariableHandle(intHandle);
						listLayout.registerChild(intSlider);
						guiManager.moveElement<SnackerEngine::GuiSlider<int>>(std::move(intSlider));

						SnackerEngine::GuiSlider<double> doubleSlider1("double slider: ", 42.0, 99.999999999999, style);
						doubleSlider1.setVariableHandle(doubleSliderHandle);
						listLayout.registerChild(doubleSlider1);
						guiManager.moveElement<SnackerEngine::GuiSlider<double>>(std::move(doubleSlider1));

						SnackerEngine::GuiSlider<double> doubleSlider2("double slider: ", 20, 70.0, style);
						doubleSlider2.setVariableHandle(doubleSliderHandle);
						listLayout.registerChild(doubleSlider2);
						guiManager.moveElement<SnackerEngine::GuiSlider<double>>(std::move(doubleSlider2));
						*/
					}
					guiManager.moveElement(std::move(listLayout));
				}
				guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(VerticalLayout));

			}
			guiManager.moveElement<SnackerEngine::HorizontalLayout>(std::move(layout));
			guiManager.moveElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
			break;
		}
		case DebugVariant::TEXT_DEBUG:
		{
			SnackerEngine::GuiWindow parentWindow2 = SnackerEngine::GuiWindow(style);
			guiManager.registerElement(parentWindow2);
			parentWindow2.setPosition({ 10, 10 });
			parentWindow2.setSize({ 1180, 680 });
			SnackerEngine::HorizontalLayout horizontalLayout;
			parentWindow2.registerChild(horizontalLayout);
			{
				// LEFT aligned text
				SnackerEngine::VerticalLayout verticalLayout;
				horizontalLayout.registerChild(verticalLayout, 1.0);
				{
					std::vector<SnackerEngine::Color3f> colors = {
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(220, 28, 19)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(234, 76, 70)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(240, 116, 112)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(241, 149, 155)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(246, 189, 192)),
					};
					// Text box: FORCE_SIZE_SCALE_TEXT_DOWN
					SnackerEngine::GuiEditTextBox editBox("SCALE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[0]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT_UP
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[1]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[2]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE_DOWN
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[3]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[4]);
					guiManager.moveElement(std::move(editBox));
				}
				guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(verticalLayout));
				// CENTER aligned text
				verticalLayout = SnackerEngine::VerticalLayout();
				horizontalLayout.registerChild(verticalLayout, 1.0);
				{
					std::vector<SnackerEngine::Color3f> colors = {
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(46, 182, 44)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(87, 200, 77)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(131, 212, 117)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(171, 224, 152)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(197, 232, 183)),
					};
					// Text box: FORCE_SIZE_SCALE_TEXT_DOWN
					SnackerEngine::GuiEditTextBox editBox("SCALE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[0]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT_UP
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[1]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[2]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE_DOWN
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[3]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[4]);
					guiManager.moveElement(std::move(editBox));
				}
				guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(verticalLayout));
				// RIGHT aligned text
				verticalLayout = SnackerEngine::VerticalLayout();
				horizontalLayout.registerChild(verticalLayout, 1.0);
				{
					std::vector<SnackerEngine::Color3f> colors = {
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(0, 0, 255)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(31, 31, 255)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(73, 73, 255)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(120, 121, 255)),
						SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned>(163, 163, 255)),
					};
					// Text box: FORCE_SIZE_SCALE_TEXT_DOWN
					SnackerEngine::GuiEditTextBox editBox("SCALE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[0]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT_UP
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[1]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_SCALE_TEXT
					editBox = SnackerEngine::GuiEditTextBox("SCALE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::SCALE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[2]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE_DOWN
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[3]);
					guiManager.moveElement(std::move(editBox));
					// Text box: FORCE_SIZE_REOMPUTE_SCALE
					editBox = SnackerEngine::GuiEditTextBox("RECOMPUTE_UP_DOWN", style);
					editBox.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
					editBox.setTextParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
					editBox.setTextScaleMode(SnackerEngine::GuiDynamicTextBox::TextScaleMode::RECOMPUTE_UP_DOWN);
					verticalLayout.registerChild(editBox);
					editBox.setBackgroundColor(colors[4]);
					guiManager.moveElement(std::move(editBox));
				}
				guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(verticalLayout));
			}
			guiManager.moveElement<SnackerEngine::HorizontalLayout>(std::move(horizontalLayout));
			guiManager.moveElement<SnackerEngine::GuiWindow>(std::move(parentWindow2));
			break;
		}
		case DebugVariant::VERTICAL_LIST_LAYOUT_DEBUG:
		{
			parentWindow = SnackerEngine::GuiWindow(style);
			guiManager.registerElement(parentWindow);
			parentWindow.setPosition({ 10, 10 });
			parentWindow.setSize({ 1180, 680 });
			SnackerEngine::VerticalListLayout verticalListLayout(10, false, SnackerEngine::AlignmentHorizontal::RIGHT, SnackerEngine::AlignmentVertical::CENTER);
			parentWindow.registerChild(verticalListLayout);
			{
				SnackerEngine::GuiPanel panelTemplate;
				panelTemplate.setBackgroundColor({ 1.0f, 0.0f, 0.0f });

				SnackerEngine::GuiPanel panel = panelTemplate;
				panel.setMinSize({ 100, 100 });
				panel.setMaxSize({ 200, 200 });
				panel.setPreferredSize({ 150, 150 });
				verticalListLayout.registerChild(panel);
				guiManager.moveElement(std::move(panel));

				panel = panelTemplate;
				panel.setMinSize({ 100, 100 });
				panel.setMaxSize({ 200, 500 });
				panel.setPreferredSize({ -1, 150 });
				verticalListLayout.registerChild(panel);
				guiManager.moveElement(std::move(panel));

				panel = panelTemplate;
				panel.setMinSize({ 100, 100 });
				panel.setMaxSize({ 300, -1 });
				panel.setPreferredSize({ 200, -1 });
				panel.setBackgroundColor({ 0.0f, 1.0f, 0.0f });
				verticalListLayout.registerChild(panel);
				guiManager.moveElement(std::move(panel));

				panel = panelTemplate;
				panel.setMinSize({ 100, 100 });
				panel.setMaxSize({ -1, 500 });
				panel.setPreferredSize({ -1, 150 });
				verticalListLayout.registerChild(panel);
				guiManager.moveElement(std::move(panel));
			}
			guiManager.moveElement(std::move(verticalListLayout));
			guiManager.moveElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
			break;
		}
		case DebugVariant::UTF8_DEBUG:
		{
			editBoxUTF8 = SnackerEngine::GuiEditTextBox(SnackerEngine::encodeUTF8(u8"Hällo Wörld"), style);
			editBoxUTF8.setEventHandleTextWasEdited(eventHandle);
			guiManager.registerElement(editBoxUTF8);
			break;
		}
		default:
			break;
		}
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
		if (eventHandle.isActive()) 
		{
			eventHandle.reset();
			std::string text = editBoxUTF8.getText();
			SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << "text was: " << text << SnackerEngine::LOGGER::ENDL;
			editBoxUTF8.setText(text);
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