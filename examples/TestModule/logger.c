#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "logger.h"

static FILE *gFp = NULL;
static int gLevel = LOG_LEVEL_DONTCARE, gMode = LOG_MODE_CONSOLE;
static char* gLogName = NULL;

void loggerInit(int mode, char *logName, int setLevel)
{
	time_t now;
	char *buf;
	char timeName[100];
	struct tm *ltm;
	time(&now);
	ltm = localtime(&now);
	gLevel = setLevel;
	gLogName = logName;
	gMode = mode;
	if(mode & LOG_MODE_CONSOLE)
	{
		//fprintf(stdout, "logger console mode is enable\n");
	}
	if(mode & LOG_MODE_FILE)
	{
		FILE *fp;
		buf = (char*)malloc(256*sizeof(char));
		memset(buf,0, 256*sizeof(char));
		memset(timeName, 0, 100*sizeof(char));
		sprintf(timeName, "%d%02d%02d", (1900 + ltm->tm_year), (1 + ltm->tm_mon),  ltm->tm_mday);
		//fprintf(stdout, "logger file mode is enable\n");
		if(gFp == NULL)
		{
			sprintf(buf, "%s_%s.log", logName, timeName);
			fp = fopen(buf, "r");
			if(fp == NULL)
			{
				//First time to create data
				gFp = fopen(buf, "w");
				if(gFp == NULL)
				{
					fprintf(stderr, "logger file open failed\n");
					exit(0);
				}
				//fprintf(stdout, "logger file is created\n");
				
			}
			else
			{
				//File Exist
				gFp = fopen(buf, "a");
				if(gFp == NULL)
				{
					fprintf(stderr, "logger file open failed\n");
					fclose(fp);
					exit(0);
				}
				//fprintf(stdout, "logger file already exist, overwrite them\n");
				fclose(fp);
			}
		}
		free(buf);
	}

}



void logFormat(const char* level, const char* message, va_list args)
{   
	FILE *ftmp;
	struct tm *ltm;
	int logLevel;
	time_t now;
	time(&now);

	ltm = localtime(&now);
	if(!strcmp(level, "ERROR"))
	{
		ftmp = stderr;
		logLevel = LOG_LEVEL_ERROR;
 	}
	else if (!strcmp(level, "INFO"))
	{
		ftmp = stdout;
		logLevel = LOG_LEVEL_INFO;
	}
	else if(!strcmp(level, "DEBUG"))
	{
		ftmp = stdout;
		logLevel = LOG_LEVEL_DEBUG;
	}
	else
	{
		ftmp = stdout;
		logLevel = LOG_LEVEL_DONTCARE;
	}

	if(logLevel >= gLevel)
	{
		if(gMode & LOG_MODE_CONSOLE)
		{
			fprintf(ftmp, "%d-%02d-%02d %02d:%02d:%02d - %s - %s -", (1900+ltm->tm_year), ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec, gLogName, level);  
			vfprintf(ftmp, message, args);
		}
		if(gMode & LOG_MODE_FILE)
		{
			fprintf(gFp, "%d-%02d-%02d %02d:%02d:%02d - %s - %s -", (1900+ltm->tm_year), ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec, gLogName, level);  
			vfprintf(gFp, message, args);    
		}
	}
}

void logError(const char* message, ...) 
{  va_list args;   
   va_start(args, message);    
   logFormat("ERROR", message, args);    
   va_end(args); 
}

void logInfo(const char* message, ...) 
{   
	va_list args;   
	va_start(args, message);    
	logFormat("INFO", message, args);  
	va_end(args); 
}

void logDebug(const char* message, ...) 
{  
	va_list args;  
	va_start(args, message);    
	logFormat("DEBUG", message, args);     
	va_end(args); 
}

void loggerExit()
{
	if(gFp)
	{
		fclose(gFp);
	}
}

/*
void main()
{
	loggerInit((LOG_MODE_CONSOLE|LOG_MODE_FILE),"logTest", LOG_LEVEL_DONTCARE);
	logInfo("log info test\n");
	loggerExit();
}
*/
