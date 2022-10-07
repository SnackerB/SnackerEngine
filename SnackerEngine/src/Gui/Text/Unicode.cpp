#include "Gui/Text/Unicode.h"

namespace SnackerEngine
{

	std::string encodeUTF8(std::u8string input)
	{
		return std::string(input.begin(), input.end());
	}

}