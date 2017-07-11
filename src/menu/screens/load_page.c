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
#include "load_page.h"

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
#include "spiffs_hw.h"
/* Defines */

#define TIMER_NUM 1

#define WIDGET_NUM 2

#define TIMER_PROGRESS 210

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
static void format_mem_task(void);
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_timer_t timer0 = {.id = TIMER_PROGRESS, .name = "tpro"};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
uint8_t loadfilesNum = 0;

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
static void format_mem_task(void)
{
	spiffs_format();
	RESET;
	while(1)
	{

	}
}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
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
	static uint8_t progress_val;
	bool ret;
	char str[20];
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		if ((loadfilesNum & MEM_FORMAT) == MEM_FORMAT)
		{
			widgetVisible(&btn_sim, NT_HIDE);
			widgetVisible(&btn_nao, NT_HIDE);
			widgetVisible(&txt_info, NT_HIDE);
			widgetVisible(&load_bar, NT_SHOW);
			snprintf(str,sizeof(str), "Formatando mem ext");
		    xTaskCreate( (pdTASK_CODE)format_mem_task,     "format_mem_task    ",  256, NULL, 1, NULL); /* keyboard_task      */
			mn_screen_create_timer(&timer0,200);
			mn_screen_start_timer(&timer0);
		}
		else if ((loadfilesNum & MCU_FILE) == MCU_FILE)
		{
			get_fileName(str);
		}
		changeTxt(&txt_version,str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sim.id, EVENT_CLICK))
	{

		if ((loadfilesNum & MCU_FILE) == MCU_FILE)
		{
			widgetVisible(&btn_sim, NT_HIDE);
			widgetVisible(&btn_nao, NT_HIDE);
			widgetVisible(&txt_info, NT_HIDE);
			widgetVisible(&load_bar, NT_SHOW);
			while((progress_val = R_loader_progress()) < 16)
			{
				widgetProgressBar(&load_bar,(progress_val*100)/16);
			}
		}
		if ((loadfilesNum & NEXTION_FILE) == NEXTION_FILE)
		{
			//ret = NexUpload_checkFile("MT01_5in_regular.tft");
			//if(ret)
			//{
				ret = NexUpload_setDownloadBaudrate(1500000);
				//if (ret)
				//{
					NexUpload_downloadTftFile("MT01_5in_regular.tft");
					ret = NexUpload_waitingReset(10000);
				//}
			//}
		}
		RESET
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_nao.id, EVENT_CLICK))
	{
		mn_screen_change(&splash_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(timer0.id,EVENT_TIMER))
	{
		if(progress_val <= 10)
		{
			widgetProgressBar(&load_bar,(progress_val*100)/10);
			progress_val++;
		}
		else
		{
			progress_val = 0;
		}
	}
}
