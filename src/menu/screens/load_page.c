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
#include "machine_com.h"
#include "r_flash_loader_rx_if.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 2

static mn_widget_t btn_sim = {.name = "b0", .selectable = true};
static mn_widget_t btn_nao = {.name =  "b1", .selectable = true};
static mn_widget_t load_bar = {.name =  "j0", .selectable = false};
static mn_widget_t txt_info = {.name =  "p0", .selectable = false};
static mn_widget_t txt_version = {.name =  "t0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_sim,&btn_nao
};

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */


#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t load_page = {.id = SC_PAGE11,
					.wt_selected = 0,
					.name        = "Load",
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
	char str[20];
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		get_fileName(str);
		changeTxt(&txt_version,str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sim.id, EVENT_CLICK))
	{
		uint8_t progress_val;
		widgetVisible(&btn_sim, NT_HIDE);
		widgetVisible(&btn_nao, NT_HIDE);
		widgetVisible(&txt_info, NT_HIDE);
		widgetVisible(&load_bar, NT_SHOW);
		while((progress_val = R_loader_progress()) < 16)
		{
			widgetProgressBar(&load_bar,(progress_val*100)/16);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_nao.id, EVENT_CLICK))
	{
		mn_screen_change(&splash_page,EVENT_SHOW);
	}
}
