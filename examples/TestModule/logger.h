#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOG_MODE_CONSOLE	0x00000001
#define LOG_MODE_FILE		0x00000002
enum
{
	LOG_LEVEL_DONTCARE = 0,
	LOG_LEVEL_DEBUG = 10,
	LOG_LEVEL_INFO = 20,
	LOG_LEVEL_ERROR = 30
};

extern void loggerInit(int mode, char *logName, int setLevel);
extern void logError(const char* message, ...);
extern void logInfo(const char* message, ...);
extern void logDebug(const char* message, ...); 
extern void loggerExit();


#endif