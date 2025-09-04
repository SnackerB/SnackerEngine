#pragma once

namespace SnackerEngine
{

	/// initializes network functionality. Should be called once before
	/// any other network functionality is used.
	void initializeNetwork();

	/// Cleans up network functionality. Should be called once before
	/// the program exits
	void cleanupNetwork();

}