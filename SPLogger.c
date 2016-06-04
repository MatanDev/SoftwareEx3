#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#define ERROR_MSG "---ERROR---\n"
#define WARNING_MSG  "---WARNING---\n"
#define INFO_MSG  "---INFO---\n"
#define DEBUG_MSG  "---DEBUG---\n"

#define GENERAL_MESSAGE_SKELETON "%s- file: %s\n- function: %s\n- line: %d\n- message: %s"
#define SHORT_MESSAGE_SKELETON "%s- message: %s"

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

/*
 * The given message is printed as string format.
 * A new line is printed at the end of msg
 * The message will be printed in all levels.
 *
 * @param msg - The message to be printed
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If msg is null
 * SP_LOGGER_WRITE_FAIL			- If Write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrintFormmatedString(const char* msg, ...){
	if (!logger)
		return SP_LOGGER_UNDIFINED;
	if (!msg)
		return SP_LOGGER_INVAlID_ARGUMENT;

    va_list args;
    va_start(args, msg);

	if (vfprintf(logger->outputChannel, msg, args) < 0)
		return SP_LOGGER_WRITE_FAIL;

	va_end(args);

	if (fprintf(logger->outputChannel, "\n") < 0) // prints a new line
		return SP_LOGGER_WRITE_FAIL;

	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg) {
	return spLoggerPrintFormmatedString(msg);
}

/*
 * This method translate log level enum to its name as a String
 * assumption - logType not null
 * @param logType - the enum item to be translated
 */
const char* getLoggerNameFromType(enum sp_logger_level_t logType) {
	switch(logType) {
		case SP_LOGGER_ERROR_LEVEL:
			return ERROR_MSG;
		case SP_LOGGER_WARNING_ERROR_LEVEL:
			return WARNING_MSG;
		case SP_LOGGER_INFO_WARNING_ERROR_LEVEL:
			return INFO_MSG;
		case SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL:
			return DEBUG_MSG;
		default:
			return NULL;
	}
}


/*
 * This method gets log level enum and returns true iff it should write the log to the file
 * assumption - logger is not null, enum sp_logger_level_t is ordered according to write priviliges
 * @param logType - the enum item to be verified
 */
bool verifyWritePrivileges(enum sp_logger_level_t logType) {
	return (logType <= logger->level);
}



/**
 * 	Prints general message by type. as the following format:
 * 	---TYPE---
 * 	- file: <file>
 *  - function: <function>
 *  - line: <line>
 *  - message: <msg>
 *
 * 	<file> 	   - is the string given by file, it represents the file in which
 * 		   		 the warning print call occurred.
 * 	<function> - is the string given by function, it represents the function in which
 * 			   	 the warning print call occurred.
 * 	<line> 	   - is the string given by line, it represents the line in which
 * 		   		 the warning print call occurred
 * 	<msg> 	   - is the string given by msg, it contains the msg given by the user
 *
 *	Warning messages will be printed at levels:
 *
 *	SP_LOGGER_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_INFO_WARNING_ERROR_LEVEL,
 *	SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL
 *
 *	A new line will be printed after the print call.
 * @param logType - the TYPE of the message
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the spLoggerPrintWarning call occurred
 * @return
 * SP_LOGGER_UNDIFINED 			- If the logger is undefined
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If write failure occurred
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG spLoggerPrint(enum sp_logger_level_t logType, const char* msg, const char* file,
		const char* function, const int line) {
	if (!logger)
		return SP_LOGGER_UNDIFINED;
	if (!msg || !file || !function || line < 0) // TODO - check if we need to check the args first or the privilege
		return SP_LOGGER_INVAlID_ARGUMENT;
	if (!verifyWritePrivileges(logType))
		return SP_LOGGER_SUCCESS;
	return spLoggerPrintFormmatedString(GENERAL_MESSAGE_SKELETON,getLoggerNameFromType(logType), file,function,line,msg);
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrint(SP_LOGGER_ERROR_LEVEL, msg,file,function,line);
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrint(SP_LOGGER_WARNING_ERROR_LEVEL, msg,file,function,line);
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg) {
	if (!logger)
		return SP_LOGGER_UNDIFINED;
	if (!msg)
		return SP_LOGGER_INVAlID_ARGUMENT;
	if (!verifyWritePrivileges(SP_LOGGER_INFO_WARNING_ERROR_LEVEL))
		return SP_LOGGER_SUCCESS;
	return spLoggerPrintFormmatedString(SHORT_MESSAGE_SKELETON,INFO_MSG,msg);
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line) {
	return spLoggerPrint(SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL, msg,file,function,line);
}
