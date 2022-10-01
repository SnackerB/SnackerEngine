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

class CustomEventHandle : public SnackerEngine::GuiEventHandle
{
protected:
	void onEvent() override
	{
		SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN <<
			"event happened: button clicked :)" << SnackerEngine::LOGGER::ENDL;
		reset();
	}
};

class TextureDemo : public SnackerEngine::Scene
{

	SnackerEngine::GuiManager guiManager;
	CustomEventHandle eventHandle;
	unsigned int counter;

public:

	TextureDemo()
		: guiManager(3), counter(0)
	{
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
		
		std::u8string lorem = u8"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum.sanctus sea sed takimata ut vero voluptua.est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat. Consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua.At vero eos et accusam et justo duo dolores et ea rebum.Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Nam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum.Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo";
		SnackerEngine::GuiWindow parentWindow2({ 10, 10 }, { 500, 300 }, { 1.0f, 0.5f, 0.5f });
		auto gridLayoutRef2 = guiManager.registerLayout(parentWindow2, SnackerEngine::GridLayout(3, 3));
		guiManager.registerElement<SnackerEngine::GuiStaticTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 10 }, { 1000, 100 }, u8"Hello Textbox! ggI|", SnackerEngine::Font("fonts/arial.ttf"), 24, { 1.0f, 1.0f, 1.0f, 1.0f }, {0.2f, 0.2, 0.2f, 1.0f}, SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText2::Alignment::LEFT),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 1, 1 }));
		guiManager.registerElement<SnackerEngine::GuiStaticTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 10 }, { 1000, 100 }, lorem, SnackerEngine::Font("fonts/arial.ttf"), 2.5, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText2::Alignment::CENTER),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 2, 2 }));
		guiManager.registerElement<SnackerEngine::GuiStaticTextBox, SnackerEngine::GridLayout>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 10 }, { 1000, 100 }, u8"Hello Textbox! ggI|", SnackerEngine::Font("fonts/arial.ttf"), 24, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::StaticText2::ParseMode::WORD_BY_WORD, SnackerEngine::StaticText2::Alignment::RIGHT),
			gridLayoutRef, SnackerEngine::GridLayoutOptions({ 3, 3 }));
		//guiManager.registerElement<SnackerEngine::GuiStaticTextBox>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 260 }, { 1000, 100 }, u8"Hello Textbox!", SnackerEngine::Font("fonts/arial.ttf"), 24, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::TextParseMode::CHARACTERS, SnackerEngine::Alignment::CENTER));
		//guiManager.registerElement<SnackerEngine::GuiStaticTextBox>(parentWindow2, SnackerEngine::GuiStaticTextBox({ 10, 410 }, { 1000, 100 }, u8"Hello Textbox!", SnackerEngine::Font("fonts/arial.ttf"), 11, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.2f, 0.2, 0.2f, 1.0f }, SnackerEngine::TextParseMode::CHARACTERS, SnackerEngine::Alignment::CENTER));
		guiManager.registerElement<SnackerEngine::GuiWindow>(std::move(parentWindow2));
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