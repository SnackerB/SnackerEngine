#include "Buffer.h"
#include "base64.h"
#include "Assert/Assert.h"

#include <External/nlohmann_json/json.hpp>
#include <sstream>
#include <fstream>
#include <memory>
#include <algorithm>
#include <utility>
#include <optional>
#include <string.h>
#include <iterator>
#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>
#include <filesystem>

namespace SnackerEngine
{

	/// Helper function that creates a string from a begin and end const_iterator into
	/// a byte vector. This copies the data.
	static std::string byteVectorToString(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end) {
		std::stringstream result;
		for (std::vector<std::byte>::const_iterator cit = begin; cit < end; cit++) {
			result << static_cast<char>(*cit);
		}
		return result.str();
	}

	/// Helper function that creates a base64 encoded string from a begin and end const_iterator into
	/// a byte vector. This copies the data.
	static std::string byteVectorToStringBase64(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end) {
		return base64_encode(reinterpret_cast<const unsigned char*>(std::to_address(begin)), static_cast<std::size_t>(end - begin));
	}

	/// Helper function that creates a json binary from a begin and end const_iterator into
	/// a byte vector. This copies the data.
	static nlohmann::json::binary_t byteVectorToBinaryJson(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end) {
		std::vector<std::uint8_t> binary(static_cast<std::size_t>(end - begin));
		// Copy data and transform byte to uint8_t as expected from nlohmann::json::binary.
		std::transform(begin, end, binary.begin(), [](std::byte b) { return std::to_integer<std::uint8_t>(b); });
		return nlohmann::json::binary(std::move(binary));
	}

	/// Helper function that locates the position of the first byte in the vector matching
	/// the given byte, similar to std::string::find_first_of(...). If the given byte is not
	/// found, std::string::npos is returned instead.
	static std::size_t find_first_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, std::byte byte, std::size_t offset)
	{
		for (std::size_t i = offset; i < static_cast<std::size_t>(end - begin); ++i) {
 			if (*(std::next(begin, i)) == byte) return i;
		}
		return std::string::npos;
	}
	static std::size_t find_first_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::vector<std::byte>& bytes, std::size_t offset)
	{
		for (std::size_t i = offset; i < static_cast<std::size_t>(end - begin); ++i) {
			for (const auto& byte : bytes) {
				if (*(std::next(begin, i)) == byte) return i;
			}
		}
		return std::string::npos;
	}

	/// Helper function that locates the position of the first byte(s) in the vector not matching
	/// the given byte, similar to std::string::find_first_not_of(...). If all bytes match the 
	/// given byte, std::string::npos is returned instead.
	static std::size_t find_first_not_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, std::byte byte, std::size_t offset)
	{
		for (std::size_t i = offset; i < static_cast<std::size_t>(end - begin); ++i) {
			if (*(std::next(begin, i)) != byte) return i;
		}
		return std::string::npos;
	}
	static std::size_t find_first_not_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::vector<std::byte>& bytes, std::size_t offset)
	{
		for (std::size_t i = offset; i < static_cast<std::size_t>(end - begin); ++i) {
			bool foundMatch = false;
			for (const auto& byte : bytes) {
				if (*(std::next(begin, i)) == byte) {
					foundMatch = true;
					break;
				}
			}
			if (!foundMatch) return i;
		}
		return std::string::npos;
	}

	/// Helper function that creates a new buffer by copying the bytes from the given buffer
	static Buffer copyBytes(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, std::size_t offset = 0, std::size_t size = SIZE_MAX)
	{
		auto [offset, size] = clampOffsetAndSize(static_cast<std::size_t>(end - begin), offset, size);
		auto newBegin = begin + static_cast<std::ptrdiff_t>(offset);
		return Buffer(std::vector<std::byte>(newBegin, newBegin + static_cast<ptrdiff_t>(size)));
	}

	/// Helper function that compares a range of a byte vector to a string. Returns true
	/// on match and false otherwise
	static bool compare(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::string& string)
	{
		if ((end - begin) != string.length()) return false;
		for (std::size_t i = 0; i < static_cast<std::size_t>(end - begin); ++i) {
			if (*(std::next(begin, i)) != static_cast<std::byte>(string[i])) return false;
		}
		return true;
	}

	/// Helper function that clamps a requested (offset, size) pair to a range of totalSize elements.
	/// The result always describes a valid (possibly empty) sub-range.
	static std::pair<std::size_t, std::size_t> clampOffsetAndSize(std::size_t totalSize, std::size_t offset, std::size_t size)
	{
		ASSERT(offset + size <= totalSize);
		offset = std::min(offset, totalSize);
		size = std::min(size, totalSize - offset);
		return { offset, size };
	}

	BufferView BufferView::getBufferView(std::size_t offset, std::size_t size)
	{
		auto [offset, size] = clampOffsetAndSize(this->size(), offset, size);
		auto begin = _begin + static_cast<std::ptrdiff_t>(offset);
		return BufferView(begin + offset, begin + static_cast<std::ptrdiff_t>(size));
	}

	ConstantBufferView BufferView::getBufferView(std::size_t offset, std::size_t size) const
	{
		return ConstantBufferView(*this).getBufferView(offset, size);
	}

	std::string BufferView::to_string() const
	{
		return byteVectorToString(_begin, _end);
	}

	std::string BufferView::to_string_base64() const
	{
		return byteVectorToStringBase64(_begin, _end);
	}

	nlohmann::json::binary_t BufferView::to_Json_binary() const
	{
		return byteVectorToBinaryJson(_begin, _end);
	}

	std::size_t BufferView::find_first_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(_begin, _end, byte, offset);
	}

	std::size_t BufferView::find_first_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(_begin, _end, bytes, offset);
	}

	std::size_t BufferView::find_first_not_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(_begin, _end, byte, offset);
	}

	std::size_t BufferView::find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(_begin, _end, bytes, offset);
	}

	Buffer BufferView::copyBytes(std::size_t offset, std::size_t size) const
	{
		return SnackerEngine::copyBytes(_begin, _end, offset, size);
	}

	bool BufferView::compare(const std::string& string) const
	{
		return SnackerEngine::compare(_begin, _end, string);
	}

	ConstantBufferView::ConstantBufferView(const BufferView& bufferView)
		: _begin(bufferView.cbegin()), _end(bufferView.cend()) 
	{
	}

	ConstantBufferView ConstantBufferView::getBufferView(std::size_t offset, std::size_t size) const
	{
		auto [offset, size] = clampOffsetAndSize(this->size(), offset, size);
		auto begin = _begin + static_cast<std::ptrdiff_t>(offset);
		return ConstantBufferView(begin + offset, begin + static_cast<std::ptrdiff_t>(size));
	}

	std::string ConstantBufferView::to_string() const
	{
		return byteVectorToString(_begin, _end);
	}

	std::string ConstantBufferView::to_string_base64() const
	{
		return byteVectorToStringBase64(_begin, _end);
	}

	nlohmann::json::binary_t ConstantBufferView::to_Json_binary() const
	{
		return byteVectorToBinaryJson(_begin, _end);
	}

	std::size_t ConstantBufferView::find_first_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(_begin, _end, byte, offset);
	}

	std::size_t ConstantBufferView::find_first_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(_begin, _end, bytes, offset);
	}

	std::size_t ConstantBufferView::find_first_not_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(_begin, _end, byte, offset);
	}

	std::size_t ConstantBufferView::find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(_begin, _end, bytes, offset);
	}

	Buffer ConstantBufferView::copyBytes(std::size_t offset, std::size_t size) const
	{
		return SnackerEngine::copyBytes(_begin, _end, offset, size);
	}

	bool ConstantBufferView::compare(const std::string& string) const
	{
		return SnackerEngine::compare(_begin, _end, string);
	}

	Buffer::Buffer(std::vector<std::byte>&& data)
		: data(std::move(data)) {}

	Buffer::Buffer(const std::string& data)
		: data{}
	{
		if (data.empty()) return;
		this->data.resize(data.length());
		std::memcpy(&this->data[0], data.c_str(), data.length());
	}

	Buffer::Buffer(const nlohmann::json::binary_t& data)
		: Buffer(data.size())
	{
		std::memcpy(&this->data[0], &data[0], sizeof(std::byte) * data.size());
	}

	Buffer::Buffer(std::size_t size)
		: data(size) {}

	std::optional<Buffer> Buffer::fromBase64(const std::string& encodedData)
	{
		try {
			return Buffer(base64_decode_to_byte_vector(encodedData));
		}
		catch (const std::runtime_error&) { // Invalid character
			return std::nullopt;
		}
		catch (const std::runtime_error&) { // Truncated input
			return std::nullopt;
		}
	}

	// Uses modified code from https://stackoverflow.com/questions/5778155/how-do-i-copy-the-contents-of-a-file-into-virtual-memory
	std::optional<Buffer> Buffer::loadFromFile(const std::filesystem::path &filename)
	{
		std::ifstream file(filename, std::ios::binary);
		if (!file) return std::nullopt;
		// Get file size (use offset, since file is opened at the end)
		const std::streamoff length = file.tellg();
		if (length < 0) return std::nullopt;
		// Move stream to file beginning
		file.seekg(0, std::ios::beg);
		// Copy data
		Buffer buffer(static_cast<std::size_t>(length));
		if (length > 0 && !file.read(reinterpret_cast<char*>(buffer.data.data()), length)) return std::nullopt; // Return nullopt if read fails.
		return buffer;
	}

	BufferView Buffer::getBufferView(std::size_t offset, std::size_t size)
	{
		return BufferView(data.begin(), data.end()).getBufferView(offset, size);
	}

	ConstantBufferView Buffer::getBufferView(std::size_t offset, std::size_t size) const
	{
		return ConstantBufferView(data.begin(), data.end()).getBufferView(offset, size);
	}

	const void* Buffer::getDataPtr() const
	{
		return data.data();
	}

	std::string Buffer::to_string() const
	{
		return SnackerEngine::byteVectorToString(data.begin(), data.end());
	}

	std::string Buffer::to_string_base64() const
	{
		return SnackerEngine::byteVectorToStringBase64(data.cbegin(), data.cend());
	}

	nlohmann::json::binary_t Buffer::to_json_binary() const
	{

		return SnackerEngine::byteVectorToBinaryJson(data.cbegin(), data.cend());
	}

	std::size_t Buffer::find_first_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(data.begin(), data.end(), byte, offset);
	}

	std::size_t Buffer::find_first_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_of(data.begin(), data.end(), bytes, offset);
	}

	std::size_t Buffer::find_first_not_of(std::byte byte, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(data.begin(), data.end(), byte, offset);
	}

	std::size_t Buffer::find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset) const
	{
		return SnackerEngine::find_first_not_of(data.begin(), data.end(), bytes, offset);
	}

	Buffer Buffer::copyBytes(std::size_t offset, std::size_t size) const
	{
		return SnackerEngine::copyBytes(data.begin(), data.end(), offset, size);
	}

	bool Buffer::compare(const std::string& string) const
	{
		return SnackerEngine::compare(data.begin(), data.end(), string);
	}

}
