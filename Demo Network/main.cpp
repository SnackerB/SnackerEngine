#include "Core/Scene.h"
#include "Core/Engine.h"
#include "Core/Log.h"
#include "Network/Network.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"
#include "Gui/Layouts/HorizontalLayout.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiStyle.h"
#include "Gui/GuiEventHandles/GuiVariableHandle.h"
#include "Gui/GuiElements/GuiEditVariable.h"

class NetworkDemoScene : public SnackerEngine::Scene
{
	SnackerEngine::GuiManager guiManager;
	SnackerEngine::GuiEventHandle sendButtonHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> dstHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> SMPtypeHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> SMPoptionHandle;
	SnackerEngine::GuiEditTextBox editDataTextBox;

public:
	NetworkDemoScene()
		: guiManager{} 
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned char>(253, 152, 51)));
		// Setup GUI
		SnackerEngine::GuiStyle style = SnackerEngine::getDefaultStyle();
		SnackerEngine::HorizontalLayout horizontalLayout(true, false);
		guiManager.registerElement(horizontalLayout);
		{
			SnackerEngine::VerticalScrollingListLayout leftList(style);
			leftList.setBackgroundColor(SnackerEngine::Color4f(0.2f, 0.2f, 0.2f, 1.0f));
			horizontalLayout.registerChild(leftList, 2.0);
			{
			
			}
			guiManager.moveElement(std::move(leftList));

			SnackerEngine::VerticalScrollingListLayout rightList(style);
			horizontalLayout.registerChild(rightList, 1.0);
			{
				SnackerEngine::GuiButton buttonSend(sendButtonHandle, "Send Message", style);
				rightList.registerChild(buttonSend);
				guiManager.moveElement(std::move(buttonSend));
				
				SnackerEngine::GuiEditVariable<uint16_t> editDstVariable("destination: ", dstHandle, style);
				rightList.registerChild(editDstVariable);
				guiManager.moveElement(std::move(editDstVariable));
				
				SnackerEngine::GuiEditVariable<uint16_t> editTypeVariable("SMP type: ", SMPtypeHandle, style);
				rightList.registerChild(editTypeVariable);
				guiManager.moveElement(std::move(editTypeVariable));
				
				SnackerEngine::GuiEditVariable<uint16_t> editOptionVariable("SMP option: ", SMPoptionHandle, style);
				rightList.registerChild(editOptionVariable);
				guiManager.moveElement(std::move(editOptionVariable));

				SnackerEngine::GuiDynamicTextBox editDataLabel("data:", style);
				rightList.registerChild(editDataLabel);
				guiManager.moveElement(std::move(editDataLabel));

				editDataTextBox = SnackerEngine::GuiEditTextBox("", style);
				rightList.registerChild(editDataTextBox);
			}
			guiManager.moveElement(std::move(rightList));
		}
		guiManager.moveElement(std::move(horizontalLayout));
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
		if (sendButtonHandle.isActive()) {
			sendButtonHandle.reset();
			SnackerEngine::NetworkManager::SMP_Message message;
			message.smpHeader = SnackerEngine::SMP_Header(static_cast<SnackerEngine::MESSAGE_TYPE>(SMPtypeHandle.get()), SMPoptionHandle.get());
			std::string dataText = editDataTextBox.getText();
			message.data = std::vector<uint8_t>(dataText.size());
			std::memcpy(message.data.data(), dataText.data(), dataText.size());
			SnackerEngine::NetworkManager::sendMessage(message, dstHandle.get());
		}
		SnackerEngine::NetworkManager::update(dt);
		guiManager.update(dt);
	}

};

int main()
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Demo: Network")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
	}

	{
		NetworkDemoScene scene;
		SnackerEngine::Engine::setActiveScene(scene);
		SnackerEngine::NetworkManager::initialize();
		SnackerEngine::NetworkManager::connectToSERPServer();
		SnackerEngine::Engine::startup();
	}

	SnackerEngine::Engine::terminate();
}