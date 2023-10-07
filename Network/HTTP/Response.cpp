#include "Response.h"

#include <sstream>

namespace SnackerEngine
{

    std::optional<std::pair<HTTPResponse, std::size_t>> HTTPResponse::parseResponseLine(const ConstantBufferView& buffer)
    {
        HTTPResponse result;
        std::size_t pos = buffer.find_first_of(static_cast<std::byte>(' '));
        if (pos == std::string::npos) return {};
        if (!buffer.getBufferView(0, pos).compare("HTTP/1.0")) return {};
        // Parse ResponseStatusCode
        std::size_t lastPos = pos + 1;
        pos = buffer.find_first_of({ static_cast<std::byte>('\n'), static_cast<std::byte>(' ')}, lastPos);
        ConstantBufferView responseStatusCodeBuffer = pos == std::string::npos ? buffer.getBufferView(lastPos) : buffer.getBufferView(lastPos, pos - lastPos);
        std::optional<ResponseStatusCode> temp = parseResponseStatusCode(responseStatusCodeBuffer.to_string());
        if (!temp.has_value()) return {};
        result.responseStatusCode = temp.value();
        return std::make_pair<>(result, pos + 1);
    }

    std::string HTTPResponse::toString() const
    {
        std::stringstream result;
        result << "HTTP/1.0 ";
        result << responseStatusCode << "\n" << HTTPMessage::toString();
        return result.str();
    }

    Buffer HTTPResponse::toBuffer() const
    {
        return Buffer(toString());
    }

    void sendResponse(const SocketTCP& socket, const HTTPResponse& response)
    {
        Buffer buffer(std::move(response.toBuffer()));
        sendTo(socket, buffer);
    }

}