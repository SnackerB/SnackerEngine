#include "server.h"
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <syslog.h>
#include <fstream>
#include <cstring>
#include <optional>
//----------------------------------------------------------------------------------------------------------------------
/// Displays information on how this program should be called
static void printHelp()
{
    std::cout << "Usage: ./serp <mode>" << std::endl;
    std::cout << "with <mode> one of the following:" << std::endl;
    std::cout << "\t\"start\": starts the SERP daemon if it is not yet running" << std::endl;
    std::cout << "\t\"end\": ends the SERP daemon if it is running" << std::endl;
}
//----------------------------------------------------------------------------------------------------------------------
/// Checks if a process with a given pid currently exists.
/// If an error occurs while checking, an empty optional is returned
static std::optional<bool> checkIfProcessExists(pid_t pid)
{
    // Try to send a signal to the process
    if (kill(pid, 0) == 0) return true;
    if (errno == ESRCH) return false;
    return {};
}
//----------------------------------------------------------------------------------------------------------------------
/// Reads the daemon PID from the given filepath.
/// If the file is not found or the PID is not valid, an empty optional is returned.
static std::optional<pid_t> readDaemonPID(const char* filepath)
{
    std::ifstream f(filepath);
    if (!f.is_open()) return {};
    pid_t pid;
    f >> pid;
    if (pid == 0) return {};
    return pid;
}
//----------------------------------------------------------------------------------------------------------------------
/// Writes the daemon PID to the given filepath. Returns true on success and false on failure.
static bool writeDaemonPID(const pid_t& pid, const char* filepath)
{
    std::ofstream f(filepath);
    if (f.is_open()) {
        f << pid << std::endl;
    }
    else {
        return false;
    }
    f.close();
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
/// Call at the beginning of the program to create a daemon process and terminate the parent process.
/// Reference: https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
static void daemonize()
{
    pid_t pid;
    // Fork off the parent process
    pid = fork();
    // Check for error
    if (pid < 0) {
        std::cout << "[ERROR]: an error occurred while starting daemon." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);
    // On success: The child process becomes session leader
    if (setsid() < 0){
        std::cout << "[ERROR]: an error occurred while starting daemon." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Catch, ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    // Fork off for the second time
    pid = fork();
    // Check for error
    if (pid < 0) {
        std::cout << "[ERROR]: an error occurred while starting daemon." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);
    // Write PID to file
    if (!writeDaemonPID(getpid(), "SERP_PID.txt")) {
        // Can't write PID to file
        std::cout << "[ERROR]: an error occurred while starting daemon." << std::endl;
        exit(EXIT_FAILURE);
    }
    // Print success message
    std::cout << "[INFO]: started SERP daemon with pid " << getpid() << " successfully." << std::endl;
    // Set new file permissions
    umask(0);
    // Close all open file descriptors
    for (long x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (static_cast<int>(x));
    }
}
//----------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printHelp();
    }
    else if (strcmp(argv[1], "start") == 0) {
        // Get daemon pid
        auto pid = readDaemonPID("SERP_PID.txt");
        if (!pid.has_value()) {
            std::cout << "[ERROR]: an error occurred while trying to reading daemon PID from file." << std::endl;
            return EXIT_FAILURE;
        }
        // Check if daemon already exists
        auto exists = checkIfProcessExists(pid.value());
        if (!exists.has_value()) {
            std::cout << "[ERROR]: an error occurred while checking if daemon was already running." << std::endl;
            return EXIT_FAILURE;
        }
        if (exists.value()) {
            std::cout << "[ERROR]: SERP daemon is already running." << std::endl;
            return EXIT_FAILURE;
        }
        // Start daemon
        daemonize();
        // Initialize SERP server
        if (initialize()) {
            // Set signal handler
            struct sigaction sigbreak{};
            sigbreak.sa_handler = &handleSignal;
            sigemptyset(&sigbreak.sa_mask);
            sigbreak.sa_flags = 0;
            sigaction(SIGTERM, &sigbreak, nullptr);
            // daemon loop
            startMainLoop();
        }
    }
    else if (strcmp(argv[1], "kill") == 0) {
        /// Get daemon pid
        auto pid = readDaemonPID("SERP_PID.txt");
        if (!pid.has_value()) {
            std::cout << "[ERROR]: an error occurred while trying to reading daemon PID from file." << std::endl;
            return EXIT_FAILURE;
        }
        if (kill(pid.value(), SIGTERM) < 0) {
            std::cout << "[ERROR]: an error occurred while trying to kill daemon." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "[INFO]: killed SERP daemon with pid " << pid.value() << " successfully." << std::endl;
    }
    else {
        printHelp();
    }
    return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------------------------------------------------