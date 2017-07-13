/** @file screen.c
 *  @brief Function implementation for the screen.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */
#include "fInfo_page.h"
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

#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "xio.h"
#include "keypad_page.h"
#include "config_menu_pl.h"
#include "state_functions.h"
#include "eeprom.h"
#include "keyboard.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 5
/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

static void warning_rodar_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_nome_arquivo = {.name = "b1", .selectable = true};
static mn_widget_t btn_modo_maquina = {.name = "b2", .selectable = true};
static mn_widget_t btn_vel_corte = {.name = "b3", .selectable = true};
static mn_widget_t btn_ok = {.name = "b6", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_widget_t cfg_txt[5] = { 				{.name = "t0", .selectable = false},
												{.name = "t1", .selectable = false},
												{.name = "t2", .selectable = false} };

static mn_warning_t warn_cutting_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_CONTINUAR,
											.msg_count = 1,
											.func_callback = warning_rodar_callback
										   };

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_nome_arquivo,&btn_modo_maquina,&btn_vel_corte,&btn_ok,&btn_voltar
};

static mn_screen_event_t fileInfo;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t fInfoOX_page = {.id 		 = SC_PAGE13,
					.wt_selected = 0,
					.name        = "fInfoOX",
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
extern uint32_t choosedLine;
/************************** Static functions *********************************************/
static void fileInfo_key_esc (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	widgetClick(&btn_voltar, NT_PRESS);
}

static void fileInfo_key_release (void *p_arg)
{
	if (btn_voltar.click == NT_PRESS)
	{
		widgetClick(&btn_voltar, NT_RELEASE);
		fileInfo.event = EVENT_SIGNAL(btn_voltar.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &fileInfo, 0 );
	}
	uint32_t *key_pressed = p_arg;
	if (*key_pressed == KEY_ENTER)
	{
		mn_screen_event_t touch;
		widgetClick(page->p_widget[page->wt_selected], NT_RELEASE);
		touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	fInfoOX_page.iif_func[SC_KEY_ESC] = fileInfo_key_esc;
	fInfoOX_page.iif_func[SC_KEY_RELEASE] = fileInfo_key_release;
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
	static char result_str[20];
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW ||
		p_page_hdl->event == EMERGENCIA_EVENT)
	{
		spiffs_stat fileStat;
		xio_open(cs.primary_src,0,0);
		SPIFFS_fstat(&uspiffs[0].gSPIFFS, uspiffs[0].f, &fileStat);
		changeTxt(&cfg_txt[0],(const char *)fileStat.name);
		xio_close(cs.primary_src);
		sprintf(result_str, "%d", choosedLine);
		changeTxt(&cfg_txt[1],result_str);
		sprintf(result_str, "%4.0f", configVarOx[OX_CONFIG_VELOC_CORTE]);
		changeTxt(&cfg_txt[2],result_str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_ok.id,EVENT_CLICK))
	{
		warning_page.p_args = &warn_cutting_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&auto_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

static void warning_rodar_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM:
			if (g_runCase ==  FILE_AUTO)
			{
				mn_screen_change(&cutOxi_page,EVENT_SHOW);
			}
			else if (g_runCase ==  FILE_SIM)
			{
				mn_screen_change(&simOxi_page, EVENT_SHOW);
			}
			break;
		case BTN_PRESSED_NAO: mn_screen_change(&auto_page,EVENT_SHOW); break;
	}
}

