#include <iostream>

#include "SnackerNet.h"
#include "Network\SerpManager.h"

#include "Core/Scene.h"
#include "Gui/GuiManager.h"
#include "Core/Engine.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextureDataBuffer.h"
#include "AssetManager/TextureManager.h"

#include "Gui/Layouts/PositioningLayout.h"
#include "Gui/Layouts/HorizontalListLayout.h"
#include "Gui/Layouts/VerticalListLayout.h"
#include "Gui/Layouts/HorizontalWeightedLayout.h"
#include "Gui/Layouts/VerticalScrollingListLayout.h"
#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/GuiElements/GuiEditBox.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiImage.h"

class ChatScene : public SnackerEngine::Scene
{
private:
	SnackerEngine::GuiManager guiManager{};
	SnackerEngine::SERPManager serpManager{};
	enum class Mode {
		MAIN_MENU,
		JOIN_MENU,
		CHAT_HOST,
		CHAT_JOIN,
	};
	Mode mode = Mode::MAIN_MENU;
	/// Vector of event handles for gui interactions
	std::vector<SnackerEngine::GuiEventHandle> eventHandles{};
	/// Edit box for joining chat
	SnackerEngine::GuiEditBox serpIdEditBox{};
	/// Pointer to the popup window, if it is active
	std::unique_ptr<SnackerEngine::GuiElement> popupWindow = nullptr;
	/// Handle to the ok button of the pipup window
	SnackerEngine::GuiEventHandle popupOkHandle{};
	/// The list layout storing all messages
	SnackerEngine::GuiVerticalScrollingListLayout chatListLayout{};
	/// The edit box used for posting messages
	SnackerEngine::GuiEditBox postEditBox{};
	/// The event handle for the post button
	SnackerEngine::GuiEventHandle postHandle{};
	/// The event handle for the "post image" buttin
	SnackerEngine::GuiEventHandle postImageHandle{};
	/// The color of ougoing/incoming messages
	static const inline SnackerEngine::Color4f outgoingMessageColor = SnackerEngine::Color4f{0.3f, 0.8f, 0.3f, 1.0f};
	static const inline SnackerEngine::Color4f incomingMessageColor = SnackerEngine::Color4f{ 0.3f, 0.3f, 0.8f, 1.0f };
	/// The pending response object for all requests to the server
	std::unique_ptr<SnackerEngine::SERPManager::PendingResponse> pendingResponse = nullptr;
	/// The SerpID of the host (if in join mode)
	SnackerNet::SERPID hostID;
	/// The SerpIDs of all clients (if in host mode)
	std::vector<SnackerNet::SERPID> clientIDs;

	void initializeMainMenu() {
		mode = Mode::MAIN_MENU;
		// Build GUI
		guiManager.clear();
		eventHandles.resize(3);
		SnackerEngine::GuiVerticalListLayout verticalListLayout;
		verticalListLayout.setAlignmentVertical(SnackerEngine::AlignmentVertical::CENTER);
		guiManager.registerElement(verticalListLayout);
		{
			SnackerEngine::GuiButton joinButton("Join Chat");
			verticalListLayout.registerChild(joinButton);
			joinButton.setEventHandle(eventHandles[0]);
			guiManager.moveElement(std::move(joinButton));
			SnackerEngine::GuiButton hostButton("Host Chat");
			verticalListLayout.registerChild(hostButton);
			hostButton.setEventHandle(eventHandles[1]);
			guiManager.moveElement(std::move(hostButton));
			SnackerEngine::GuiButton exitButton("Exit");
			verticalListLayout.registerChild(exitButton);
			exitButton.setEventHandle(eventHandles[2]);
			guiManager.moveElement(std::move(exitButton));
		}
		guiManager.moveElement(std::move(verticalListLayout));
	}

	void initializeJoinMenu() {
		mode = Mode::JOIN_MENU;
		// Build GUI
		guiManager.clear();
		eventHandles.resize(2);
		SnackerEngine::GuiVerticalListLayout verticalListLayout;
		verticalListLayout.setAlignmentVertical(SnackerEngine::AlignmentVertical::CENTER);
		guiManager.registerElement(verticalListLayout);
		{
			serpIdEditBox = SnackerEngine::GuiEditBox{};
			serpIdEditBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
			serpIdEditBox.setText("0000");
			verticalListLayout.registerChild(serpIdEditBox);
			serpIdEditBox.setEventHandleEnterWasPressed(eventHandles[0]);
			SnackerEngine::GuiButton joinButton("Join");
			verticalListLayout.registerChild(joinButton);
			joinButton.setEventHandle(eventHandles[0]);
			guiManager.moveElement(std::move(joinButton));
			SnackerEngine::GuiButton exitButton("Back");
			verticalListLayout.registerChild(exitButton);
			exitButton.setEventHandle(eventHandles[1]);
			guiManager.moveElement(std::move(exitButton));
		}
		guiManager.moveElement(std::move(verticalListLayout));
	}

	/// Loads up a popup window with the given message and an ok button to close it again
	void buildPopup(const std::string& message) {
		popupWindow = std::make_unique<SnackerEngine::GuiPanel>(
			SnackerEngine::Vec2i(), SnackerEngine::Vec2i(),
			SnackerEngine::GuiElement::ResizeMode::SAME_AS_PARENT,
			SnackerEngine::Color4f{ 0.0f, 0.0f, 0.0f, 0.4f });
		guiManager.registerElement(*popupWindow);
		SnackerEngine::GuiPositioningLayout positioningLayout(SnackerEngine::GuiPositioningLayout::Mode::CENTER);
		popupWindow->registerChild(positioningLayout);
		{
			SnackerEngine::GuiVerticalListLayout verticalListLayout{};
			verticalListLayout.setAlignmentVertical(SnackerEngine::AlignmentVertical::CENTER);
			verticalListLayout.setShrinkHeightToChildren(true);
			verticalListLayout.setShrinkWidthToChildren(true);
			verticalListLayout.setResizeMode(SnackerEngine::GuiElement::ResizeMode::RESIZE_RANGE);
			verticalListLayout.setBackgroundColor(SnackerEngine::Color4f{ 0.2f, 0.2f, 0.2f, 1.0f });
			verticalListLayout.setDefaultAlignmentHorizontal(SnackerEngine::AlignmentHorizontal::CENTER);
			verticalListLayout.setHorizontalBorder(SnackerEngine::GuiElement::defaultBorderNormal);
			verticalListLayout.setVerticalBorder(SnackerEngine::GuiElement::defaultBorderNormal);
			positioningLayout.registerChild(verticalListLayout);
			{
				SnackerEngine::GuiTextBox messageBox(message);
				messageBox.setAlignment(SnackerEngine::StaticText::Alignment::CENTER);
				verticalListLayout.registerChild(messageBox);
				guiManager.moveElement(std::move(messageBox));
				SnackerEngine::GuiButton buttonOk("Ok");
				buttonOk.setSizeHintModePreferredSize(SnackerEngine::GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
				verticalListLayout.registerChild(buttonOk);
				buttonOk.setEventHandle(popupOkHandle);
				guiManager.moveElement(std::move(buttonOk));
			}
			guiManager.moveElement(std::move(verticalListLayout));
		}
		guiManager.moveElement(std::move(positioningLayout));
	}
	
	/// Constructs the general chatroom GUI, with the message displayed at the top of the sidebar
	void buildChatRoom(const std::string& sideBarTitle) {
		guiManager.clear();
		eventHandles.resize(1);
		SnackerEngine::GuiHorizontalWeightedLayout horizontalWeightedLayout{};
		horizontalWeightedLayout.setAllowMoveBorders(true);
		guiManager.registerElement(horizontalWeightedLayout);
		{
			SnackerEngine::GuiVerticalListLayout verticalListLayout{};
			horizontalWeightedLayout.registerChild(verticalListLayout, 0.8f);
			{
				chatListLayout = SnackerEngine::GuiVerticalScrollingListLayout{};
				verticalListLayout.registerChild(chatListLayout);
				SnackerEngine::GuiHorizontalListLayout horizontalListLayout{};
				horizontalListLayout.setShrinkHeightToChildren(true);
				verticalListLayout.registerChild(horizontalListLayout);
				{
					postEditBox = SnackerEngine::GuiEditBox{};
					horizontalListLayout.registerChild(postEditBox);
					postEditBox.setEventHandleEnterWasPressed(postHandle);
					SnackerEngine::GuiButton postButton("Post");
					postButton.setSizeHintModePreferredSize(SnackerEngine::GuiButton::SizeHintMode::SET_TO_TEXT_SIZE);
					horizontalListLayout.registerChild(postButton);
					postButton.setEventHandle(postHandle);
					guiManager.moveElement(std::move(postButton));
					SnackerEngine::GuiButton postImageButton("Post Image");
					postImageButton.setSizeHintModePreferredSize(SnackerEngine::GuiButton::SizeHintMode::SET_TO_TEXT_SIZE);
					horizontalListLayout.registerChild(postImageButton);
					postImageButton.setEventHandle(postImageHandle);
					guiManager.moveElement(std::move(postImageButton));
				}
				guiManager.moveElement(std::move(horizontalListLayout));
			}
			guiManager.moveElement(std::move(verticalListLayout));
			SnackerEngine::GuiVerticalScrollingListLayout sidebar{};
			sidebar.setHorizontalBorder(SnackerEngine::GuiElement::defaultBorderNormal);
			sidebar.setBackgroundColor(SnackerEngine::Color4f{ 0.3f, 0.3f, 0.3f, 1.0f });
			horizontalWeightedLayout.registerChild(sidebar, 0.2f);
			{
				SnackerEngine::GuiTextBox title(sideBarTitle);
				sidebar.registerChild(title);
				guiManager.moveElement(std::move(title));
				SnackerEngine::GuiButton backButton("Back to Main Menu");
				sidebar.registerChild(backButton);
				backButton.setEventHandle(eventHandles[0]);
				guiManager.moveElement(std::move(backButton));
			}
			guiManager.moveElement(std::move(sidebar));
		}
		guiManager.moveElement(std::move(horizontalWeightedLayout));
	}

	void initializeChatHostMenu() {
		mode = Mode::CHAT_HOST;
		// Subscribe to request path
		serpManager.registerPathForIncomingRequests("messages");
		serpManager.registerPathForIncomingRequests("clients");
		// Build GUI
		buildChatRoom("Hosting Room " + SnackerNet::to_string(serpManager.getSerpID()));
	}

	void initializeChatJoinMenu() {
		mode = Mode::CHAT_JOIN;
		// Subscribe to request paths
		serpManager.registerPathForIncomingRequests("messages");
		// Build GUI
		buildChatRoom("Joined Room " + SnackerNet::to_string(hostID));
	}

	void handleUpdateMainMenu() {
		if (eventHandles[0].isActive()) {
			eventHandles[0].reset();
			initializeJoinMenu();
		}
		else if (eventHandles[1].isActive()) {
			eventHandles[1].reset();
			if (!serpManager.isConnectedToSERPServer()) {
				buildPopup("Not connected to SERP server!");
			}
			else {
				initializeChatHostMenu();
			}
		}
		else if (eventHandles[2].isActive()) {
			eventHandles[2].reset();
			SnackerEngine::Engine::stopEngine();
		}
	}

	void handleUpdateJoinMenu() {
		if (eventHandles[0].isActive()) {
			eventHandles[0].reset();
			if (!serpManager.isConnectedToSERPServer()) {
				buildPopup("Not connected to SERP server!");
			}
			else {
				// Parse serpID
				auto serpID = SnackerNet::parseSERPID(serpIdEditBox.getText());
				if (!serpID.has_value() || serpID.value() == 0) {
					buildPopup("Invalid serpID!");
				}
				else {
					if (!pendingResponse) {
						pendingResponse = std::make_unique<SnackerEngine::SERPManager::PendingResponse>(
							serpManager.sendRequest(SnackerNet::SERPRequest(
								serpManager.getSerpID(), serpID.value(),
								SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_POST, "/clients", {}))));
						hostID = serpID.value();
					}
				}
			}
		}
		else if (eventHandles[1].isActive()) {
			initializeMainMenu();
			pendingResponse = nullptr;
			eventHandles[1].reset();
		}
		// Check for response or timeout
		if (pendingResponse && pendingResponse->getStatus() != SnackerEngine::SERPManager::PendingResponse::Status::PENDING) {
			if (pendingResponse->getStatus() == SnackerEngine::SERPManager::PendingResponse::Status::OBTAINED) {
				if (pendingResponse->getResponse().response.responseStatusCode == SnackerNet::ResponseStatusCode::OK &&
					pendingResponse->getResponse().getSource() == hostID) {
					initializeChatJoinMenu();
				}
				else {
					buildPopup("Could not find host!");
				}
			}
			else if (pendingResponse->getStatus() == SnackerEngine::SERPManager::PendingResponse::Status::TIMEOUT) {
				buildPopup("timeout when waiting for response!");
			}
			pendingResponse = nullptr;
		}
	}

	void postMessage(const std::string& message, bool incoming = true) {
		SnackerEngine::GuiTextBox chatMessage;
		chatMessage.setParseMode(SnackerEngine::StaticText::ParseMode::WORD_BY_WORD);
		chatMessage.setText(message.empty() ? "   " : message);
		chatMessage.setSizeHintModePreferredSize(SnackerEngine::GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE);
		chatMessage.setSizeHintModeMinSize(SnackerEngine::GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (incoming) {
			chatMessage.setAlignment(SnackerEngine::StaticText::Alignment::LEFT);
			chatMessage.setBackgroundColor(incomingMessageColor);
			chatListLayout.registerChild(chatMessage, SnackerEngine::AlignmentHorizontal::LEFT);
		}
		else {
			chatMessage.setAlignment(SnackerEngine::StaticText::Alignment::RIGHT);
			chatMessage.setBackgroundColor(outgoingMessageColor);
			chatListLayout.registerChild(chatMessage, SnackerEngine::AlignmentHorizontal::RIGHT);
		}
		guiManager.moveElement(std::move(chatMessage));
	}

	void postImage(const SnackerEngine::Texture& texture, bool incoming = true) {
		SnackerEngine::GuiImage image(texture);
		if (incoming) {
			chatListLayout.registerChild(image, SnackerEngine::AlignmentHorizontal::LEFT);
		}
		else {
			chatListLayout.registerChild(image, SnackerEngine::AlignmentHorizontal::RIGHT);
		}
		guiManager.moveElement(std::move(image));
	}

	void handleUpdateChatHost() {
		// Go through requests
		if (serpManager.areIncomingRequests("messages")) {
			auto& requests = serpManager.getIncomingRequests("messages");
			while (!requests.empty()) {
				SnackerNet::SERPRequest request = requests.front();
				if (request.request.requestMethod == request.request.HTTP_POST) {
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::OK, {}, "")));
					postMessage(request.request.body, true);
					// Relay message to all other clients
					for (auto it = clientIDs.begin(); it != clientIDs.end(); ++it) {
						if (*it != request.getSource()) {
							serpManager.sendRequest(SnackerNet::SERPRequest(serpManager.getSerpID(), *it,
								SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_POST, "/messages", {}, request.request.body)));
						}
					}
				}
				else {
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::BAD_REQUEST, {}, "")));
				}
				requests.pop();
			}
		}
		if (serpManager.areIncomingRequests("clients")) {
			auto& requests = serpManager.getIncomingRequests("clients");
			while (!requests.empty()) {
				SnackerNet::SERPRequest request = requests.front();
				if (request.request.requestMethod == request.request.HTTP_POST) {
					bool found = false;
					for (auto it = clientIDs.begin(); it != clientIDs.end(); ++it) {
						if (*it == request.getSource()) {
							found = true;
							break;
						}
					}
					if (!found) clientIDs.push_back(request.getSource());
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::OK, {}, "")));
				}
				else if (request.request.requestMethod == request.request.HTTP_DELETE) {
					for (auto it = clientIDs.begin(); it != clientIDs.end(); ++it) {
						if (*it == request.getSource()) {
							clientIDs.erase(it);
							break;
						}
					}
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::OK, {}, "")));
				}
				requests.pop();
			}
		}
		// Handle GUI updates
		if (postHandle.isActive()) {
			postHandle.reset();
			std::string message = postEditBox.getText();
			postMessage(message, false);
			// post message to all clients
			for (const auto& clientID : clientIDs) {
				serpManager.sendRequest(SnackerNet::SERPRequest(serpManager.getSerpID(), clientID,
					SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_POST, "/messages", {}, message)));
			}
		}
		else if (postImageHandle.isActive()) {
			postImageHandle.reset();
			std::string imagePath = postEditBox.getText();
			auto buffer = SnackerEngine::TextureDataBuffer::loadTextureDataBuffer2D(imagePath);
			if (buffer.has_value()) {
				postImage(SnackerEngine::Texture::CreateFromBuffer(buffer.value()), false);
				// Send texture to clients
				std::vector<std::byte> dataBuffer;
				buffer.value().serialize(dataBuffer);
				SnackerNet::SERPRequest request(serpManager.getSerpID(), 0,
					SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_POST, "/messages", { {"messageType", "image"} }, byteVectorToString(dataBuffer)));
				for (auto clientID : clientIDs) {
					request.setDestination(clientID);
					serpManager.sendRequest(request);
				}
			}
			else {
				buildPopup("Could not find image at \"" + imagePath + "\"");
			}
		}
		else if (eventHandles[0].isActive()) {
			eventHandles[0].reset();
			initializeMainMenu();
		}
	}

	void handleUpdateChatJoin() {
		// Go through requests
		if (serpManager.areIncomingRequests("messages")) {
			auto& requests = serpManager.getIncomingRequests("messages");
			while (!requests.empty()) {
				SnackerNet::SERPRequest request = requests.front();
				if (request.request.requestMethod == request.request.HTTP_POST) {
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::OK, {}, "")));
					// Look for the "messageType" header
					bool foundHeader = false;
					for (const auto& header : request.request.headers) {
						if (header.name == "messageType") {
							foundHeader = true;
							if (header.value == "image") {
								std::vector<std::byte> buffer = stringToByteVector(request.request.body);
								auto textureDataBuffer = SnackerEngine::TextureDataBuffer::Deserialize(buffer);
								if (textureDataBuffer.has_value()) postImage(SnackerEngine::Texture::CreateFromBuffer(textureDataBuffer.value()));
								else SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Received invalid texture buffer!" << SnackerEngine::LOGGER::ENDL;
							}
							else if (header.value == "text") {
								postMessage(request.request.body, true);
							}
							else {
								SnackerEngine::warningLogger << SnackerEngine::LOGGER::BEGIN << "Unknown messageType \"" << header.value << "\"" << SnackerEngine::LOGGER::ENDL;
							}
							break;
						}
					}
					if (!foundHeader) postMessage(request.request.body, true);
				}
				else {
					serpManager.sendResponse(SnackerNet::SERPResponse(serpManager.getSerpID(), request.getSource(),
						SnackerNet::HTTPResponse(SnackerNet::ResponseStatusCode::BAD_REQUEST, {}, "")));
				}
				requests.pop();
			}
		}
		// Handle GUI updates
		if (postHandle.isActive()) {
			postHandle.reset();
			std::string message = postEditBox.getText();
			postMessage(message, false);
			// post message to server
			serpManager.sendRequest(SnackerNet::SERPRequest(serpManager.getSerpID(), hostID,
				SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_POST, "/messages", {}, message)));
		}
		else if (eventHandles[0].isActive()) {
			eventHandles[0].reset();
			serpManager.sendRequest(SnackerNet::SERPRequest(serpManager.getSerpID(), hostID,
				SnackerNet::HTTPRequest(SnackerNet::HTTPRequest::HTTP_DELETE, "/clients", {}, "")));
			initializeMainMenu();
		}
	}

	std::string byteVectorToString(const std::vector<std::byte>& buffer) {
		std::stringstream ss;
		for (const auto& byte : buffer) {
			ss << static_cast<char>(byte);
		}
		return ss.str();
	}

	std::vector<std::byte> stringToByteVector(const std::string& string) {
		std::vector<std::byte> result;
		result.reserve(string.length());
		for (const auto& c : string) {
			result.push_back(static_cast<std::byte>(c));
		}
		return result;
	}

public:
	virtual void initialize() override
	{
		serpManager.connectToSERPServer();
		initializeMainMenu();
	}

	void draw() override
	{
		guiManager.draw();
	}

	virtual void callbackKeyboard(const int& key, const int& scancode, const int& action, const int& mods) override
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

	virtual void callbackWindowResize(const SnackerEngine::Vec2i& screenDims) override
	{
		guiManager.callbackWindowResize(screenDims);
	}

	virtual void callbackMouseScroll(const SnackerEngine::Vec2d& offset) override
	{
		guiManager.callbackMouseScroll(offset);
	}

	virtual void callbackCharacterInput(const unsigned int& codepoint) override
	{
		guiManager.callbackCharacterInput(codepoint);
	}

	virtual void update(const double& dt) override
	{
		switch (mode)
		{
		case ChatScene::Mode::MAIN_MENU: handleUpdateMainMenu(); break;
		case ChatScene::Mode::JOIN_MENU: handleUpdateJoinMenu(); break;
		case ChatScene::Mode::CHAT_HOST: handleUpdateChatHost(); break;
		case ChatScene::Mode::CHAT_JOIN: handleUpdateChatJoin(); break;
		default:
			break;
		}
		serpManager.update(dt);
		guiManager.update(dt);
		if (popupOkHandle.isActive()) {
			popupOkHandle.reset();
			popupWindow = nullptr;
		}
	}

};

int main()
{
	if (!SnackerEngine::Engine::initialize(1200, 700, "Chat demo")) {
		SnackerEngine::errorLogger << SnackerEngine::LOGGER::BEGIN << "startup failed!" << SnackerEngine::LOGGER::ENDL;
		return 0;
	}

	SnackerEngine::Renderer::setClearColor(SnackerEngine::Color3f(0.4f, 0.4f, 0.4f));

	SnackerNet::initialize();
	
	{
		ChatScene chatScene;
		chatScene.initialize();
		SnackerEngine::Engine::setActiveScene(chatScene);
		SnackerEngine::Engine::startup();
	}
	
	SnackerEngine::Engine::terminate();
}