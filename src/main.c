
#include <string.h>

#include "main.h"
#include "netx_io_areas.h"
#include "rdy_run.h"
#include "systime.h"

#include "uprintf.h"
#include "version.h"

/*-------------------------------------------------------------------------*/


static void init_mmios(void)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);


	/* Set MMIO4 to GPIO4. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[4] = MMIO_CFG_GPIO4;
}



static void play_note(void)
{
	HOSTDEF(ptGpioArea);
	unsigned long ulDurationTicks;
	unsigned long ulNoteTicks;
	unsigned long ulValue;


	/* Play some note. */
	ulDurationTicks = 0x042c1d80;
	ulNoteTicks = 0x00045e88;

	/* Stop both timers. */
	ptGpioArea->aulGpio_counter_ctrl[0] = 0;
	ptGpioArea->aulGpio_counter_cnt[0] = 0;
	ptGpioArea->aulGpio_counter_ctrl[1] = 0;
	ptGpioArea->aulGpio_counter_cnt[1] = 0;

	/* Set GPIO4 to PWM mode. */
	ulValue  = 7U << HOSTSRT(gpio_cfg4_mode);
	ulValue |= 1U << HOSTSRT(gpio_cfg4_count_ref);
	ptGpioArea->aulGpio_cfg[4] = ulValue;

	/* Set the threshold. */
	ptGpioArea->aulGpio_tc[4] = ulNoteTicks / 2U;

	/* Setup timer 0 with the duration of the note. */
	ptGpioArea->aulGpio_counter_max[0] = ulDurationTicks;
	ulValue  = HOSTMSK(gpio_counter0_ctrl_run);
	ulValue |= HOSTMSK(gpio_counter0_ctrl_once);
	ptGpioArea->aulGpio_counter_ctrl[0] = ulValue;

	/* Setup timer 1 with the frequency. */
	ptGpioArea->aulGpio_counter_max[1] = ulNoteTicks;
	ulValue  = HOSTMSK(gpio_counter1_ctrl_run);
	ulValue |= 4U << HOSTSRT(gpio_counter1_ctrl_gpio_ref);
	ptGpioArea->aulGpio_counter_ctrl[1] = ulValue;

	/* Wait until timer 0 stopped. */
	do
	{
		ulValue  = ptGpioArea->aulGpio_counter_ctrl[0];
		ulValue &= HOSTMSK(gpio_counter0_ctrl_run);
	} while( ulValue!=0 );

	/* Stop playing the note. */
	ptGpioArea->aulGpio_counter_ctrl[1] = 0;
}



void skweek_main(TEST_PARAMETER_T *ptTestParam)
{
	systime_init();

	uprintf("\f. *** Skweek by doc_bacardi@users.sourceforge.net ***\n");
	uprintf("V" VERSION_ALL "\n\n");

	/* Switch all LEDs off. */
	rdy_run_setLEDs(RDYRUN_OFF);

	init_mmios();

	play_note();

	/* Set the result to "OK". */
	ptTestParam->ulReturnValue = TEST_RESULT_OK;
}

