
#include <string.h>

#include "netx_io_areas.h"
#include "rdy_run.h"
#include "skweek.h"
#include "systime.h"

/*-------------------------------------------------------------------------*/


#define XSTR(a) #a
#define STR(a) XSTR(a)

#define TUNE_START CONCAT(_binary, TUNE_NAME, bin_start)
#define TUNE_END CONCAT(_binary, TUNE_NAME, bin_end)

extern const unsigned char TUNE_START[];
extern const unsigned char TUNE_END[];


void skweek_standalone(void);
void skweek_standalone(void)
{
	/* Play the tune. */
	skweek_play(TUNE_START, TUNE_END);
}

