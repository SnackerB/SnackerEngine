#include "Log.h"

#include <Windows.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
#define LOG_COLOR_ERROR 12
#define LOG_COLOR_WARNING 14
#define LOG_COLOR_INFO 15
	//------------------------------------------------------------------------------------------------------
	const ErrorLogger errorLogger;
	const WarningLogger warningLogger;
	const InfoLogger infoLogger;
	//------------------------------------------------------------------------------------------------------
	void Logger::startLog(const PRINT_MODE& mode) const
	{
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		FlushConsoleInputBuffer(hConsole);
		switch (mode)
		{
		case PRINT_MODE::MODE_ERROR: 
			SetConsoleTextAttribute(hConsole, LOG_COLOR_ERROR); break;
		case PRINT_MODE::MODE_WARNING:
			SetConsoleTextAttribute(hConsole, LOG_COLOR_WARNING); break;
		case PRINT_MODE::MODE_INFORMATION:
			SetConsoleTextAttribute(hConsole, LOG_COLOR_INFO); break;
		default:
			break;
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Logger::endLog() const
	{
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 15);
	}
	//------------------------------------------------------------------------------------------------------
	const ErrorLogger& ErrorLogger::operator<<(const LOGGER& token) const
	{
		switch (token)
		{
		case SnackerEngine::LOGGER::BEGIN:
			return *this << "[ERROR]: ";
		case SnackerEngine::LOGGER::ENDL:
			std::cout << std::endl; return *this;
		default:
			break;
		}
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	const WarningLogger& WarningLogger::operator<<(const LOGGER& token) const
	{
		switch (token)
		{
		case SnackerEngine::LOGGER::BEGIN:
			return *this << "[WARNING]: ";
		case SnackerEngine::LOGGER::ENDL:
			std::cout << std::endl; return *this;
		default:
			break;
		}
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	const InfoLogger& InfoLogger::operator<<(const LOGGER& token) const
	{
		switch (token)
		{
		case SnackerEngine::LOGGER::BEGIN:
			return *this << "[INFO]: ";
		case SnackerEngine::LOGGER::ENDL:
			std::cout << std::endl; return *this;
		default:
			break;
		}
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
}
