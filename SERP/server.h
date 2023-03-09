#ifndef SERP_SERVER_H
#define SERP_SERVER_H

#include <string>

/// Initializes the SERP server. This should be called before registering the SIGTERM signal handle
/// and starting the main loop. If something fails, initialize will return false. In this case the daemon should
/// be terminated.
bool initialize();

/// catches the SIGTERM signal and tries to exit in a well-defined way.
void handleSignal(int signal);

/// Starts the main server loop
void startMainLoop();

#endif //SERP_SERVER_H