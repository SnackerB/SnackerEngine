#pragma once

#include <iostream>
#include <fstream>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// The mode of the logger. Can be set to log to the console or a file respectively
	enum class LOG_MODE
	{
		CONSOLE,
		FILE,
	};
	//------------------------------------------------------------------------------------------------------
	/// Special logger tokens: Do something special when streamed into a logger!
	enum class LOGGER {
		BEGIN, /// Prints "[ERROR]: ", "[WARNING]: " or "[INFO]: ", depending on which logger is used 
		ENDL, /// ends the current line, similar to std::endl
	};
	//------------------------------------------------------------------------------------------------------
	/// Base Logger class. Only used internally
	class Logger
	{
	private:
		LOG_MODE mode = LOG_MODE::CONSOLE;
		std::string filename = "";
	protected:
		virtual void startLog() {};
		virtual void setConsoleColor() {};
		void resetConsoleColor();
	public:
		template<typename T>
		Logger& operator<<(const T& val);
		virtual Logger& operator<<(const LOGGER& token);
		void logToConsole();
		void logToFile(const std::string& filename);
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing errors
	class ErrorLogger : public Logger
	{
	protected:
		void startLog() override;
		void setConsoleColor() override;
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing warnings
	class WarningLogger : public Logger
	{
	protected:
		void startLog() override;
		void setConsoleColor() override;
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing information
	class InfoLogger : public Logger
	{
	protected:
		void startLog() override;
		void setConsoleColor() override;
	};
	//------------------------------------------------------------------------------------------------------
	/// These are the three loggers with which the user can interact.
	/// Example: 
	///		errorLogger << LOGGER::BEGIN << "an error occured :(" << LOGGER::ENDL;
	extern ErrorLogger errorLogger;
	extern WarningLogger warningLogger;
	extern InfoLogger infoLogger;
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline Logger& Logger::operator<<(const T& val)
	{
		switch (mode)
		{
		case SnackerEngine::LOG_MODE::CONSOLE:
		{
			setConsoleColor();
			std::cout << val;
			resetConsoleColor();
			std::cout.flush();
			return *this;
		}
		case SnackerEngine::LOG_MODE::FILE:
		{
			try
			{
				std::ofstream out(filename, std::ios::app);
				out << val;
			}
			catch (const std::exception&)
			{
				std::cout << "Error while trying to write to log file ..." << std::endl;
			}
			return *this;
		}
		default:
			break;
		}
	}
	//------------------------------------------------------------------------------------------------------
}