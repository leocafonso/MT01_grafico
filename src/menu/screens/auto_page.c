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
#include "state_functions.h"
#include "keypad_page.h"

#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "xio.h"
#include "macros.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 6

#define TIME_SCREENS 2000

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_desloca_callback(warn_btn_t btn_type);
static void warning_semzeromaquina_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_rodar = {.name = "b1", .selectable = true};
static mn_widget_t btn_sim = {.name = "b2", .selectable = true};
static mn_widget_t btn_deslocar = {.name = "b3", .selectable = true};
static mn_widget_t btn_sel = {.name = "b4", .selectable = true};
static mn_widget_t btn_limites = {.name = "b5", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_warning_t warn_args;
static mn_warning_t warn_semzeromaquina_args = { .buttonUseInit = BTN_OK,
											.img_txt[0] = IMG_SEM_ZERO_MAQ,
											.msg_count = 1,
											.func_callback = warning_semzeromaquina_callback
										   };
static mn_keypad_t selLines_keypad_args;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_rodar,&btn_sim,&btn_deslocar,&btn_sel,&btn_limites,&btn_voltar
};


#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t auto_page = {.id 		 = SC_PAGE2,
					.wt_selected = 0,
					.name        = "auto",
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
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{


	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_rodar.id,EVENT_CLICK))
	{
		xio_open(cs.primary_src,0,0);
		if(uspiffs[0].f < 0)
		{
			xio_close(cs.primary_src);
			mn_screen_change(&selFiles_page,EVENT_SHOW);
		}
		else
		{
			xio_close(cs.primary_src);
			mn_screen_change(&fileInfo_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sim.id,EVENT_CLICK))
	{
		xio_open(cs.primary_src,0,0);
		if(uspiffs[0].f < 0)
		{
			xio_close(cs.primary_src);
			mn_screen_change(&selFiles_page,EVENT_SHOW);
		}
		else
		{
			xio_close(cs.primary_src);
			mn_screen_change(&fileInfo_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_deslocar.id,EVENT_CLICK))
	{
		if ((zero_flags & ZERO_MAQ_FLAG) ==  ZERO_MAQ_FLAG)
		{
			xio_close(cs.primary_src);
			warn_args.buttonUseInit = BTN_ASK;
			warn_args.img_txt[0] = IMG_CONTINUAR;
			warn_args.msg_count = 1;
			warn_args.func_callback = warning_desloca_callback;
			warning_page.p_args = &warn_args;
		}
		else
		{
			warning_page.p_args = &warn_semzeromaquina_args;
		}
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sel.id,EVENT_CLICK))
	{
		selLines_keypad_args.p_var = &selecionarLinhas;
		selLines_keypad_args.key_var = KEY_LINES;
		selLines_keypad_args.step = 1;
		selLines_keypad_args.min = 0;
		selLines_keypad_args.max = selecionarlinhasMax();
		keypad_page.p_args = &selLines_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_limites.id,EVENT_CLICK))
	{
		mn_screen_change(&limite_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&main_page,EVENT_SHOW);
	}
}

static void warning_desloca_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
	case BTN_PRESSED_SIM: mn_screen_change(&desloca_page,EVENT_SHOW);break;
	case BTN_PRESSED_NAO: mn_screen_change(&auto_page,EVENT_SHOW);break;
	}
}

static void warning_semzeromaquina_callback(warn_btn_t btn_type)
{
	mn_screen_change(&jog_page,EVENT_SHOW);
}
