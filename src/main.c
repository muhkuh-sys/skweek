
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


	/* Set MMIO4 to PIO. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[4] = MMIO_CFG_PIO;

	/* Set MMIO5 to GPIO4. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
	ptMmioCtrlArea->aulMmio_cfg[5] = MMIO_CFG_GPIO4;
}



static void note(unsigned long ulNoteTicks, unsigned long ulDurationTicks)
{
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;


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
	ptGpioArea->aulGpio_cfg[4] = 0U;
}



static void pause(unsigned long ulDurationTicks)
{
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;


	/* Stop the timer. */
	ptGpioArea->aulGpio_counter_ctrl[0] = 0;
	ptGpioArea->aulGpio_counter_cnt[0] = 0;

	/* Setup timer 0 with the duration of the pause. */
	ptGpioArea->aulGpio_counter_max[0] = ulDurationTicks;
	ulValue  = HOSTMSK(gpio_counter0_ctrl_run);
	ulValue |= HOSTMSK(gpio_counter0_ctrl_once);
	ptGpioArea->aulGpio_counter_ctrl[0] = ulValue;

	/* Wait until timer 0 stopped. */
	do
	{
		ulValue  = ptGpioArea->aulGpio_counter_ctrl[0];
		ulValue &= HOSTMSK(gpio_counter0_ctrl_run);
	} while( ulValue!=0 );
}



static unsigned long get_u32(const unsigned char *pucCnt)
{
	unsigned long ulValue;


	ulValue  =                 pucCnt[0];
	ulValue |= (unsigned long)(pucCnt[1] <<  8U);
	ulValue |= (unsigned long)(pucCnt[2] << 16U);
	ulValue |= (unsigned long)(pucCnt[3] << 24U);

	return ulValue;
}



#define MAX_STACK_DEPTH 16

typedef struct SKWEEK_STACK_STRUCT
{
	const unsigned char *pucBlockStart;
	unsigned long ulRepeat;
} SKWEEK_STACK_T;



static int play_skweek(const unsigned char *pucStart, const unsigned char *pucEnd)
{
	int iResult;
	const unsigned char *pucCnt;
	unsigned char ucCmd;
	unsigned long ulNoteTicks;
	unsigned long ulDurationTicks;
	unsigned long ulRepeat;
	SKWEEK_STACK_T atStack[MAX_STACK_DEPTH];
	unsigned int uiStackPointer;
	SKWEEK_STACK_T *ptStackElement;


	/* Be optimistic. */
	iResult = 0;

	/* The stack is empty. */
	uiStackPointer = 0U;

	/* Loop over the complete file. */
	pucCnt = pucStart;
	while( pucCnt<pucEnd )
	{
		/* Get the next command. */
		ucCmd = *(pucCnt++);
		if( ucCmd=='N' )
		{
			/* The "note" command needs at least 8 more bytes. */
			if( (pucCnt+8U)>pucEnd )
			{
				uprintf("Not enough data left for the 'note' command.\n");
				iResult = -1;
				break;
			}
			else
			{
				/* Get the note and the duration in ticks. */
				ulNoteTicks = get_u32(pucCnt);
				pucCnt += 4U;
				ulDurationTicks = get_u32(pucCnt);
				pucCnt += 4U;

				uprintf("Play note %08x %08x\n", ulNoteTicks, ulDurationTicks);

//				note(ulNoteTicks, ulDurationTicks);
			}
		}
		else if( ucCmd=='P' )
		{
			/* The "pause" command needs at least 4 more bytes. */
			if( (pucCnt+4U)>pucEnd )
			{
				uprintf("Not enough data left for the 'pause' command.\n");
				iResult = -1;
				break;
			}
			else
			{
				/* Get the duration in ticks. */
				ulDurationTicks = get_u32(pucCnt);
				pucCnt += 4U;

				uprintf("Pause %08x\n", ulDurationTicks);

				pause(ulDurationTicks);
			}
		}
		else if( ucCmd=='B' )
		{
			uprintf("Start block\n");

			/* The "block" command needs at least 4 more bytes. */
			if( (pucCnt+4U)>pucEnd )
			{
				uprintf("Not enough data left for the 'block' command.\n");
				iResult = -1;
				break;
			}
			else
			{
				/* Get the repeat counter. */
				ulRepeat = get_u32(pucCnt);
				pucCnt += 4U;

				if( uiStackPointer>=MAX_STACK_DEPTH )
				{
					uprintf("Reached the top of the stack!\n");
					iResult = -1;
					break;
				}
				else
				{
					ptStackElement = atStack + uiStackPointer;
					ptStackElement->ulRepeat = ulRepeat;
					ptStackElement->pucBlockStart = pucCnt;

					++uiStackPointer;
				}
			}
		}
		else if( ucCmd=='E' )
		{
			uprintf("End block\n");

			if( uiStackPointer==0 )
			{
				uprintf("End of block found with an empty stack.\n");
				iResult = -1;
				break;
			}
			else
			{
				ptStackElement = atStack + uiStackPointer - 1U;
				if( ptStackElement->ulRepeat==0 )
				{
					/* No more repeats left. Continue. */
					--uiStackPointer;
				}
				else
				{
					ptStackElement->ulRepeat -= 1U;
					pucCnt = ptStackElement->pucBlockStart;
				}
			}
		}
		else
		{
			uprintf("Unknown command: 0x%02x\n", ucCmd);
			iResult = -1;
			break;
		}
	}

	return iResult;
}


extern const unsigned char _binary_simpsons_theme_bin_start[];
extern const unsigned char _binary_simpsons_theme_bin_end[];



void skweek_main(TEST_PARAMETER_T *ptTestParam)
{
	systime_init();

	uprintf("\f. *** Skweek by doc_bacardi@users.sourceforge.net ***\n");
	uprintf("V" VERSION_ALL "\n\n");

	/* Switch all LEDs off. */
	rdy_run_setLEDs(RDYRUN_OFF);

//	init_mmios();

	/* Play the tune. */
	play_skweek(_binary_simpsons_theme_bin_start, _binary_simpsons_theme_bin_end);

	/* Set the result to "OK". */
	ptTestParam->ulReturnValue = TEST_RESULT_OK;
}

