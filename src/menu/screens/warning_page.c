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
		switch (p_warning->buttonUseInit)
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
		p_warning->func_callback(BTN_PRESSED_OK);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sim.id, EVENT_CLICK))
	{
		p_warning->func_callback(BTN_PRESSED_SIM);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_nao.id, EVENT_CLICK))
	{
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
//	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
//	{
//		mn_screen_change(&emergencia_page);
//	}
}
