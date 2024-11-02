#include "Gui/Text/Unicode.h"
#include <vector>

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	std::string encodeUTF8(std::u8string input)
	{
		return std::string(input.begin(), input.end());
	}
	//--------------------------------------------------------------------------------------------------
	/*
	void appendUnicodeCharacter(std::stringstream& ss, const Unicode& codepoint)
	{
		std::vector<char> chars;
		if (codepoint < 128) {
			// one UTF8 character
			ss << (static_cast<char>(codepoint));
		}
		else if (codepoint < 2048) {
			// two UTF8 characters
			ss << (static_cast<char>(0b11000000) | static_cast<char>((0b11111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
		else if (codepoint	< 65536) {
			// three UTF8 characters
			ss << (static_cast<char>(0b11100000) | static_cast<char>((0b1111000000000000 & codepoint) >> 12));
			ss << (static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
		else {
			// four UTF8 characters
			ss << (static_cast<char>(0b11100000) | static_cast<char>((0b111000000000000000000 & codepoint) >> 18));
			ss << (static_cast<char>(0b10000000) | static_cast<char>((0b111111000000000000 & codepoint) >> 12));
			ss << (static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
	}
	*/
	//--------------------------------------------------------------------------------------------------
	std::string encodeUTF8(const std::vector<Unicode>& codepoints)
	{
		std::vector<char> chars;
		for (const auto& codepoint : codepoints)
		{
			if (codepoint < 128) {
				// one UTF8 character
				chars.push_back(static_cast<char>(codepoint));
			}
			else if (codepoint < 2048) {
				// two UTF8 characters
				chars.push_back(static_cast<char>(0b11000000) | static_cast<char>((0b11111000000 & codepoint) >> 6));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
			}
			else if (codepoint < 65536) {
				// three UTF8 characters
				chars.push_back(static_cast<char>(0b11100000) | static_cast<char>((0b1111000000000000 & codepoint) >> 12));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
			}
			else {
				// four UTF8 characters
				chars.push_back(static_cast<char>(0b11100000) | static_cast<char>((0b111000000000000000000 & codepoint) >> 18));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000000000 & codepoint) >> 12));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
				chars.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
			}
		}
		return std::string(chars.begin(), chars.end());
	}
	//--------------------------------------------------------------------------------------------------
	void appendUnicodeCharacter(std::vector<char>& charsUTF8, const Unicode& codepoint)
	{
		if (codepoint < 128) {
			// one UTF8 character
			charsUTF8.push_back(static_cast<char>(codepoint));
		}
		else if (codepoint < 2048) {
			// two UTF8 characters
			charsUTF8.push_back(static_cast<char>(0b11000000) | static_cast<char>((0b11111000000 & codepoint) >> 6));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
		else if (codepoint < 65536) {
			// three UTF8 characters
			charsUTF8.push_back(static_cast<char>(0b11100000) | static_cast<char>((0b1111000000000000 & codepoint) >> 12));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
		else {
			// four UTF8 characters
			charsUTF8.push_back(static_cast<char>(0b11100000) | static_cast<char>((0b111000000000000000000 & codepoint) >> 18));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000000000 & codepoint) >> 12));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>((0b111111000000 & codepoint) >> 6));
			charsUTF8.push_back(static_cast<char>(0b10000000) | static_cast<char>(0b111111 & codepoint));
		}
	}
	//--------------------------------------------------------------------------------------------------
}