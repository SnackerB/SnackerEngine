#pragma once

#include <string>
#include <optional>

namespace SnackerEngine
{

	/// The SERPID is a unique 4 digit number given to each client connecting to a SERP
	/// server. The SERP server assigns an ID to each client. The SERPID 0000 is reserved for
	/// the SERP server
	using SERPID = unsigned;

	/// The SERPID of the SERP SERVER
	constexpr SERPID SERP_SERVER_ID = 0;

	/// Creates a random SERPID in the range [1, 9999]
	SERPID getRandomSerpID();

	/// Creates a string representation from the given SERPID.
	/// Assumes the SERPID is valid.
	std::string to_string(SERPID serpID);

	/// Parses the given string and returns the parsed SERPID.
	/// If the string does not represent a SERPID, an empty optional
	/// is returned instead.
	std::optional<SERPID> parseSERPID(const std::string& string);

}