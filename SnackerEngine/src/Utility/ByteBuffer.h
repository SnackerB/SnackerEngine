#pragma once

#include <vector>
#include <bit>
#include <string>

namespace SnackerEngine
{

	/// Resizes the given data array and copies the given data byte-wise to the buffer
	template<typename T>
	void copyToBuffer(std::vector<std::byte>& buffer, const T& data)
	{
		buffer.resize(sizeof(T));
		std::memcpy(buffer.data(), &data, sizeof(T));
	}

	/// Resizes the given data array and appends the given data byte-wise to the end of the buffer
	template<typename T>
	void appendToBuffer(std::vector<std::byte>& buffer, const T& data)
	{
		std::size_t size = buffer.size();
		buffer.resize(size + sizeof(T));
		std::memcpy(buffer.data() + size, &data, sizeof(T));
	}

	void copyToBufferString(std::vector<std::byte>& buffer, const std::string& data);
	void appendToBufferString(std::vector<std::byte>& buffer, const std::string& data);

	void copyToBufferJSON(std::vector<std::byte>&)

}