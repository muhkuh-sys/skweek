#include "skweek.h"

#include "netx_io_areas.h"
#if CFG_VERBOSE!=0
#       include "uprintf.h"
#endif

/*-------------------------------------------------------------------------*/

#if ASIC_TYP==ASIC_TYP_NETX4000_RELAXED
static void note(unsigned long ulNoteTicks, unsigned long ulDurationTicks)
{
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;


	/* Stop both timers. */
	ptGpioArea->aulGpio_counter_ctrl[0] = 0;
	ptGpioArea->aulGpio_counter_cnt[0] = 0;
	ptGpioArea->aulGpio_counter_ctrl[1] = 0;
	ptGpioArea->aulGpio_counter_cnt[1] = 0;

	/* Set GPIO0 to PWM mode. */
	ulValue  = 7U << HOSTSRT(gpio_cfg0_mode);
	ulValue |= 1U << HOSTSRT(gpio_cfg0_count_ref);
	ptGpioArea->aulGpio_cfg[0] = ulValue;

	/* Set the threshold. */
	ptGpioArea->aulGpio_tc[0] = ulNoteTicks / 2U;

	/* Setup timer 0 with the duration of the note. */
	ptGpioArea->aulGpio_counter_max[0] = ulDurationTicks;
	ulValue  = HOSTMSK(gpio_counter0_ctrl_run);
	ulValue |= HOSTMSK(gpio_counter0_ctrl_once);
	ptGpioArea->aulGpio_counter_ctrl[0] = ulValue;

	/* Setup timer 1 with the frequency. */
	ptGpioArea->aulGpio_counter_max[1] = ulNoteTicks;
	ulValue  = HOSTMSK(gpio_counter1_ctrl_run);
	ulValue |= 0U << HOSTSRT(gpio_counter1_ctrl_gpio_ref);
	ptGpioArea->aulGpio_counter_ctrl[1] = ulValue;

	/* Wait until timer 0 stopped. */
	do
	{
		ulValue  = ptGpioArea->aulGpio_counter_ctrl[0];
		ulValue &= HOSTMSK(gpio_counter0_ctrl_run);
	} while( ulValue!=0 );

	/* Stop playing the note. */
	ptGpioArea->aulGpio_counter_ctrl[1] = 0;
	ptGpioArea->aulGpio_cfg[0] = 0U;
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
#elif ASIC_TYP==ASIC_TYP_NETX90_MPW
static void note(unsigned long ulNoteTicks, unsigned long ulDurationTicks)
{
	HOSTDEF(ptArmTimerComArea);
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;


	/* Stop the GPIO timer. */
	ptGpioArea->ulGpio_counter0_ctrl = 0;
	ptGpioArea->ulGpio_counter0_cnt = 0;

	/* Set GPIO0 to PWM mode. */
	ulValue  = 7U << HOSTSRT(gpio_cfg0_mode);
	ulValue |= 1U << HOSTSRT(gpio_cfg0_count_ref);
	ptGpioArea->aulGpio_cfg[0] = ulValue;

	/* Set the threshold. */
	ptGpioArea->aulGpio_tc[0] = ulNoteTicks / 2U;

	/* Setup the ARM timer with the duration of the note. */
	ptArmTimerComArea->aulTimer_config_timer[0] = 1;
	ptArmTimerComArea->aulTimer_preload_timer[0] = ulDurationTicks;
	ptArmTimerComArea->aulTimer_config_timer[0] = 0;

	/* Setup the GPIO timer with the frequency. */
	ptGpioArea->ulGpio_counter0_max = ulNoteTicks;
	ulValue  = HOSTMSK(gpio_counter0_ctrl_run);
	ulValue |= 0U << HOSTSRT(gpio_counter0_ctrl_gpio_ref);
	ptGpioArea->ulGpio_counter0_ctrl = ulValue;

	/* Wait until the ARM timer stopped. */
	do
	{
		ulValue = ptArmTimerComArea->aulTimer_timer[0];
	} while( ulValue!=0 );

	/* Stop playing the note. */
	ptGpioArea->ulGpio_counter0_ctrl = 0;
	ptGpioArea->aulGpio_cfg[0] = 0U;
}



static void pause(unsigned long ulDurationTicks)
{
	HOSTDEF(ptArmTimerComArea);
	unsigned long ulValue;


	/* Stop the timer. */
	ptArmTimerComArea->aulTimer_config_timer[0] = 1;
	ptArmTimerComArea->aulTimer_preload_timer[0] = ulDurationTicks;

	/* Start timer 0. */
	ptArmTimerComArea->aulTimer_config_timer[0] = 0;

	/* Wait until timer 0 stopped. */
	do
	{
		ulValue = ptArmTimerComArea->aulTimer_timer[0];
	} while( ulValue!=0 );
}
#endif


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



int skweek_play(const unsigned char *pucStart, const unsigned char *pucEnd)
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
#if CFG_VERBOSE!=0
				uprintf("Not enough data left for the 'note' command.\n");
#endif
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

#if CFG_VERBOSE!=0
				uprintf("Play note %08x %08x\n", ulNoteTicks, ulDurationTicks);
#endif

				note(ulNoteTicks, ulDurationTicks);
			}
		}
		else if( ucCmd=='P' )
		{
			/* The "pause" command needs at least 4 more bytes. */
			if( (pucCnt+4U)>pucEnd )
			{
#if CFG_VERBOSE!=0
				uprintf("Not enough data left for the 'pause' command.\n");
#endif
				iResult = -1;
				break;
			}
			else
			{
				/* Get the duration in ticks. */
				ulDurationTicks = get_u32(pucCnt);
				pucCnt += 4U;

#if CFG_VERBOSE!=0
				uprintf("Pause %08x\n", ulDurationTicks);
#endif

				pause(ulDurationTicks);
			}
		}
		else if( ucCmd=='B' )
		{
#if CFG_VERBOSE!=0
			uprintf("Start block\n");
#endif

			/* The "block" command needs at least 4 more bytes. */
			if( (pucCnt+4U)>pucEnd )
			{
#if CFG_VERBOSE!=0
				uprintf("Not enough data left for the 'block' command.\n");
#endif
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
#if CFG_VERBOSE!=0
					uprintf("Reached the top of the stack!\n");
#endif
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
#if CFG_VERBOSE!=0
			uprintf("End block\n");
#endif

			if( uiStackPointer==0 )
			{
#if CFG_VERBOSE!=0
				uprintf("End of block found with an empty stack.\n");
#endif
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
#if CFG_VERBOSE!=0
			uprintf("Unknown command: 0x%02x\n", ucCmd);
#endif
			iResult = -1;
			break;
		}
	}

	return iResult;
}
