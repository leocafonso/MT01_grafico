/** @file screen.c
 *  @brief Function implementation for the screen.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* Includes */
#include "platform.h"
#include "machine_com.h"
#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"
#include "warning_page.h"
#include "spiffs.h"
#include "tinyg.h"
#include "xio.h"
/* Defines */

#define TIMER_NUM 1

#define WIDGET_NUM 4

#define TIMER_WARNING 200

/* Static functions */
static void warning_key_right (void *p_arg);
static void warning_key_left (void *p_arg);
static void warning_key_enter (void *p_arg);
static void warning_key_release (void *p_arg);

static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_sim = {.name = "b0", .selectable = true};
static mn_widget_t btn_ok = {.name = "b1", .selectable = true};
static mn_widget_t btn_nao = {.name =  "b2", .selectable = true};
static mn_widget_t msg_pic = {.name =  "p0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_sim,&btn_ok,&btn_nao,&msg_pic
};
static mn_screen_event_t warning;
static mn_timer_t timer_warning = {.id = TIMER_WARNING, .name = "twar"};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer_warning};
#endif
/* Global variables and const */
mn_screen_t warning_page = {.id 		 = SC_PAGE1,
					.wt_selected = 0,
					.name        = "warning",
					.p_widget = p_widget,
#if (TIMER_NUM > 0)
					.p_timer = p_timer,
#else
					.p_timer = NULL,
#endif
					.widgetSize = WIDGET_NUM,
					.iif_func 	 = {	[SC_HANDLER] = page_handler,
										[SC_ATTACH] = page_attach,
										[SC_DETACH] = page_detach
									}};
/* extern variables */

/************************** Static functions *********************************************/
static void warning_key_right (void *p_arg)
{
	if (warning_page.wt_selected == 0)
	{
		widgetSelRec(warning_page.p_widget[0], 3, DESELECT_COLOR);
		warning_page.wt_selected = 2;
		widgetSelRec(warning_page.p_widget[2], 3, SELECT_COLOR);
	}
}
static void warning_key_left (void *p_arg)
{
	if (warning_page.wt_selected == 2)
	{
		widgetSelRec(warning_page.p_widget[2], 3, DESELECT_COLOR);
		warning_page.wt_selected = 0;
		widgetSelRec(warning_page.p_widget[0], 3, SELECT_COLOR);
	}
}
static void warning_key_enter (void *p_arg)
{
	if (warning_page.wt_selected == 0)
	{
		widgetClick(&btn_sim, NT_PRESS);
	}
	else if (warning_page.wt_selected == 1)
	{
		widgetClick(&btn_ok, NT_PRESS);
	}
	else if (warning_page.wt_selected == 2)
	{
		widgetClick(&btn_nao, NT_PRESS);
	}
}
static void warning_key_release (void *p_arg)
{
	if (btn_sim.click == NT_PRESS)
	{
		widgetClick(&btn_sim, NT_RELEASE);
		warning.event = EVENT_SIGNAL(btn_sim.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &warning, 0 );
	}
	else if (btn_ok.click == NT_PRESS)
	{
		widgetClick(&btn_ok, NT_RELEASE);
		warning.event = EVENT_SIGNAL(btn_ok.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &warning, 0 );
	}
	else if (btn_nao.click == NT_PRESS)
	{
		widgetClick(&btn_nao, NT_RELEASE);
		warning.event = EVENT_SIGNAL(btn_nao.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &warning, 0 );
	}

}

static void warning_key_keyborad_bind (uint8_t buttonUsage)
{
	switch (buttonUsage)
	{
		case BTN_OK:
			warning_page.wt_selected = 1;
			for (uint8_t i = SC_KEY_ENTER; i < SC_KEY_TOTAL; i++)
			{
				switch (i)
				{
					case SC_KEY_ENTER: warning_page.iif_func[i] = &warning_key_enter; 	break;
					case SC_KEY_RELEASE: warning_page.iif_func[i] = &warning_key_release; 	break;
					default: warning_page.iif_func[i] = &mn_screen_idle;
				}
			}
			break;
		case BTN_ASK:
			warning_page.wt_selected = 2;
			for (uint8_t i = SC_KEY_ENTER; i < SC_KEY_TOTAL; i++)
			{
				switch (i)
				{
					case SC_KEY_RIGHT: warning_page.iif_func[i] = &warning_key_right; 	break;
					case SC_KEY_LEFT:  warning_page.iif_func[i] = &warning_key_left; 	break;
					case SC_KEY_ENTER: warning_page.iif_func[i] = &warning_key_enter; 	break;
					case SC_KEY_RELEASE: warning_page.iif_func[i] = &warning_key_release; 	break;
					default: warning_page.iif_func[i] = &mn_screen_idle;
				}
			}
			break;
		case BTN_NO_USE:
			for (uint8_t i = SC_KEY_ENTER; i < SC_KEY_TOTAL; i++)
			{
				warning_page.iif_func[i] = &mn_screen_idle;
			}
			break;
	}
}

/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{

}

void page_detach (void *p_arg)
{
#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page->p_timer[i]);
#endif
}

void page_handler (void *p_arg)
{
	static uint8_t msg_counter = 0;
	mn_screen_event_t *p_page_hdl = p_arg;
	mn_warning_t *p_warning = p_page_hdl->p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		widgetVisible(&msg_pic, NT_SHOW);
		widgetChangePic(&msg_pic, p_warning->img_txt[0],NO_IMG);
		warning_key_keyborad_bind (p_warning->buttonUseInit);
		switch (p_warning->buttonUseInit)
		{
			case BTN_OK:
				widgetChangePic(&btn_ok, IMG_BTN_OK,IMG_BTN_OK_PRESS);
				widgetTouchable(&btn_sim, NT_DISABLE);
				widgetTouchable(&btn_nao, NT_DISABLE);
				widgetTouchable(&btn_ok, NT_ENABLE);
				widgetVisible(&btn_sim, NT_HIDE);
				widgetVisible(&btn_nao, NT_HIDE);
				widgetVisible(&btn_ok, NT_SHOW);
				break;
			case BTN_ASK:
				widgetTouchable(&btn_sim, NT_ENABLE);
				widgetTouchable(&btn_nao, NT_ENABLE);
				widgetTouchable(&btn_ok, NT_DISABLE);
				widgetVisible(&btn_sim, NT_SHOW);
				widgetVisible(&btn_nao, NT_SHOW);
				widgetVisible(&btn_ok, NT_HIDE);
				break;
			case BTN_NO_USE:
				widgetTouchable(&btn_sim, NT_DISABLE);
				widgetTouchable(&btn_nao, NT_DISABLE);
				widgetTouchable(&btn_ok, NT_DISABLE);
				widgetVisible(&btn_sim, NT_HIDE);
				widgetVisible(&btn_nao, NT_HIDE);
				widgetVisible(&btn_ok, NT_HIDE);
				mn_screen_create_timer(&timer_warning,p_warning->time_interval);
				mn_screen_start_timer(&timer_warning);
				break;

		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_ok.id, EVENT_CLICK))
	{
		//widgetVisible(&msg_pic, NT_HIDE);
		p_warning->func_callback(BTN_PRESSED_OK);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sim.id, EVENT_CLICK))
	{
		//widgetVisible(&msg_pic, NT_HIDE);
		p_warning->func_callback(BTN_PRESSED_SIM);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_nao.id, EVENT_CLICK))
	{
		//widgetVisible(&msg_pic, NT_HIDE);
		p_warning->func_callback(BTN_PRESSED_NAO);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(TIMER_WARNING, EVENT_TIMER))
	{
		msg_counter++;
		widgetChangePic(&msg_pic, p_warning->img_txt[msg_counter],NO_IMG);
		if(msg_counter == p_warning->msg_count - 1)
		{
			msg_counter = 0;
			mn_screen_stop_timer(&timer_warning);
			warning_key_keyborad_bind (p_warning->buttonUseEnd);
			switch (p_warning->buttonUseEnd)
			{
				case BTN_OK:
					widgetTouchable(&btn_sim, NT_DISABLE);
					widgetTouchable(&btn_nao, NT_DISABLE);
					widgetTouchable(&btn_ok, NT_ENABLE);
					widgetVisible(&btn_sim, NT_HIDE);
					widgetVisible(&btn_nao, NT_HIDE);
					widgetVisible(&btn_ok, NT_SHOW);
					break;
				case BTN_ASK:
					widgetTouchable(&btn_sim, NT_ENABLE);
					widgetTouchable(&btn_nao, NT_ENABLE);
					widgetTouchable(&btn_ok, NT_DISABLE);
					widgetVisible(&btn_sim, NT_SHOW);
					widgetVisible(&btn_nao, NT_SHOW);
					widgetVisible(&btn_ok, NT_HIDE);
					break;
			}
		}
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}
