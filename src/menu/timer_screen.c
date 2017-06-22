/** @file screen.c
 *  @brief Function implementation for the screen.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

/* Includes */
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "platform.h"
#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"

/* Defines */

/* Static functions */
static void vTimerScreenCallback( TimerHandle_t xTimer  );

/* Static variables and const */

/* Global variables and const */

/* extern variables */

/************************** Static functions *********************************************/

/************************** Public functions *********************************************/
mn_timer_err_t mn_screen_create_timer (mn_timer_t *p_timer,uint32_t period)
{
	mn_timer_err_t err = MN_TIMER_ERROR;
	if (p_timer->hdl == NULL)
	{
		p_timer->hdl = xTimerCreate(p_timer->name, period,pdTRUE, (void *) p_timer->id, vTimerScreenCallback);
		err = MN_TIMER_SUCCESS;
	}
	else
	{
		err = MN_TIMER_ALREADY_CREATED;
	}
	return err;
}

mn_timer_err_t mn_screen_delete_timer (mn_timer_t *p_timer)
{
	mn_timer_err_t err = MN_TIMER_ERROR;
	if (p_timer->hdl != NULL)
	{
		if(xTimerDelete(p_timer->hdl, 0))
		{
			err = MN_TIMER_SUCCESS;
		}
	}
	else
	{
		err = MN_TIMER_ERROR;
	}
	return err;
}

mn_timer_err_t mn_screen_start_timer (mn_timer_t *p_timer)
{

	mn_timer_err_t err = MN_TIMER_ERROR;
	if (p_timer->hdl != NULL)
	{
		if(xTimerStart(p_timer->hdl, 0))
		{
			err = MN_TIMER_SUCCESS;
		}
		else
		{
			err = MN_TIMER_ALREADY_STARTED;
		}
	}
	else
	{
		err = MN_TIMER_ERROR;
	}
	return err;
}

mn_timer_err_t mn_screen_stop_timer (mn_timer_t *p_timer)
{

	mn_timer_err_t err = MN_TIMER_ERROR;
	if (p_timer->hdl != NULL)
	{
		if(xTimerStop(p_timer->hdl, 0))
		{
			err = MN_TIMER_SUCCESS;
		}
		else
		{
			err = MN_TIMER_ALREADY_STOPPED;
		}
	}
	else
	{
		err = MN_TIMER_ERROR;
	}
	return err;
}

static void vTimerScreenCallback( TimerHandle_t xTimer  )
{
	mn_screen_event_t timer;
	timer.event = ( uint32_t ) pvTimerGetTimerID( xTimer );
	timer.event = EVENT_SIGNAL(timer.event,EVENT_TIMER);
	xQueueSend( menu.qEvent, &timer, 0 );
	//page->iif_func[SC_HANDLER](&timer);
}
