


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

