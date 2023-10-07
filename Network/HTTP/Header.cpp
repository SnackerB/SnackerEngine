#include "HTTP/Header.h"

namespace SnackerEngine
{

	std::optional<HTTPHeader> HTTPHeader::parse(const ConstantBufferView& buffer)
	{
		std::size_t colonPos = buffer.find_first_of(static_cast<std::byte>(':'));
		if (colonPos == std::string::npos) return {};
		HTTPHeader result{};
		if (colonPos > 0) result.name = buffer.getBufferView(0, colonPos).to_string();
		std::size_t valueStartPos = buffer.find_first_not_of(static_cast<std::byte>(' '), colonPos + 1);
		if (valueStartPos != std::string::npos) result.value = buffer.getBufferView(valueStartPos).to_string();
		return result;
	}

	std::string HTTPHeader::toString() const
	{
		return name + ":" + value;
	}

	Buffer HTTPHeader::toBuffer() const
	{
		return Buffer(toString());
	}

}
