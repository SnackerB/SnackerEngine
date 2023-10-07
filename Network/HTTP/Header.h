#pragma once

#include <string>
#include <optional>

#include "Utility/Buffer.h"

namespace SnackerEngine
{

	/// Struct representing a HTTP header, consisting of a name and a value, seperated by a colon.
	/// Whitespaces before the colon are ignored
	struct HTTPHeader
	{
		std::string name;
		std::string value;
		/// Constructor
		HTTPHeader() = default;
		HTTPHeader(const std::string& name, const std::string& value)
			: name(name), value(value) {}
		/// Parses a header from a bufferView
		static std::optional<HTTPHeader> parse(const ConstantBufferView& buffer);
		/// Transforms the header to a string
		std::string toString() const;
		/// Transforms the header to a buffer
		Buffer toBuffer() const;
	};

}