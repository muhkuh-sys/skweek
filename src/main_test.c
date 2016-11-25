
#include <string.h>

#include "main_test.h"
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
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key; /* @suppress("Assignment to itself") */
	ptMmioCtrlArea->aulMmio_cfg[4] = MMIO_CFG_PIO;

	/* Set MMIO5 to GPIO4. */
	ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key; /* @suppress("Assignment to itself") */
	ptMmioCtrlArea->aulMmio_cfg[5] = MMIO_CFG_GPIO4;
}



void skweek_main(TEST_PARAMETER_T *ptTestParam)
{
	const SKWEEK_PARAMETER_T *ptParameter;
	const unsigned char *pucTuneStart;
	const unsigned char *pucTuneEnd;
	TEST_RESULT_T tResult;
	int iResult;


	/* Be optimistic. */
	tResult = TEST_RESULT_OK;

	systime_init();

	uprintf("\f. *** Skweek by doc_bacardi@users.sourceforge.net ***\n");
	uprintf("V" VERSION_ALL "\n\n");

	ptParameter = (const SKWEEK_PARAMETER_T*)(ptTestParam->pvInitParams);
	if( ptParameter==NULL )
	{
		uprintf("No tune to play, the init parameter is NULL!\n");
		tResult = TEST_RESULT_ERROR;
	}
	else
	{
		uprintf("Tune start: 0x%08x\n", (unsigned long)(ptParameter->pucTuneStart));
		uprintf("Tune size:  %d\n", ptParameter->ulTuneSizeInBytes);

		hexdump(ptParameter->pucTuneStart, ptParameter->ulTuneSizeInBytes);

		pucTuneStart = ptParameter->pucTuneStart;
		pucTuneEnd   = ptParameter->pucTuneStart + ptParameter->ulTuneSizeInBytes;

		/* Switch all LEDs off. */
		rdy_run_setLEDs(RDYRUN_OFF);

		init_mmios();

		/* Play the tune. */
		iResult = skweek_play(pucTuneStart, pucTuneEnd);
		if( iResult!=0 )
		{
			uprintf("Failed to play the tune!\n");
			tResult = TEST_RESULT_ERROR;
		}
		else
		{
			uprintf("Finished the tune.\n");
		}
	}

	/* Set the result to "OK". */
	ptTestParam->ulReturnValue = tResult;
}

