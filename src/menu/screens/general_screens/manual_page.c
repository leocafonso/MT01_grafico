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
#include "state_functions.h"
#include "keypad_page.h"

#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "xio.h"
#include "macros.h"
#include "keyboard.h"

/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 6

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_zerarmaquina_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_manual = {.name = "b1", .selectable = true};
static mn_widget_t btn_vel_manual = {.name = "b2", .selectable = true};
static mn_widget_t btn_zerar_maq = {.name = "b3", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_warning_t warn_zerarmaquina_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_ZERO_MAQ,
											.msg_count = 1,
											.func_callback = warning_zerarmaquina_callback};

static mn_keypad_t vel_keypad_args;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_manual,&btn_vel_manual,&btn_zerar_maq,&btn_voltar
};
static mn_screen_event_t manual;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t manual_page = {.id 		 = SC_PAGE2,
					.wt_selected = 0,
					.name        = "manual",
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
static void manual_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void manual_key_release (void *p_arg)
{
	if (btn_voltar.click == NT_PRESS)
	{
		widgetClick(&btn_voltar, NT_RELEASE);
		manual.event = EVENT_SIGNAL(btn_voltar.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &manual, 0 );
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
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	manual_page.iif_func[SC_KEY_ESC] = manual_key_esc;
	manual_page.iif_func[SC_KEY_RELEASE] = manual_key_release;
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
	if (p_page_hdl->event == EVENT_SHOW)
	{


	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_manual.id,EVENT_CLICK))
	{
		mn_screen_change(&jog_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_vel_manual.id,EVENT_CLICK))
	{
		vel_keypad_args.p_var = &configVarJog[JOG_RAPIDO];
		vel_keypad_args.key_var = KEY_CONFIG_JOG;
		vel_keypad_args.step = 1;
		vel_keypad_args.min = 10;
		vel_keypad_args.max = 10000;
		keypad_page.p_args = &vel_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zerar_maq.id,EVENT_CLICK))
	{
		warning_page.p_args = &warn_zerarmaquina_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&main_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

static void warning_zerarmaquina_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: machine_zerar_maquina(); break;
		case BTN_PRESSED_NAO: break;
	}
	mn_screen_change(&jog_page,EVENT_SHOW);
}
