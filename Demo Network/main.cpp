#include <iostream>

#include "TCP\TCP.h"
#include "SnackerNet.h"
#include "HTTP\Header.h"
#include "HTTP\Message.h"
#include "HTTP\Request.h"
#include "HTTP\Response.h"

#include "Network\SerpManager.h"

void main()
{
	SnackerNet::initialize();
	SnackerEngine::SERPManager serpManager;
	serpManager.connectToSERPServer();
	while (true) {
		Sleep(100);
		serpManager.update(0.1);
	}
	// SnackerNet::HTTPRequest request;
	// request.requestMethod = SnackerNet::HTTPRequest::RequestMethod::HTTP_GET;
	// request.path = "/0042/serpID";
	// request.message.headers.push_back(SnackerNet::HTTPHeader{ "sender", "Benis" });
	// request.message.headers.push_back(SnackerNet::HTTPHeader{ "placeholde", "this header does nothing : )" });
	// request.message.body = "This is the message body, containing the actual message!";
	//SnackerNet::HTTPResponse response;
	//response.responseStatusCode = SnackerNet::ResponseStatusCode::IM_A_TEAPOT;
	//response.message.body = "this is the message body!";
	//response.message.headers.push_back(SnackerNet::HTTPHeader("destinationID", "0042"));
	//auto socket = SnackerNet::createSocketTCP();
	//if (socket.has_value()) {
	//	if (SnackerNet::connectTo(socket.value(), SnackerNet::getSERPServerAdressTCP())) {
	//		Sleep(1);
	//		std::vector<std::byte> data;
	//		std::string messageText = std::move(response.toString());
	//		for (const auto& c : messageText) data.push_back(static_cast<std::byte>(c));
	//		SnackerNet::Buffer buffer(std::move(data));
	//		SnackerNet::sendTo(socket.value(), buffer);
	//		// Wait for response
	//		SnackerNet::Buffer storageBuffer(4000);
	//		auto responseBuffer = SnackerNet::receiveFrom(socket.value(), storageBuffer);
	//		if (responseBuffer.has_value()) {
	//			std::cout << "response received: " << responseBuffer.value().to_string() << std::endl;
	//		}
	//	}
	//	else {
	//		std::cout << "connect() failed!" << std::endl;
	//	}
	//}
	//std::cout << "exiting!" << std::endl;
}