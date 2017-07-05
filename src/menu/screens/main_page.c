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
#include "main_page.h"
#include "spiffs.h"
#include "tinyg.h"
#include "xio.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 6

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_loadfile = {.name = "b1", .selectable = true};
static mn_widget_t btn_auto = {.name = "b0", .selectable = true};
static mn_widget_t btn_manual = {.name = "b2", .selectable = true};
static mn_widget_t btn_confCorte = {.name = "b3", .selectable = true};
static mn_widget_t btn_confMaq = {.name = "b4", .selectable = true};

static mn_widget_t filename_txt = {.name = "t0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_loadfile,&btn_manual,&btn_auto,&btn_confCorte,&btn_confMaq,&filename_txt
};


#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t main_page = {.id 		 = SC_PAGE1,
					.wt_selected = 0,
					.name        = "main",
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
		mn_screen_stop_timer(main_page.p_timer[i]);
#endif
}

void page_handler (void *p_arg)
{
	spiffs_DIR sf_dir;
	spiffs_stat fileStat;
	struct spiffs_dirent e;
	struct spiffs_dirent *pe = &e;
	spiffs_file *fd = &uspiffs[0].f;
	spiffs *fs = &uspiffs[0].gSPIFFS;
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		SPIFFS_opendir(fs, "/", &sf_dir);
		pe = SPIFFS_readdir(&sf_dir, pe);
		if(pe != NULL)
		{
			changeTxt(&filename_txt,pe->name);
			SPIFFS_closedir(&sf_dir);
		}

	}
	if (p_page_hdl->event == EVENT_SHOW)
	{

	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_loadfile.id,EVENT_CLICK))
	{
		mn_screen_change(&selFiles_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_manual.id,EVENT_CLICK))
	{
		mn_screen_change(&manual_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_auto.id,EVENT_CLICK))
	{
		mn_screen_change(&auto_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_confCorte.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgCorte_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_confMaq.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgMaq_page,EVENT_SHOW);
	}
}
