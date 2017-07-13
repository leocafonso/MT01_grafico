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

#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "xio.h"
#include "keypad_page.h"
#include "config_maquina.h"
#include "state_functions.h"
#include "eeprom.h"
#include "keyboard.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 3

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_kerf = {.name = "p0", .selectable = true};
static mn_widget_t btn_antimergulho = {.name = "p1", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_kerf,&btn_antimergulho,&btn_voltar
};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t cfgParTHC_page = {.id 		 = SC_PAGE9,
					.wt_selected = 0,
					.name        = "parTHC",
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
static void cfgParTHC_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void cfgParTHC_key_release (void *p_arg)
{
	mn_screen_event_t touch;
	uint32_t *key_pressed = p_arg;
	if (btn_voltar.click == NT_PRESS)
	{
		widgetClick(&btn_voltar, NT_RELEASE);
		touch.event = EVENT_SIGNAL(btn_voltar.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	if (*key_pressed == KEY_ENTER)
	{
		widgetClick(page->p_widget[page->wt_selected], NT_RELEASE);
		touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	cfgParTHC_page.iif_func[SC_KEY_ESC] = cfgParTHC_key_esc;
	cfgParTHC_page.iif_func[SC_KEY_RELEASE] = cfgParTHC_key_release;
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
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW ||
		p_page_hdl->event == EMERGENCIA_EVENT)
	{
		widgetChangePic(&btn_kerf,(machine_flag_get(KERF) ? (IMG_LIG_EN) : (IMG_DESL_EN)),NO_IMG);
		widgetChangePic(&btn_antimergulho,(machine_flag_get(MERGULHO) ? (IMG_LIG_EN) : (IMG_DESL_EN)),NO_IMG);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_kerf.id,EVENT_CLICK))
	{
		machine_flag_set(KERF,!machine_flag_get(KERF));
		widgetChangePic(&btn_kerf,(machine_flag_get(KERF) ? (IMG_LIG_EN) : (IMG_DESL_EN)),NO_IMG);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_antimergulho.id,EVENT_CLICK))
	{
		machine_flag_set(MERGULHO,!machine_flag_get(MERGULHO));
		widgetChangePic(&btn_antimergulho,(machine_flag_get(MERGULHO) ? (IMG_LIG_EN) : (IMG_DESL_EN)),NO_IMG);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgMaq_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

