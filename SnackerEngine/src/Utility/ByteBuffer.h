#pragma once

#include "External/json.hpp"

#include <cinttypes>
#include <vector>
#include <bit>
#include <string>
#include <iostream> // DEBUG
#include <iomanip> // DEBUG

namespace SnackerEngine
{

	/// Resizes the given data array and copies the given data byte-wise to the buffer
	template<typename T>
	inline void copyToBuffer(std::vector<std::byte>& buffer, const T& data)
	{
		buffer.resize(sizeof(T));
		std::memcpy(buffer.data(), &data, sizeof(T));
	}

	/// Resizes the given data array and appends the given data byte-wise to the end of the buffer
	template<typename T>
	inline void appendToBuffer(std::vector<std::byte>& buffer, const T& data)
	{
		std::size_t size = buffer.size();
		buffer.resize(size + sizeof(T));
		std::memcpy(buffer.data() + size, &data, sizeof(T));
	}

	/// Reads data of the given type from the given byte-vector, if possible. Starts reading at 
	/// the given offset. If the type can be arbitrarily long (eg. string or json) it will take up
	/// the rest of the data. Returns an empty optional if reading fails. On success, returns a pair
	/// of the read object and the offset after reading
	template<typename T>
	inline std::optional<std::pair<T, unsigned int>> readFromBuffer(const std::vector<std::byte>& buffer, const unsigned int& offset = 0)
	{
		if (buffer.size() - offset < sizeof(T)) return {};
		T result{};
		std::memcpy(&result, buffer.data() + offset, sizeof(T));
		return std::make_pair(std::move(result), offset + sizeof(T));
	}

	/// Template for vectors of data
	template<typename T>
	inline void copyToBuffer(std::vector<std::byte>& buffer, const std::vector<T>& data)
	{
		buffer.resize(sizeof(T) * data.size());
		std::memcpy(buffer.data(), data.data(), sizeof(T) * data.size());
	}

	/// Template for vectors of data
	template<typename T>
	inline void appendToBuffer(std::vector<std::byte>& buffer, std::vector<T>& data)
	{
		std::size_t size = buffer.size();
		buffer.resize(size + sizeof(T) * data.size());
		std::memcpy(buffer.data() + size, data.data(), sizeof(T) * data.size());
	}

	template<>
	inline void copyToBuffer<std::string>(std::vector<std::byte>& buffer, const std::string& data)
	{
		buffer.resize(data.size());
		std::memcpy(buffer.data(), data.data(), data.size());
	}

	template<>
	inline void appendToBuffer<std::string>(std::vector<std::byte>& buffer, const std::string& data)
	{
		std::size_t size = buffer.size();
		buffer.resize(size + data.size());
		std::memcpy(buffer.data() + size, data.data(), data.size());
	}

	template<>
	inline void copyToBuffer<nlohmann::json>(std::vector<std::byte>& buffer, const nlohmann::json& data)
	{
		std::vector<std::uint8_t> v_bson = nlohmann::json::to_bson(data);
		buffer.resize(v_bson.size());
		std::memcpy(buffer.data(), v_bson.data(), v_bson.size() * sizeof(std::uint8_t));
		// DEBUG
		for (const auto& c : v_bson)
			std::cout << std::hex << static_cast<unsigned int>(c) << " ";
		std::cout << std::endl;
	}

	template<>
	inline void appendToBuffer<nlohmann::json>(std::vector<std::byte>& buffer, const nlohmann::json& data)
	{
		std::vector<std::uint8_t> v_bson = nlohmann::json::to_bson(data);
		std::size_t size = buffer.size();
		buffer.resize(size + v_bson.size() * sizeof(std::uint8_t));
		std::memcpy(buffer.data() + size, v_bson.data(), v_bson.size() * sizeof(std::uint8_t));
	}

	template<>
	inline std::optional<std::pair<std::string, unsigned int>> readFromBuffer<std::string>(const std::vector<std::byte>& buffer, const unsigned int& offset)
	{
		if (offset > buffer.size()) return {};
		std::string result(reinterpret_cast<const char*>(buffer.data() + offset), buffer.size() - offset);
		return std::make_pair(std::move(result), buffer.size());
	}

	template<>
	inline std::optional<std::pair<nlohmann::json, unsigned int>> readFromBuffer<nlohmann::json>(const std::vector<std::byte>& buffer, const unsigned int& offset)
	{
		if (offset > buffer.size()) return {};
		// TODO: Make more efficient by preventing double copying ... 
		std::vector<std::byte> bson;
		bson.resize(buffer.size() - offset);
		std::memcpy(bson.data(), buffer.data () + offset, buffer.size() - offset);
		// DEBUG
		for (const auto& c : bson)
			std::cout << std::hex << static_cast<unsigned int>(c) << " ";
		std::cout << std::endl;
		try
		{
			nlohmann::json result = nlohmann::json::from_bson(bson);
			return std::make_pair(std::move(result), buffer.size());
		}
		catch (const std::exception& exception)
		{
			return {};
		}
		/*
		if (offset > buffer.size()) return {};
		nlohmann::json result = nlohmann::json::from_bson(std::span(buffer.begin() + offset, buffer.end()));
		return std::make_pair(std::move(result), buffer.size());
		*/
	}

}