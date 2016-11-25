#ifndef __MAIN_TEST_H__
#define __MAIN_TEST_H__


typedef enum TEST_RESULT_ENUM
{
	TEST_RESULT_OK = 0,
	TEST_RESULT_ERROR = 1
} TEST_RESULT_T;

typedef struct TEST_PARAMETER_STRUCT
{
	unsigned int ulReturnValue;
	void *pvInitParams;
	void *pvReturnMessage;
} TEST_PARAMETER_T;

typedef struct SKWEEK_PARAMETER_STRUCT
{
	const unsigned char *pucTuneStart;
	unsigned long ulTuneSizeInBytes;
} SKWEEK_PARAMETER_T;



void skweek_main(TEST_PARAMETER_T *ptTestParam);


#endif  /* __MAIN_TEST_H__ */
