//
//  base64 encoding and decoding with C++.
//  taken from https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp/
//
// Modified for SnackerEngine:
//	- Moved function into SnackerEngine namespace
//	- added base64_decode_to_byte_vector()
//	- decode() templated over the output container
//	- changed include gurd to "# pragma once"
//

#include <string>
#include <string_view>
#include <vector>
#include <cstddef>

namespace SnackerEngine
{

	std::string base64_encode(std::string const& s, bool url = false);
	std::string base64_encode_pem(std::string const& s);
	std::string base64_encode_mime(std::string const& s);

	std::string base64_decode(std::string const& s, bool remove_linebreaks = false);
	std::string base64_encode(unsigned char const*, size_t len, bool url = false);
	std::vector<std::byte> base64_decode_to_byte_vector(std::string const& s, bool remove_linebreaks = false);

	//
	// Interface with std::string_view rather than const std::string&
	// Requires C++17
	// Provided by Yannic Bonenberger (https://github.com/Yannic)
	//
	std::string base64_encode(std::string_view s, bool url = false);
	std::string base64_encode_pem(std::string_view s);
	std::string base64_encode_mime(std::string_view s);

	std::string base64_decode(std::string_view s, bool remove_linebreaks = false);

}