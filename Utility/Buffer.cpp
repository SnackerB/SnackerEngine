#include "Buffer.h"
#include "Exception.h"
#include "base64.h"

#include <sstream>
#include <fstream>

#include <iostream> // DEBUG

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
		return base64_encode((const unsigned char*)(&(*begin)), end - begin);
	}

	/// Helper function that creates a json binary from a begin and end const_iterator into
	/// a byte vector. This copies the data.
	static nlohmann::json::binary_t byteVectorToBinaryJson(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end) {
		std::vector<uint8_t> binary((end - begin) * sizeof(std::byte));
		memcpy(&binary[0], &(*begin), sizeof(std::byte) * (end - begin));
		return nlohmann::json::binary(std::move(binary));
	}

	/// Helper function that locates the position of the first byte in the vector matching
	/// the given byte, similar to std::string::find_first_of(...). If the given byte is not
	/// found, std::string::npos is returned instead.
	static std::size_t find_first_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, std::byte byte, std::size_t offset)
	{
		for (std::size_t i = offset; i < end - begin; ++i) {
 			if (*(std::next(begin, i)) == byte) return i;
		}
		return std::string::npos;
	}
	static std::size_t find_first_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::vector<std::byte>& bytes, std::size_t offset)
	{
		for (std::size_t i = offset; i < end - begin; ++i) {
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
		for (std::size_t i = offset; i < end - begin; ++i) {
			if (*(std::next(begin, i)) != byte) return i;
		}
		return std::string::npos;
	}
	static std::size_t find_first_not_of(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::vector<std::byte>& bytes, std::size_t offset)
	{
		for (std::size_t i = offset; i < end - begin; ++i) {
			for (const auto& byte : bytes) {
				if (*(std::next(begin, i)) != byte) return i;
			}
		}
		return std::string::npos;
	}

	/// Helper function that creates a new buffer by copying the bytes from the given buffer
	static Buffer copyBytes(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, std::size_t offset = 0, std::size_t n = SIZE_MAX)
	{
		std::vector<std::byte> newData = std::vector<std::byte>(std::min(n, (end - begin) - offset));
		if (newData.size() == 0) return Buffer(std::move(newData));
		memcpy(&(newData[0]), &(*(std::next(begin, offset))), newData.size());
		return Buffer(std::move(newData));
	}

	/// Helper function that compares a range of a byte vector to a string. Returns true
	/// on match and false otherwise
	static bool compare(const std::vector<std::byte>::const_iterator& begin, const std::vector<std::byte>::const_iterator& end, const std::string& string)
	{
		if ((end - begin) != string.length()) return false;
		for (std::size_t i = 0; i < (end - begin); ++i) {
			if (*(std::next(begin, i)) != static_cast<std::byte>(string[i])) return false;
		}
		return true;
	}

	BufferView BufferView::getBufferView(std::size_t offset, std::size_t size)
	{
		if (size == SIZE_MAX) return BufferView(std::next(_begin, offset), _end);
		return BufferView(_begin + offset, std::min(std::next(_begin, offset + size), _end));
	}

	ConstantBufferView BufferView::getBufferView(std::size_t offset, std::size_t size) const
	{
		if (size == SIZE_MAX) return BufferView(std::next(_begin, offset), _end);
		return ConstantBufferView(_begin + offset, std::min(std::next(_begin, offset + size), _end));
	}

	std::string BufferView::to_string() const
	{
		return byteVectorToString(_begin, _end);
	}

	std::string BufferView::to_string_base64() const
	{
		return byteVectorToStringBase64(_begin, _end);
	}

	nlohmann::json::binary_t BufferView::to_json_binary() const
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
		if (size == SIZE_MAX) return ConstantBufferView(std::next(_begin, offset), _end);
		return ConstantBufferView(_begin + offset, std::min(std::next(_begin, offset + size), _end));
	}

	std::string ConstantBufferView::to_string() const
	{
		return byteVectorToString(_begin, _end);
	}

	std::string ConstantBufferView::to_string_base64() const
	{
		return byteVectorToStringBase64(_begin, _end);
	}

	nlohmann::json::binary_t ConstantBufferView::to_json_binary() const
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

	Buffer ConstantBufferView::copyBytes(std::size_t offset, std::size_t n) const
	{
		return SnackerEngine::copyBytes(_begin, _end, offset, n);
	}

	bool ConstantBufferView::compare(const std::string& string) const
	{
		return SnackerEngine::compare(_begin, _end, string);
	}

	Buffer::Buffer(std::vector<std::byte>&& data)
		: data(std::move(data)) {}

	Buffer::Buffer(const std::string& data, bool base64)
		: data{}
	{
		if (data.empty()) return;
		if (base64) {
			this->data = std::move(base64_decode_to_byte_vector(data));
		}
		else {
			this->data.resize(data.length());
			std::memcpy(&this->data[0], data.c_str(), data.length());
		}
	}

	Buffer::Buffer(const nlohmann::json::binary_t& data)
		: Buffer(data.size())
	{
		std::memcpy(&this->data[0], &data[0], sizeof(std::byte) * data.size());
	}

	Buffer::Buffer(std::size_t size)
		: data(size) {}

	// Uses code from https://stackoverflow.com/questions/5778155/how-do-i-copy-the-contents-of-a-file-into-virtual-memory
	std::optional<Buffer> Buffer::loadFromFile(const std::string filename)
	{
		std::ifstream file(filename, std::ios::binary);
		if (file) {
			// Get file size
			file.seekg(0, std::ios::end);
			std::streampos length = file.tellg();
			file.seekg(0, std::ios::beg);
			// Copy data
			Buffer buffer(length);
			file.read((char*)(buffer.getDataPtr()), length);
			return buffer;
		}
		return std::nullopt;
	}

	BufferView Buffer::getBufferView(std::size_t offset, std::size_t size)
	{
		if (size == SIZE_MAX) return BufferView(std::next(data.begin(), offset), data.end());
		return BufferView(data.begin() + offset, std::min(std::next(data.begin(), offset + size), data.end()));
	}

	ConstantBufferView Buffer::getBufferView(std::size_t offset, std::size_t size) const
	{
		if (size == SIZE_MAX) return ConstantBufferView(std::next(data.begin(), offset), data.end());
		return ConstantBufferView(data.begin() + offset, std::min(std::next(data.begin(), offset + size), data.end()));
	}

	std::string Buffer::to_string() const
	{
		return SnackerEngine::byteVectorToString(data.begin(), data.end());
	}

	std::string Buffer::to_string_base_64() const
	{
		return base64_encode((const unsigned char*)(&data[0]), data.size());
	}

	nlohmann::json::binary_t Buffer::to_json_binary() const
	{
		std::vector<uint8_t> binary(data.size() * sizeof(std::byte));
		memcpy(&binary[0], &data[0], sizeof(std::byte) * data.size());
		return nlohmann::json::binary(std::move(binary));
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

	Buffer Buffer::copyBytes(std::size_t offset, std::size_t n) const
	{
		return SnackerEngine::copyBytes(data.begin(), data.end(), offset, n);
	}

	bool Buffer::compare(const std::string& string) const
	{
		return SnackerEngine::compare(data.begin(), data.end(), string);
	}

}
