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
#include "Gui/GuiElements/GuiTextVariable.h"
#include "Network/SERP.h"
#include "Graphics/TextureDataBuffer.h"
#include "Gui/GuiElements/GuiImage.h"


#include <sstream>
#include <vector>

class NetworkDemoScene : public SnackerEngine::Scene
{
	SnackerEngine::GuiManager guiManager;
	SnackerEngine::GuiStyle style;
	SnackerEngine::GuiVariableHandle<uint16_t> clientIDHandle;
	SnackerEngine::GuiEventHandle sendButtonHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> dstHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> SMPtypeHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> SMPoptionHandle;
	SnackerEngine::GuiEditTextBox editDataTextBox;
	SnackerEngine::VerticalScrollingListLayout incomingMessagesList;
	SnackerEngine::GuiEventHandle addMulticastAddressHandle;
	SnackerEngine::GuiVariableHandle<uint16_t> newMulticastAddressHandle;
	SnackerEngine::GuiEventHandle clearMulticastAddressesHandle;
	SnackerEngine::VerticalListLayout multicastAdressesList;
	std::vector<uint16_t> multicastAdresses;
	SnackerEngine::GuiVariableHandle<std::string> texturePathHandle;
	SnackerEngine::GuiEventHandle sendTextureButtonHandle;
	SnackerEngine::GuiVariableHandleUnsignedInt bytesPerSecondHandle;

public:
	NetworkDemoScene()
		: guiManager{} 
	{
		SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f::fromColor256(SnackerEngine::Color3<unsigned char>(253, 152, 51)));
		// Setup GUI
		style = SnackerEngine::getDefaultStyle();
		SnackerEngine::HorizontalLayout horizontalLayout(true, false);
		guiManager.registerElement(horizontalLayout);
		{
			incomingMessagesList = SnackerEngine::VerticalScrollingListLayout(style);
			incomingMessagesList.setBackgroundColor(SnackerEngine::Color4f(0.2f, 0.2f, 0.2f, 1.0f));
			horizontalLayout.registerChild(incomingMessagesList, 2.0);
			{

			}

			SnackerEngine::VerticalScrollingListLayout rightList(style);
			rightList.setBackgroundColor(SnackerEngine::Color4f(0.15f, 0.15f, 0.15f, 1.0f));
			horizontalLayout.registerChild(rightList, 1.0);
			{
				SnackerEngine::GuiTextVariable<uint16_t> clientIDDisplay("clientID: ", clientIDHandle, style);
				rightList.registerChild(clientIDDisplay);
				guiManager.moveElement(std::move(clientIDDisplay));

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

				SnackerEngine::GuiButton buttonAddMulticastAddress(addMulticastAddressHandle, "Add Multicast address", style);
				rightList.registerChild(buttonAddMulticastAddress);
				guiManager.moveElement(std::move(buttonAddMulticastAddress));

				SnackerEngine::GuiEditVariable<uint16_t> editNewMulticastAddressVariable("new multicast address: ", newMulticastAddressHandle, style);
				rightList.registerChild(editNewMulticastAddressVariable);
				guiManager.moveElement(std::move(editNewMulticastAddressVariable));

				SnackerEngine::GuiButton buttonClearMulticastAddresses(clearMulticastAddressesHandle, "Clear multicast addresses", style);
				rightList.registerChild(buttonClearMulticastAddresses);
				guiManager.moveElement(std::move(buttonClearMulticastAddresses));

				SnackerEngine::GuiDynamicTextBox multicastAdressesLabel("Multicast adresses:", style);
				rightList.registerChild(multicastAdressesLabel);
				guiManager.moveElement(std::move(multicastAdressesLabel));

				multicastAdressesList = SnackerEngine::VerticalListLayout(style.verticalScrollingListLayoutLeftBorder, true, true, true);
				multicastAdressesList.setBackgroundColor(SnackerEngine::Color4f(0.1f, 0.1f, 0.1f, 1.0f));
				rightList.registerChild(multicastAdressesList);

				SnackerEngine::GuiEditVariable<std::string> editTexturePathVariable("texture path: ", texturePathHandle, style);
				texturePathHandle.set("textures/dab.jpg");
				rightList.registerChild(editTexturePathVariable);
				guiManager.moveElement(std::move(editTexturePathVariable));

				SnackerEngine::GuiButton sendTextureButton(sendTextureButtonHandle, "Send texture", style);
				rightList.registerChild(sendTextureButton);
				guiManager.moveElement(std::move(sendTextureButton));

				SnackerEngine::GuiEditVariable<unsigned int> bytesPerSecondEditVariable("bytes per second: ", bytesPerSecondHandle, style);
				bytesPerSecondHandle.set(500'000);
				rightList.registerChild(bytesPerSecondEditVariable);
				guiManager.moveElement(std::move(bytesPerSecondEditVariable));
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
		if (SnackerEngine::NetworkManager::isConnectedToSERPServer()) {
			clientIDHandle.set(SnackerEngine::NetworkManager::getClientID());
		}
		if (addMulticastAddressHandle.isActive()) {
			addMulticastAddressHandle.reset();
			uint16_t newMulticastAddress = newMulticastAddressHandle.get();
			bool foundaddress = false;
			for (const auto& multicastAddress : multicastAdresses) {
				if (multicastAddress == newMulticastAddress) {
					foundaddress = true;
					break;
				}
			}
			if (!foundaddress) {
				multicastAdresses.push_back(newMulticastAddress);
				SnackerEngine::GuiDynamicTextBox newMulticastAddressTextBox(std::to_string(newMulticastAddress), style);
				newMulticastAddressTextBox.setSizeHintModeMinSize(SnackerEngine::GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
				multicastAdressesList.registerChild(newMulticastAddressTextBox);
				guiManager.moveElement(std::move(newMulticastAddressTextBox));
			}
		}
		if (clearMulticastAddressesHandle.isActive()) {
			clearMulticastAddressesHandle.reset();
			multicastAdresses.clear();
			multicastAdressesList.clear();
		}
		if (sendButtonHandle.isActive()) {
			sendButtonHandle.reset();
			SnackerEngine::NetworkManager::SMP_Message message;
			message.smpHeader = SnackerEngine::SMP_Header(static_cast<SnackerEngine::MESSAGE_TYPE>(SMPtypeHandle.get()), SMPoptionHandle.get());
			std::string dataText = editDataTextBox.getText();
			message.data = std::vector<std::byte>(dataText.size());
			std::memcpy(message.data.data(), dataText.data(), dataText.size());
			if (dstHandle.get() == SERP_DST_MULTICAST) {
				SnackerEngine::NetworkManager::sendMessageMulticast(message, multicastAdresses);
			}
			else {
				SnackerEngine::NetworkManager::sendMessage(message, dstHandle.get());
			}
		}
		if (sendTextureButtonHandle.isActive()) {
			sendTextureButtonHandle.reset();
			auto textureDataBuffer = SnackerEngine::TextureDataBuffer::loadTextureDataBuffer2D(texturePathHandle.get());
			if (textureDataBuffer.has_value()) {
				SnackerEngine::NetworkManager::SMP_Message message;
				message.smpHeader = SnackerEngine::SMP_Header(static_cast<SnackerEngine::MESSAGE_TYPE>(100), 0);
				textureDataBuffer.value().serialize(message.data);
				if (dstHandle.get() == SERP_DST_MULTICAST) {
					SnackerEngine::NetworkManager::sendMessageMulticast(message, multicastAdresses);
				}
				else {
					SnackerEngine::NetworkManager::sendMessage(message, dstHandle.get());
				}
			}
		}
		SnackerEngine::NetworkManager::update(dt);
		// Look at incoming messages
		std::vector<SnackerEngine::NetworkManager::SMP_Message> incomingMessages;
		incomingMessages = std::move(SnackerEngine::NetworkManager::getIncomingMessages());
		for (SnackerEngine::NetworkManager::SMP_Message& message : incomingMessages) {
			if (message.smpHeader.type == 100) {
				std::optional<SnackerEngine::TextureDataBuffer> textureDataBuffer = std::move(SnackerEngine::TextureDataBuffer::Deserialize(message.data));
				if (textureDataBuffer.has_value()) {
					SnackerEngine::Texture texture = SnackerEngine::Texture::CreateFromBuffer(textureDataBuffer.value());
					SnackerEngine::GuiImage image(style, texture);
					image.setGuiImageMode(SnackerEngine::GuiImage::GuiImageMode::RESIZE_TO_IMAGE_SIZE);
					incomingMessagesList.registerChild(image);
					guiManager.moveElement(std::move(image));
				}
				else {
					SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Received faulty textureDataBuffer!" << SnackerEngine::LOGGER::ENDL;
				}
			}
			else {
				std::stringstream ss;
				ss << "[" << message.src << "]: " << message.smpHeader.type << ", " << message.smpHeader.options;
				if (!message.data.empty()) {
					ss << ": ";
					for (const auto& c : message.data) {
						ss << static_cast<uint8_t>(c);
					}
				}
				SnackerEngine::GuiDynamicTextBox messageText(ss.str(), style);
				incomingMessagesList.registerChild(messageText);
				guiManager.moveElement(std::move(messageText));
			}
		}
		if (bytesPerSecondHandle.isActive()) 
		{
			bytesPerSecondHandle.reset();
			SnackerEngine::NetworkManager::setBytesPerSecondsSend(bytesPerSecondHandle.get());
		}
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