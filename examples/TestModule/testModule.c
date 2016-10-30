#include "testModule.h"

#if 0
time_t curtime()
{
	struct timeval tv;
	gettimeofday( &tv, NULL );
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif


#define FLAG_INPUT_FILE		 0x0001
#define FLAG_UNITEST_NUM	 0x0002        /* interactive mode */
#define FLAG_TEST_ALL		 0x0004
#define FLAG_OUTPUT_FILE	 0x0020

enum
{
	PASS = 0,
	FAIL,
};


int testMuImageBasic()
{
	muImage_t *temp;
	temp = muCreateImage(muSize(100,100), MU_IMG_DEPTH_8U, 1);
	muReleaseImage(&temp);

	return PASS;
}



void help()
{
	printf(
"Usage: testModule [OPTION] [INPUT]\n"
"\t-i filename     set input filename\n"
"\t-h              help menu\n"
"\t-a              test all function\n"
"\t-u              unit test number\n"
"\t-o filename     set output filename (doesn't do anything)\n"
	);

	printf(
"Usage: -u test index list:\n"
"\t1. muImage Basic Operating Test\n"
"\t2. BMP Operating Test\n"
"\t3. Dynamic Structures Test\n"
"\t4. muImage region Operating Test\n"
"\t5. muDrawRectangle Test\n"
"\t6. muRGB2HSV Test\n"
	);
}


int main(int argc, char *argv[])
{
	char *inputFile, *outputFile;
	int idx, flags = 0, status;

	if(argc < 2)
	{
		help();
	}


	while(1)
	{
		int c = getopt(argc, argv, "-i:hau:o:");
		if(c == -1) break;

		switch(c) 
		{
			case 'i': flags |= FLAG_INPUT_FILE;
				inputFile = optarg;
				break;
			case 'u': flags |= FLAG_UNITEST_NUM;
				idx = atoi(optarg);
				break;
			case 'a': flags |= FLAG_TEST_ALL; 
				break;
			case 'o': flags |= FLAG_OUTPUT_FILE;
				outputFile = optarg;
				break;
			case 'h': help(); 
				exit(0);
			default:
				break;
		}
	}	

	loggerInit((LOG_MODE_CONSOLE | LOG_MODE_FILE),"testModule", LOG_LEVEL_DONTCARE);

	if(flags & FLAG_UNITEST_NUM)
	{
		switch (idx)
		{
			case 1:
				logInfo("muImage basic operiting\n");
				status = testMuImageBasic();
				if(status)
				{
					logInfo("Failed\n");
				}
				break;
			case 5:
				logInfo("muDrawRectangle test\n");
				status = testDrawRectangle();
				if(status)
				{
					logInfo("Failed\n");
				}
				break;
			case 6:
				logInfo("muRGB2HSV test\n");
				if(flags & FLAG_INPUT_FILE)
				{
					status = testRGB2HSV(inputFile);
					if(status)
					{
						logInfo("Failed\n");
					}
					else
					{
						logInfo("Passed\n");
					}
				}
				else
				{
					logError("muRGB2HSV must give a input file testModue.exe -i test.bmp -n 6\n");
				}
				break;
			default:
				break;
		}
	}

	loggerExit();
	return 0;

}
