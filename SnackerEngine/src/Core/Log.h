#pragma once

#include <iostream>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Print mode of the Logger. Only used internally
	enum class PRINT_MODE
	{
		MODE_ERROR,
		MODE_WARNING,
		MODE_INFORMATION,
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
	protected:
		void startLog(const PRINT_MODE& mode) const;
		void endLog() const;
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing errors
	class ErrorLogger : public Logger
	{
	public:
		template<typename T>
		const ErrorLogger& operator<<(const T& val) const;
		const ErrorLogger& operator<<(const LOGGER&token) const;
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing warnings
	class WarningLogger : public Logger
	{
	public:
		template<typename T>
		const WarningLogger& operator<<(const T& val) const;
		const WarningLogger& operator<<(const LOGGER& token) const;
	};
	//------------------------------------------------------------------------------------------------------
	/// Logger for printing information
	class InfoLogger : public Logger
	{
	public:
		template<typename T>
		const InfoLogger& operator<<(const T& val) const;
		const InfoLogger& operator<<(const LOGGER& token) const;
	};
	//------------------------------------------------------------------------------------------------------
	/// These are the three loggers with which the user can interact.
	/// Example: 
	///		errorLogger << LOGGER::BEGIN << "an error occured :(" << LOGGER::ENDL;
	const extern ErrorLogger errorLogger;
	const extern WarningLogger warningLogger;
	const extern InfoLogger infoLogger;
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline const ErrorLogger& ErrorLogger::operator<<(const T& val) const
	{
		startLog(PRINT_MODE::MODE_ERROR);
		std::cout << val;
		endLog();
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline const WarningLogger& WarningLogger::operator<<(const T& val) const
	{
		startLog(PRINT_MODE::MODE_WARNING);
		std::cout << val;
		endLog();
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
	template<typename T>
	inline const InfoLogger& InfoLogger::operator<<(const T& val) const
	{
		startLog(PRINT_MODE::MODE_INFORMATION);
		std::cout << val;
		endLog();
		return *this;
	}
	//------------------------------------------------------------------------------------------------------
}