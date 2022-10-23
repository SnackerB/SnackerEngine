#include "Gui/Text/Unicode.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	std::string encodeUTF8(std::u8string input)
	{
		return std::string(input.begin(), input.end());
	}
	//--------------------------------------------------------------------------------------------------
	void appendUnicodeCharacter(std::stringstream& ss, const Unicode& codepoint)
	{
		if (codepoint < 128) {
			// one UTF8 character
			ss << (static_cast<char>(codepoint));
		}
		else if (codepoint < 2048) {
			// two UTF8 characters
			ss << (static_cast<char>(0b11000000) + static_cast<char>((0b11111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) + static_cast<char>(0b111111 & codepoint));
		}
		else if (codepoint < 65536) {
			// three UTF8 characters
			ss << (static_cast<char>(0b11100000) + static_cast<char>((0b1111000000000000 & codepoint) >> 12));
			ss << (static_cast<char>(0b10000000) + static_cast<char>((0b111111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) + static_cast<char>(0b111111 & codepoint));
		}
		else {
			// four UTF8 characters
			ss << (static_cast<char>(0b11100000) + static_cast<char>((0b111000000000000000000 & codepoint) >> 18));
			ss << (static_cast<char>(0b10000000) + static_cast<char>((0b111111000000000000 & codepoint) >> 12));
			ss << (static_cast<char>(0b10000000) + static_cast<char>((0b111111000000 & codepoint) >> 6));
			ss << (static_cast<char>(0b10000000) + static_cast<char>(0b111111 & codepoint));
		}
	}
	//--------------------------------------------------------------------------------------------------
}