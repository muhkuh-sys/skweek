#ifndef __MAIN_TEST_H__
#define __MAIN_TEST_H__


typedef enum
{
	TEST_RESULT_OK = 0,
	TEST_RESULT_ERROR = 1
} TEST_RESULT_T;

typedef struct
{
	unsigned int ulReturnValue;
	void *pvInitParams;
	void *pvReturnMessage;
} TEST_PARAMETER_T;


void skweek_main(TEST_PARAMETER_T *ptTestParam);


#endif  /* __MAIN_TEST_H__ */
