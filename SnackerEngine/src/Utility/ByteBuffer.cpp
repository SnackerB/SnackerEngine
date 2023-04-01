#include "Utility/ByteBuffer.h"

namespace SnackerEngine
{

	void copyToBufferString(std::vector<std::byte>& buffer, const std::string& data)
	{
		buffer.resize(data.size());
		std::memcpy(buffer.data(), data.data(), data.size());
	}

	void appendToBufferString(std::vector<std::byte>& buffer, const std::string& data)
	{
		std::size_t size = buffer.size();
		buffer.resize(size + data.size());
		std::memcpy(buffer.data() + size, data.data(), data.size());
	}

}
