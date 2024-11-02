#pragma once

#include <vector>
#include <bit>
#include <string>
#include <optional>
#include "Json.h"

namespace SnackerEngine
{

	/// Forward declaration of BufferView
	class BufferView;

	/// A constant bufferView is a view into a (partial) buffer, that is not allowed to
	/// modify the original buffer. With bufferView objects, efficient
	/// creation of subBuffers is possible. A subBuffer can only be created from an existing
	/// buffer. While using a subBuffer one should not move or destroy the original buffer!
	class ConstantBufferView
	{
	protected:
		friend class Buffer;
		friend class BufferView;
		std::vector<std::byte>::const_iterator _begin;
		std::vector<std::byte>::const_iterator _end;
		ConstantBufferView() = default;
		ConstantBufferView(std::vector<std::byte>::const_iterator begin, std::vector<std::byte>::const_iterator end)
			: _begin(begin), _end(end) {}
	public:
		/// A constant buffer view can be (implicitly) created from a non constant buffer view
		ConstantBufferView(const BufferView& bufferView);
		/// Access to iterators
		std::vector<std::byte>::const_iterator cbegin() const { return _begin; }
		std::vector<std::byte>::const_iterator cend() const { return _end; }
		/// Creates a bufferView of a part of this bufferView
		ConstantBufferView getBufferView(std::size_t offset = 0, std::size_t size = SIZE_MAX) const;
		/// Returns the size of the subBuffer in bytes
		std::size_t size() const { return std::distance(_begin, _end); }
		/// Returns a raw void pointer to the data
		const void* getDataPtr() const { return &*_begin; }
		/// Returns the string that is created when interpreting each byte of the buffer
		/// as a character. This copies the data.
		std::string to_string() const;
		std::string to_string_base64() const;
		/// Returns the data of the buffer serialized to json binary data
		nlohmann::json::binary_t to_json_binary() const;
		/// Returns the position of the first byte that matches the given byte(s).
		/// If no such position is found, std::string::npos is returned.
		std::size_t find_first_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Returns the position of the first byte that does not match the given byte(s).
		/// If no such position is found, std::string::npos is returned
		std::size_t find_first_not_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Creates a new buffer by copying from this buffer
		Buffer copyBytes(std::size_t offset = 0, std::size_t n = SIZE_MAX) const;
		/// Returns the byte at the given index
		std::byte get(std::size_t i) const { return *(_begin + i); }
		/// Overloading of the bracket operator []
		const std::byte& operator[](std::size_t i) const { return *(_begin + i); }
		/// Compares the buffer to a string and returns true on match
		bool compare(const std::string& string) const;
		/// Returns true if the buffer is empty
		bool empty() const { return cbegin() >= cend(); }
	};

	/// A bufferView is a view into a (partial) buffer. With bufferView objects, efficient
	/// creation of subBuffers is possible. A subBuffer can only be created from an existing
	/// buffer. While using a subBuffer one should not move or destroy the original buffer!
	class BufferView
	{
	protected:
		friend class Buffer;
		std::vector<std::byte>::iterator _begin;
		std::vector<std::byte>::iterator _end;
		BufferView() = default;
		BufferView(std::vector<std::byte>::iterator begin, std::vector<std::byte>::iterator end)
			: _begin(begin), _end(end) {}
	public:
		/// Access to iterators
		std::vector<std::byte>::iterator begin() { return _begin; }
		std::vector<std::byte>::iterator end() { return _end; }
		std::vector<std::byte>::const_iterator cbegin() const { return _begin; }
		std::vector<std::byte>::const_iterator cend() const { return _end; }
		/// Creates a bufferView of a part of this bufferView
		BufferView getBufferView(std::size_t offset = 0, std::size_t size = SIZE_MAX);
		ConstantBufferView getBufferView(std::size_t offset = 0, std::size_t size = SIZE_MAX) const;
		/// Returns the size of the subBuffer in bytes
		std::size_t size() const { return std::distance(_begin, _end); }
		/// Returns a raw void pointer to the data
		const void* getDataPtr() const { return &*_begin; }
		/// Returns the string that is created when interpreting each byte of the buffer
		/// as a character. This copies the data.
		std::string to_string() const;
		std::string to_string_base64() const;
		/// Returns the data of the buffer serialized to json binary data
		nlohmann::json::binary_t to_json_binary() const;
		/// Returns the position of the first byte that matches the given byte(s).
		/// If no such position is found, std::string::npos is returned.
		std::size_t find_first_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Returns the position of the first byte that does not match the given byte(s).
		/// If no such position is found, std::string::npos is returned
		std::size_t find_first_not_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Returns the byte at the given index
		std::byte get(std::size_t i) const { return *(_begin + i); }
		/// Sets the byte at the given index
		void set(std::size_t i, std::byte byte) { *(_begin + i) = byte; }
		/// Overloading of the bracket operator []
		std::byte& operator[](std::size_t i) { return *(_begin + i); }
		const std::byte& operator[](std::size_t i) const { return *(_begin + i); }
		/// Compares the buffer to a string and returns true on match
		bool compare(const std::string& string) const;
		/// Returns true if the buffer is empty
		bool empty() const { return cbegin() >= cend(); }
	};

	/// A buffer object can contain any data of any size. Basic functionality for modifying
	/// the data is implemented. Efficient creation of (constant) subBuffers is possible
	/// with subBuffer() and constSubBuffer() respectively.
	class Buffer
	{
	protected:
		/// The actual data as a pointer to a byte array
		std::vector<std::byte> data;
	public:
		/// Construct buffer filled with given data
		Buffer(std::vector<std::byte>&& data);
		/// Construct buffer by copying data from string.
		/// If base64 is set to true, base65 encoding is assumed
		Buffer(const std::string& data, bool base64 = false);
		/// Construct buffer by copying data from json binary
		Buffer(const nlohmann::json::binary_t& data);
		/// Construct buffer of given size with garbage data
		Buffer(std::size_t size = 0);
		/// Tries to store the contents of a file in a buffer. If the file is not found or could not be opened
		/// an empty optional is returned
		static std::optional<Buffer> loadFromFile(const std::string filename);
		/// Deleted Copy constructor and alignment operator
		Buffer(const Buffer& other) = delete;
		Buffer& operator=(const Buffer& other) = delete;
		/// Move constructor and assignment operator
		Buffer(Buffer&& other) = default;
		Buffer& operator=(Buffer&& other) = default;
		/// Returns a const ref to the data
		const std::vector<std::byte>& getData() const { return data; }
		/// Access to iterators
		std::vector<std::byte>::iterator begin() { return data.begin(); }
		std::vector<std::byte>::iterator end() { return data.end(); }
		std::vector<std::byte>::const_iterator cbegin() const { return data.begin(); }
		std::vector<std::byte>::const_iterator cend() const { return data.end(); }
		/// Creates a bufferView of a part of this bufferView
		BufferView getBufferView(std::size_t offset = 0, std::size_t size = SIZE_MAX);
		ConstantBufferView getBufferView(std::size_t offset = 0, std::size_t size = SIZE_MAX) const;
		/// Returns the size of the subBuffer in bytes
		std::size_t size() const { return data.size(); }
		/// Returns a raw void pointer to the data
		const void* getDataPtr() const { return &data[0]; }
		/// Returns the string that is created when interpreting each byte of the buffer
		/// as a character. This copies the data.
		std::string to_string() const;
		std::string to_string_base_64() const;
		/// Returns the data of the buffer serialized to json binary data
		nlohmann::json::binary_t to_json_binary() const;
		/// Returns the position of the first byte that matches the given byte(s).
		/// If no such position is found, std::string::npos is returned.
		std::size_t find_first_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Returns the position of the first byte that does not match the given byte(s).
		/// If no such position is found, std::string::npos is returned
		std::size_t find_first_not_of(std::byte byte, std::size_t offset = 0) const;
		std::size_t find_first_not_of(const std::vector<std::byte>& bytes, std::size_t offset = 0) const;
		/// Creates a new buffer by copying from this buffer
		Buffer copyBytes(std::size_t offset = 0, std::size_t n = SIZE_MAX) const;
		/// Returns the byte at the given index
		std::byte get(std::size_t i) const { return data[i]; }
		/// Sets the byte at the given index
		void set(std::size_t i, std::byte byte) { data[i] = byte; }
		/// Overloading of the bracket operator []
		std::byte& operator[](std::size_t i) { return data[i]; }
		const std::byte& operator[](std::size_t i) const { return data[i]; }
		/// Compares the buffer to a string and returns true on match
		bool compare(const std::string& string) const;
		/// Returns true if the buffer is empty
		bool empty() const { return cbegin() >= cend(); }
	};

}