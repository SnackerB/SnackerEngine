#include "Log.h"

#include <Windows.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	constexpr auto LOG_COLOR_ERROR = 12;
	constexpr auto LOG_COLOR_WARNING = 14;
	constexpr auto LOG_COLOR_INFO = 15;
	//------------------------------------------------------------------------------------------------------
	ErrorLogger errorLogger;
	WarningLogger warningLogger;
	InfoLogger infoLogger;
	//------------------------------------------------------------------------------------------------------
	void setConsoleColorInternal(WORD color)
	{
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		FlushConsoleInputBuffer(hConsole);
		SetConsoleTextAttribute(hConsole, color);
	}
	//------------------------------------------------------------------------------------------------------
	void Logger::resetConsoleColor()
	{
		HANDLE  hConsole;
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, 15);
	}
	//------------------------------------------------------------------------------------------------------
	Logger& Logger::operator<<(const LOGGER& token)
	{
		switch (token)
		{
		case LOGGER::BEGIN:
		{
			startLog();
			return *this;
		}
		case LOGGER::ENDL:
		{
			return *(static_cast<Logger*>(this)) << '\n';
		}
		}
	}
	//------------------------------------------------------------------------------------------------------
	void Logger::logToConsole()
	{
		mode = LOG_MODE::CONSOLE;
		filename = "";
	}
	//------------------------------------------------------------------------------------------------------
	void Logger::logToFile(const std::string& filename)
	{
		mode = LOG_MODE::FILE;
		this->filename = filename;
	}
	//------------------------------------------------------------------------------------------------------
	void ErrorLogger::startLog()
	{
		*(static_cast<Logger*>(this)) << "[ERROR]: ";
	}
	//------------------------------------------------------------------------------------------------------
	void ErrorLogger::setConsoleColor()
	{
		setConsoleColorInternal(LOG_COLOR_ERROR);
	}
	//------------------------------------------------------------------------------------------------------
	void WarningLogger::startLog()
	{
		*(static_cast<Logger*>(this)) << "[WARNING]: ";
	}
	//------------------------------------------------------------------------------------------------------
	void WarningLogger::setConsoleColor()
	{
		setConsoleColorInternal(LOG_COLOR_WARNING);
	}
	//------------------------------------------------------------------------------------------------------
	void InfoLogger::startLog()
	{
		*(static_cast<Logger*>(this)) << "[INFO]: ";
	}
	//------------------------------------------------------------------------------------------------------
	void InfoLogger::setConsoleColor()
	{
		setConsoleColorInternal(LOG_COLOR_INFO);
	}
	//------------------------------------------------------------------------------------------------------
}
