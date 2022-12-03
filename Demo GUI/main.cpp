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
public:

	TextureDemo()
		: guiManager(3), counter(0)
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned char>(253, 152, 51)));

		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();

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
				editBox.setSingleLine(false);
				editBox.setTextBoxMode(SnackerEngine::GuiDynamicTextBox::TextBoxMode::FORCE_SIZE);
				verticalLayout.registerChild(editBox, 1.0);
				editBox.setEventHandleTextWasEdited(helloEventHandle);
				guiManager.moveElement<SnackerEngine::GuiEditTextBox>(std::move(editBox));

				SnackerEngine::GuiDynamicTextBox textBox("Hello TexBox!", style);
				verticalLayout.registerChild(textBox, 1.0);
				guiManager.moveElement<SnackerEngine::GuiDynamicTextBox>(std::move(textBox));
				
				SnackerEngine::GuiImage guiImage(style, style.defaultFont.getMsdfTexture());
				guiImage.setBackgroundColor(SnackerEngine::Color4f(0.0f, 1.0f, 0.0f, 1.0f));
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

				SnackerEngine::ListLayout listLayout(style);
				VerticalLayout.registerChild(listLayout, 1.0);
				{
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

				}
				guiManager.moveElement<SnackerEngine::ListLayout>(std::move(listLayout));
			}
			guiManager.moveElement<SnackerEngine::VerticalLayout>(std::move(VerticalLayout));

		}
		guiManager.moveElement<SnackerEngine::HorizontalLayout>(std::move(layout));

		guiManager.moveElement<SnackerEngine::GuiWindow>(std::move(parentWindow));
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