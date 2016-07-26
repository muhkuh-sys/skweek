
#include <string.h>

#include "main.h"
#include "netx_io_areas.h"
#include "rdy_run.h"
#include "skweek.h"
#include "systime.h"

#include "uprintf.h"
#include "version.h"

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

extern const unsigned char CONCAT(_binary, TUNE_NAME, bin_start)[];
extern const unsigned char CONCAT(_binary, TUNE_NAME, bin_end)[];



void skweek_main(TEST_PARAMETER_T *ptTestParam)
{
	systime_init();

	uprintf("\f. *** Skweek by doc_bacardi@users.sourceforge.net ***\n");
	uprintf("V" VERSION_ALL "\n\n");

	uprintf("Playing " STR(TUNE_NAME) "...\n");

	/* Switch all LEDs off. */
	rdy_run_setLEDs(RDYRUN_OFF);

	init_mmios();

	/* Play the tune. */
	skweek_play(_binary_simpsons_theme_bin_start, _binary_simpsons_theme_bin_end);

	/* Set the result to "OK". */
	ptTestParam->ulReturnValue = TEST_RESULT_OK;
}

