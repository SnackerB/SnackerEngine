#pragma once

#include <string>
#include <optional>

namespace SnackerEngine
{

	/// The SERPID is a unique 4 digit number given to each client connecting to a SERP
	/// server. The SERP server assigns an ID to each client. The SERPID 0000 is reserved for
	/// the SERP server
	struct SERPID
	{
	private:
		unsigned int id;
	public:
		SERPID(unsigned int id = 0)
			: id{ id < 10000 ? id : 9999 } {}
		SERPID(uint16_t id)
			: SERPID(unsigned int(id)) {}
		operator unsigned int() const { return id; }
		static const SERPID SERVER_ID;
	};

	/// Creates a random SERPID in the range [1, 9999]
	SERPID getRandomSerpID();

}