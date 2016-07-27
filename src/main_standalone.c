
#include <string.h>

#include "netx_io_areas.h"
#include "rdy_run.h"
#include "skweek.h"
#include "systime.h"

/*-------------------------------------------------------------------------*/


static void init_mmios(void)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);


	/* Set MMIO4 to PIO. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[4] = MMIO_CFG_PIO;

	/* Set MMIO5 to GPIO4. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[5] = MMIO_CFG_GPIO4;
}


#define XSTR(a) #a
#define STR(a) XSTR(a)

#define TUNE_START CONCAT(_binary, TUNE_NAME, bin_start)
#define TUNE_END CONCAT(_binary, TUNE_NAME, bin_end)

extern const unsigned char TUNE_START[];
extern const unsigned char TUNE_END[];


void skweek_standalone(void) __attribute__((noreturn));
void skweek_standalone(void)
{
	unsigned long ulTimer;
	int iElapsed;


	systime_init();

	/* Switch all LEDs off. */
	rdy_run_setLEDs(RDYRUN_OFF);

	init_mmios();

	while(1)
	{
		/* Play the tune. */
		skweek_play(TUNE_START, TUNE_END);

		/* Delay for 5 seconds. */
		ulTimer = systime_get_ms();
		do
		{
			iElapsed = systime_elapsed(ulTimer, 5000);
		} while( iElapsed==0 );
	}
}

