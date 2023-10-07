#include "Request.h"

#include <sstream>

namespace SnackerEngine
{

    std::optional<std::pair<HTTPRequest, std::size_t>> HTTPRequest::parseRequestLine(const ConstantBufferView& buffer)
    {
        HTTPRequest result;
        std::size_t pos = buffer.find_first_of(static_cast<std::byte>(' '));
        std::string debugString = buffer.to_string(); // DEBUG
        if (pos == std::string::npos) return {};
        if (!buffer.getBufferView(0, pos).compare("HTTP/1.0")) return {};
        // Parse RequestMethod
        std::size_t lastPos = pos + 1;
        pos = buffer.find_first_of(static_cast<std::byte>(' '), lastPos);
        if (pos == std::string::npos) return {};
        ConstantBufferView requestMethod = buffer.getBufferView(lastPos, pos - lastPos);
        if (requestMethod.compare("GET")) result.requestMethod = RequestMethod::HTTP_GET;
        else if (requestMethod.compare("POST")) result.requestMethod = RequestMethod::HTTP_POST;
        else if (requestMethod.compare("HEAD")) result.requestMethod = RequestMethod::HTTP_HEAD;
        else if (requestMethod.compare("PUT")) result.requestMethod = RequestMethod::HTTP_PUT;
        else if (requestMethod.compare("DELETE")) result.requestMethod = RequestMethod::HTTP_DELETE;
        else return {};
        // Parse path
        lastPos = pos + 1;
        pos = buffer.find_first_of(static_cast<std::byte>('\n'), lastPos);
        if (pos == std::string::npos) return {};
        result.path = buffer.getBufferView(lastPos, pos - lastPos).to_string();
        return std::make_pair<>(result, pos + 1);
    }

    std::string HTTPRequest::toString() const
    {
        std::stringstream result;
        result << "HTTP/1.0 ";
        switch (requestMethod)
        {
        case HTTPRequest::HTTP_GET: result << "GET"; break;
        case HTTPRequest::HTTP_POST: result << "POST"; break;
        case HTTPRequest::HTTP_HEAD: result << "HEAD"; break;
        case HTTPRequest::HTTP_PUT: result << "PUT"; break;
        case HTTPRequest::HTTP_DELETE: result << "DELETE"; break;
        default:
            break;
        }
        result << " " << path << "\n" << HTTPMessage::toString();
        return result.str();
    }

    Buffer HTTPRequest::toBuffer() const
    {
        return Buffer(toString());
    }

    std::vector<std::string> HTTPRequest::splitPath() const
    {
        if (path.empty()) return {};
        std::vector<std::string> result;
        std::size_t lastPos = path[0] == '/' ? 1 : 0;
        for (std::size_t i = lastPos; i < path.length(); ++i) {
            if (path[i] == '/') {
                result.push_back(path.substr(lastPos, i - lastPos));
                lastPos = i + 1;
            }
        }
        if (!path.ends_with('/')) {
            result.push_back(path.substr(lastPos));
        }
        return result;
    }

    void sendRequest(const SocketTCP& socket, const HTTPRequest& request)
    {
        Buffer buffer(std::move(request.toBuffer()));
        sendTo(socket, buffer);
    }

}
