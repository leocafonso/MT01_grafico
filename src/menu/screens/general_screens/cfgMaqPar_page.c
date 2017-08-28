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

#include "config_menu_ox.h"
#include "state_functions.h"

#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "hardware.h"
#include "json_parser.h"
#include "text_parser.h"
#include "gcode_parser.h"
#include "canonical_machine.h"
#include "plan_arc.h"
#include "planner.h"
#include "stepper.h"
#include "macros.h"
#include "plasma.h"
#include "xio.h"
/* Defines */

#define	MMREV_MAX		200
#define	JERK_MAX		10000
#define	VEL_MAX			20000
#define	JUNCDEV_MAX		10
#define	JUNCACEL_MAX	2000000
#define	CHODAL_MAX		10

#define POINT_3 0.001
#define POINT_2 0.01
#define POINT_1 0.1
#define POINT_0 1

#define RESET_ENTRY 1
#define RESET_SET 2
#define RESET_CLEAR 0

#define TIMER_NUM 0

#define WIDGET_NUM 6

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_par[5] = { 	{.name = "b1", .selectable = true},
									{.name = "b2", .selectable = true},
									{.name = "b3", .selectable = true},
									{.name = "b4", .selectable = true},
									{.name = "b5", .selectable = true} };

static mn_widget_t cfg_txt[5] = { 	{.name = "t0", .selectable = false},
									{.name = "t1", .selectable = false},
									{.name = "t2", .selectable = false},
									{.name = "t3", .selectable = false},
									{.name = "t4", .selectable = false} };

static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_par[0],&btn_par[1],&btn_par[2],&btn_par[3],&btn_par[4],&btn_voltar
};

static float var_buffer;
static uint8_t reset_flag = RESET_CLEAR;
static mn_keypad_t cfgPar_keypad_args;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t cfgMaqPar_page = {.id 		 = SC_PAGE9,
					.wt_selected = 0,
					.name        = "cfgPar",
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
static void (*p_fn_btn[7])(void *p_args);
/************************** Static functions *********************************************/
void fn_eixoX1(void *p_args)
{
	cfgPar_keypad_args.p_var = &st_cfg.mot[MOTOR_4].travel_rev;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = POINT_3;
	cfgPar_keypad_args.min = 0;
	cfgPar_keypad_args.max = MMREV_MAX;
	cfgPar_keypad_args.p_ret_page = page;
	cfgPar_keypad_args.p_next_page = page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}

void fn_eixoX2(void *p_args)
{
	cfgPar_keypad_args.p_var = &st_cfg.mot[MOTOR_3].travel_rev;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = POINT_3;
	cfgPar_keypad_args.min = 0;
	cfgPar_keypad_args.max = MMREV_MAX;
	cfgPar_keypad_args.p_ret_page = page;
	cfgPar_keypad_args.p_next_page = page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}

void fn_eixoY(void *p_args)
{
	cfgPar_keypad_args.p_var = &st_cfg.mot[MOTOR_2].travel_rev;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = POINT_3;
	cfgPar_keypad_args.min = 0;
	cfgPar_keypad_args.max = MMREV_MAX;
	cfgPar_keypad_args.p_ret_page = page;
	cfgPar_keypad_args.p_next_page = page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}

void fn_jerkX(void *p_args)
{
	cfgPar_keypad_args.p_var = &cm.a[AXIS_X].jerk_max;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = POINT_0;
	cfgPar_keypad_args.min = 0;
	cfgPar_keypad_args.max = JERK_MAX;
	cfgPar_keypad_args.p_ret_page = page;
	cfgPar_keypad_args.p_next_page = page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}

void fn_jerkY(void *p_args)
{
	cfgPar_keypad_args.p_var = &cm.a[AXIS_X].jerk_max;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = POINT_0;
	cfgPar_keypad_args.min = 0;
	cfgPar_keypad_args.max = JERK_MAX;
	cfgPar_keypad_args.p_ret_page = page;
	cfgPar_keypad_args.p_next_page = page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}


void cfgMaqInit(void)
{
	char result_str[20];
	p_fn_btn[0] = fn_eixoX1;
	p_fn_btn[1] = fn_eixoX2;
	p_fn_btn[2] = fn_eixoY;
	p_fn_btn[3] = fn_jerkX;
	p_fn_btn[4] = fn_jerkY;

	sprintf(result_str, "%0.3f", st_cfg.mot[MOTOR_4].travel_rev);
	changeTxt(&cfg_txt[0],result_str);
	sprintf(result_str, "%0.3f", st_cfg.mot[MOTOR_3].travel_rev);
	changeTxt(&cfg_txt[1],result_str);
	sprintf(result_str, "%0.3f", st_cfg.mot[MOTOR_2].travel_rev);
	changeTxt(&cfg_txt[2],result_str);
	sprintf(result_str, "%0.0f", cm.a[AXIS_X].jerk_max);
	changeTxt(&cfg_txt[3],result_str);
	sprintf(result_str, "%0.0f", cm.a[AXIS_Y].jerk_max);
	changeTxt(&cfg_txt[4],result_str);
}

static void cfgMaqPar_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void cfgMaqPar_key_release (void *p_arg)
{
	uint32_t *key_pressed = p_arg;
	mn_screen_event_t touch;
	if (*key_pressed == KEY_ENTER)
	{

		widgetClick(page->p_widget[page->wt_selected], NT_RELEASE);
		touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_ESC)
	{
		widgetClick(&btn_voltar, NT_RELEASE);
		touch.event = EVENT_SIGNAL(btn_voltar.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	cfgMaqPar_page.iif_func[SC_KEY_ESC] = cfgMaqPar_key_esc;
	cfgMaqPar_page.iif_func[SC_KEY_RELEASE] = cfgMaqPar_key_release;
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
		if ((reset_flag & RESET_ENTRY) == RESET_ENTRY)
		{
			if (var_buffer != *cfgPar_keypad_args.p_var)
			{
				reset_flag |= RESET_SET;
			}
		}
		reset_flag |= RESET_ENTRY;
		page->wt_selected = mn_screen_select_widget(page,&btn_par[0]);
		cfgMaqInit();
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[0].id,EVENT_CLICK))
	{
		p_fn_btn[0](NULL);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[1].id,EVENT_CLICK))
	{
		p_fn_btn[1](NULL);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[2].id,EVENT_CLICK))
	{
		p_fn_btn[2](NULL);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[3].id,EVENT_CLICK))
	{
		p_fn_btn[3](NULL);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[4].id,EVENT_CLICK))
	{
		p_fn_btn[4](NULL);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{

		if ((reset_flag & RESET_SET) == RESET_SET)
		{
			NexPage_show(load_page.name);
			NexWidget_visible("p0",NT_HIDE);
			NexWidget_visible("b0",NT_HIDE);
			NexWidget_visible("b1",NT_HIDE);
			NexWidget_visible("t0",NT_SHOW);
			NexWidget_txt("t0","Resetando...");
			vTaskDelay(500 / portTICK_PERIOD_MS);
			RESET;
		}
		reset_flag = RESET_CLEAR;
		mn_screen_change(&cfgMaq_page, EVENT_SHOW);
	}
	if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

