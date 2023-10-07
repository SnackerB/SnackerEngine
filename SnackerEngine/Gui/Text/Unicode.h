#pragma once

#include <optional>
#include <string>
#include <sstream>
#include <vector>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	using Unicode = unsigned int;
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns the next codepoint and advances the position.
	/// The string should be encoded in UTF8 format
	inline std::optional<Unicode> getNextCodepointUTF8(const std::string& text, unsigned int& position)
	{
		const char* c = &text.at(position);
		if (!(*c & 0b10000000)) {
			position++;
			return static_cast<Unicode>(*c & 0b01111111);
		}
		// At least two chars
		if ((*c & 0b11100000) == 0b11000000) {
			position += 2;
			return (static_cast<Unicode>(*c & 0b00011111) << 6) |
				static_cast<Unicode>(*(c + 1) & 0b00111111);
		}
		// At least three chars
		if ((*c & 0b11110000) == 0b11100000) {
			position += 3;
			return (static_cast<Unicode>(*c & 0b00001111) << 11) |
				(static_cast<Unicode>(*(c + 1) & 0b00111111) << 6) |
				static_cast<Unicode>(*(c + 2) & 0b00111111);
		}
		// Four chars
		if ((*c & 0b11111000) == 0b11110000) {
			position += 4;
			return (static_cast<Unicode>(*c & 0b00000111) << 15) |
				(static_cast<Unicode>(*(c + 1) & 0b00111111) << 12) |
				(static_cast<Unicode>(*(c + 2) & 0b00111111) << 6) |
				static_cast<Unicode>(*(c + 3) & 0b00111111);
		}
		// Invalid Codepoint
		return {};
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns true if the given codepoint is considered a whitespace character
	inline bool isWhiteSpace(const Unicode& codepoint)
	{
		return
			codepoint == 0x0020 ||	// space
			codepoint == 0x0009 ||	// tab
			codepoint == 0x0085 ||	// newline
			codepoint == 0x000A;	// newline
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that resturns true if the given codepoint is the space character
	inline bool isSpaceCharacter(const Unicode& codepoint)
	{
		return codepoint == 0x0020;
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns true if the given codepoint is the newline character '\n'
	inline bool isNewline(const Unicode& codepoint)
	{
		return
			codepoint == 0x0085 ||
			codepoint == 0x000A;
	}
	//--------------------------------------------------------------------------------------------------
	/// Helper function that returns the 'newline' codepoint
	inline Unicode getNewlineCodepoint()
	{
		return 0x0085;
	}
	//--------------------------------------------------------------------------------------------------
	/// Creates a UTF8 encoded string from a std::u8string
	std::string encodeUTF8(std::u8string input);
	//--------------------------------------------------------------------------------------------------
	/// Converts a vector of unciode characters to an UTF8 encoded string
	std::string encodeUTF8(const std::vector<Unicode>& codepoints);
	//--------------------------------------------------------------------------------------------------
	/// Appends the UTF8 representation of the given codepoint to the chars vector
	void appendUnicodeCharacter(std::vector<char>& charsUTF8, const Unicode& codepoint);
	//--------------------------------------------------------------------------------------------------
	/// Returns true if the given unicode character is an alphabetic character
	bool isAlpha(const Unicode& codepoint);
	//--------------------------------------------------------------------------------------------------
	/// Returns true if the given unicode character is a numeric character
	bool isNumeric(const Unicode& codepoint);
	//--------------------------------------------------------------------------------------------------
}