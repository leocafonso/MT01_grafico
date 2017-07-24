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

#include "pages_includes.h"

#include "r_fl_globals.h"
#include "r_flash_loader_rx_if.h"
/* Defines */

#define TIMER_NUM 1

#define WIDGET_NUM 5

#define TIMER_SPLASH 201

#define TIME_SCREENS 3000

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

static void warning_callback(warn_btn_t btn_type);
static void warning_zeromaqinit_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t txt_version = {.name =  "t0", .selectable = false};
static mn_timer_t timer0 = {.id = TIMER_SPLASH, .name = "tspl"};
static mn_warning_t warn_args;
static mn_warning_t warn_zeromaq_args = { 	.buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_CONTINUAR,
											.msg_count = 1,
											.func_callback = warning_zeromaqinit_callback
										   };

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t splash_page = {.id = SC_PAGE0,
					.wt_selected = 0,
					.name        = "splash",
					.p_widget = NULL,
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
	static uint16_t page_image = 0;
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		//page_image = IMG_SPLASH_1;
		g_app_header = (fl_image_header_t *)__sectop("APPHEADER_1");
		sprintf(str, "Versão:%d.%d.%d.%03d", g_app_header->version_major,
									g_app_header->version_middle,
									g_app_header->version_minor,
									g_app_header->version_comp);
		changeTxt(&txt_version,str);
		//mn_screen_change_image(&splash_page,page_image);
		mn_screen_create_timer(&timer0,TIME_SCREENS);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(TIMER_SPLASH, EVENT_TIMER))
	{
		if (NexUpload_checkFile())
			{
				NexUpload_setDownloadBaudrate(1500000);
				NexUpload_downloadTftFile();
				NexUpload_waitingReset(10000);
				if(R_IsFileLoaderAvailable())
				{
					uint8_t val = 0;
					NexPage_show(load_page.name);
					NexWidget_visible("p0",NT_HIDE);
					NexWidget_visible("b0",NT_HIDE);
					NexWidget_visible("b1",NT_HIDE);
					NexWidget_visible("j0",NT_SHOW);
					while(R_loader_progress() < 16)
					{
						NexWidget_ProgressBar("j0",(val*100)/15);
						val++;
					}
					RESET
				}
				RESET
			}
			if(R_IsFileLoaderAvailable())
			{
				uint8_t val = 0;
				NexPage_show(load_page.name);
				NexWidget_visible("p0",NT_HIDE);
				NexWidget_visible("b0",NT_HIDE);
				NexWidget_visible("b1",NT_HIDE);
				NexWidget_visible("j0",NT_SHOW);
				while(R_loader_progress() < 16)
				{
					NexWidget_ProgressBar("j0",(val*100)/15);
					val++;
				}
				RESET
			}
			warn_args.buttonUseInit = BTN_NO_USE;
			warn_args.buttonUseEnd = BTN_ASK;
			warn_args.img_txt[0] = IMG_WARNING_1;
			warn_args.img_txt[1] = IMG_WARNING_2;
			warn_args.img_txt[2] = IMG_WARNING_3;
			warn_args.img_txt[3] = IMG_WARNING_4;
			warn_args.img_txt[4] = IMG_WARNING_5;
			warn_args.img_txt[5] = IMG_WARNING_6;
			warn_args.msg_count = 6;
			warn_args.time_interval = TIME_SCREENS;
			warn_args.func_callback = warning_callback;
			warning_page.p_args = &warn_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
	}
}

static void warning_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM:
			warning_page.p_args = &warn_zeromaq_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
			break;
		case BTN_PRESSED_NAO:
			machine_enable();
			mn_screen_change(&main_page,EVENT_SHOW);
			break;
		break;
	}
}

static void warning_zeromaqinit_callback(warn_btn_t btn_type)
{
	machine_enable();
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: mn_screen_change(&zerarmaq_page,EVENT_SHOW); break;
		case BTN_PRESSED_NAO: mn_screen_change(&main_page,EVENT_SHOW); break;
	}

//	machine_enable();
//	machine_zerar_maquina();
//	mn_screen_change(&main_page,EVENT_SHOW);
}
