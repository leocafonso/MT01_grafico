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

#define PAR_IN_SCREEN 5
#define NUM_PAR 	  12

#define POINT_3 0.001
#define POINT_2 0.01
#define POINT_1 0.1
#define POINT_0 1

#define RESET_ENTRY 1
#define RESET_SET 2
#define RESET_CLEAR 0

#define TIMER_NUM 0

#define WIDGET_NUM 8

typedef struct  {
	nt_button_t btn_img;
	float * var;
	float var_min;
	float var_max;
	float var_step;
	uint16_t var_und_imag;
	mn_screen_t *p_ret_page;
	mn_screen_t *p_next_page;
	void (*p_fn_btn)(void *p_args);
}btn_fields_t;

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void fillFields(const btn_fields_t *p_btn);
static void fn_keyboard(void *p_args);
static void fn_formMem(void *p_args);


/* Static variables and const */
static mn_widget_t btn_par[5] = { 	{.name = "b1", .selectable = true},
									{.name = "b2", .selectable = true},
									{.name = "b3", .selectable = true},
									{.name = "b4", .selectable = true},
									{.name = "b5", .selectable = true} };

static mn_widget_t label_un[5] = { 	{.name = "p0", .selectable = false},
									{.name = "p1", .selectable = false},
									{.name = "p2", .selectable = false},
									{.name = "p3", .selectable = false},
									{.name = "p4", .selectable = false} };

static mn_widget_t cfg_txt[5] = { 	{.name = "t0", .selectable = false},
									{.name = "t1", .selectable = false},
									{.name = "t2", .selectable = false},
									{.name = "t3", .selectable = false},
									{.name = "t4", .selectable = false} };

static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};
static mn_widget_t btn_up = {.name = "b6", .selectable = true};
static mn_widget_t btn_down = {.name = "b7", .selectable = true};
static mn_load_t load_arg;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_par[0],&btn_par[1],&btn_par[2],&btn_par[3],&btn_par[4],&btn_voltar,&btn_up,&btn_down
};

const btn_fields_t btn_par_maq[NUM_PAR] = 	{
	{{IMG_PAR_MAQ_EIXOX1, IMG_PAR_MAQ_EIXOX1_PRESSED}               , &st_cfg.mot[MOTOR_4].travel_rev, 0, MMREV_MAX    , POINT_3,IMG_UN_MM      , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_EIXOX2, IMG_PAR_MAQ_EIXOX2_PRESSED}               , &st_cfg.mot[MOTOR_3].travel_rev, 0, MMREV_MAX    , POINT_3,IMG_UN_MM      , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_EIXOY, IMG_PAR_MAQ_EIXOY_PRESSED}                 , &st_cfg.mot[MOTOR_2].travel_rev, 0, MMREV_MAX    , POINT_3,IMG_UN_MM      , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_JERKX, IMG_PAR_MAQ_JERKX_PRESSED}                 , &cm.a[AXIS_X].jerk_max         , 0, JERK_MAX     , POINT_0,IMG_UN_MM_MIN3 , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_JERKY, IMG_PAR_MAQ_JERKY_PRESSED}                 , &cm.a[AXIS_Y].jerk_max         , 0, JERK_MAX     , POINT_0,IMG_UN_MM_MIN3 , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_VEL_X, IMG_PAR_MAQ_VEL_X_PRESSED}                 , &cm.a[AXIS_X].velocity_max     , 0, VEL_MAX      , POINT_0,IMG_UN_MM_MIN  , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_VEL_Y, IMG_PAR_MAQ_VEL_Y_PRESSED}                 , &cm.a[AXIS_Y].velocity_max     , 0, VEL_MAX      , POINT_0,IMG_UN_MM_MIN  , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_VEL_Z, IMG_PAR_MAQ_VEL_Z_PRESSED}                 , &cm.a[AXIS_Z].velocity_max     , 0, VEL_MAX      , POINT_0,IMG_UN_MM_MIN  , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_DESVIO_JUNCAO, IMG_PAR_MAQ_DESVIO_JUNCAO_PRESSED} , &cm.a[AXIS_X].junction_dev     , 0, JUNCDEV_MAX  , POINT_2,IMG_UN_MM      , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_AC_JUNCAO, IMG_PAR_MAQ_AC_JUNCAO_PRESSED}         , &cm.junction_acceleration      , 0, JUNCACEL_MAX , POINT_0,IMG_UN_MM_MIN3 , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_TOL_ARCO, IMG_PAR_MAQ_TOL_ARCO_PRESSED}           , &cm.chordal_tolerance          , 0, CHODAL_MAX   , POINT_2,IMG_UN_MM      , &cfgMaqPar_page, &cfgMaqPar_page, fn_keyboard},
	{{IMG_PAR_MAQ_FORMAT, IMG_PAR_MAQ_FORMAT_PRESSED}               , NULL                           , 0, NULL         , NULL   ,NULL           , &cfgMaqPar_page, &cfgMaqPar_page, fn_formMem},
};

static float var_buffer;
static uint8_t reset_flag = RESET_CLEAR;
static mn_keypad_t cfgPar_keypad_args;
static uint16_t new_index = 0;

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
/************************** Static functions *********************************************/
static void fn_keyboard(void *p_args)
{
	btn_fields_t *p_btn = p_args;
	cfgPar_keypad_args.p_var = p_btn->var;
	var_buffer   = *cfgPar_keypad_args.p_var;
	cfgPar_keypad_args.step = p_btn->var_step;
	cfgPar_keypad_args.min = p_btn->var_min;
	cfgPar_keypad_args.max = p_btn->var_max;
	cfgPar_keypad_args.p_ret_page = p_btn->p_ret_page;
	cfgPar_keypad_args.p_next_page = p_btn->p_next_page;
	keypad_page.p_args = &cfgPar_keypad_args;
	mn_screen_change(&keypad_page,EVENT_SHOW);
}

static void fn_formMem(void *p_args)
{
//	NexPage_show(load_page.name);
//	NexWidget_visible("p0",NT_HIDE);
//	NexWidget_visible("b0",NT_HIDE);
//	NexWidget_visible("b1",NT_HIDE);
//	NexWidget_visible("t0",NT_SHOW);
//	NexWidget_txt("t0","formatando...");
//	vTaskDelay(500 / portTICK_PERIOD_MS);
//	RESET;
	load_arg.type = MEM_FORMAT;
	load_page.p_args = &load_arg;
	mn_screen_change(&load_page,EVENT_SHOW);
}

static void showParDown(const btn_fields_t *p_btn, uint16_t index)
{
	if (index >= PAR_IN_SCREEN)
	{
		new_index = index - (PAR_IN_SCREEN - 1);

		fillFields(p_btn);
	}
}

static void showParUp(const btn_fields_t *p_btn, uint16_t index)
{
	new_index = index;

	fillFields(p_btn);
}


static void fillFields(const btn_fields_t *p_btn)
{
	char result_str[20];
	uint16_t decimalCount = 0;
	for (uint8_t i = 0; i < PAR_IN_SCREEN; i++)
	{
		widgetChangePic(cfgMaqPar_page.p_widget[i],
					p_btn[i + new_index].btn_img.btn_released,
					p_btn[i + new_index].btn_img.btn_pressed);
		widgetChangePic(&label_un[i], IMG_UN_ERASE, NO_IMG);
		if (p_btn[i + new_index].var_und_imag != NULL)
		{
			widgetChangePic(&label_un[i], p_btn[i + new_index].var_und_imag, NO_IMG);
		}
		if (p_btn[i + new_index].var != NULL )
		{
			decimalCount = get_decimal_digits(p_btn[i + new_index].var_step);
			sprintf(result_str, "%0.*f", decimalCount,*p_btn[i + new_index].var);
			changeTxt(&cfg_txt[i],result_str);
		}
		else
		{
			changeTxt(&cfg_txt[i]," ");
		}
	}
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
	if (*key_pressed == KEY_UP)
	{
		touch.event = EVENT_SIGNAL(btn_up.id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	if (*key_pressed == KEY_DOWN)
	{
		touch.event = EVENT_SIGNAL(btn_down.id,EVENT_CLICK);
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
	cfgMaqPar_page.iif_func[SC_KEY_ENTER] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_DOWN] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_UP] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_RIGHT] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_LEFT] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_ZDOWN] = mn_screen_idle;
	cfgMaqPar_page.iif_func[SC_KEY_ZUP] = mn_screen_idle;
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
	static int16_t listIndex = 0;
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
		listIndex = 0;
		new_index = 0;
		page->wt_selected = mn_screen_select_widget(page,&btn_par[0]);
		fillFields(btn_par_maq);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[0].id,EVENT_CLICK))
	{
		listIndex = new_index;
		btn_par_maq[listIndex].p_fn_btn(&btn_par_maq[listIndex]);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[1].id,EVENT_CLICK))
	{
		listIndex = new_index + 1;
		btn_par_maq[listIndex].p_fn_btn(&btn_par_maq[listIndex]);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[2].id,EVENT_CLICK))
	{
		listIndex = new_index + 2;
		btn_par_maq[listIndex].p_fn_btn(&btn_par_maq[listIndex]);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[3].id,EVENT_CLICK))
	{
		listIndex = new_index + 3;
		btn_par_maq[listIndex].p_fn_btn(&btn_par_maq[listIndex]);
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_par[4].id,EVENT_CLICK))
	{
		listIndex = new_index + 4;
		btn_par_maq[listIndex].p_fn_btn(&btn_par_maq[listIndex]);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_up.id,EVENT_CLICK))
		{
			if (page->wt_selected > 0)
			{
				widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
				page->wt_selected -= 1;
				widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
				listIndex -= 1;
			}
			else if (page->wt_selected == 0)
			{
				if (listIndex > 0)
				{
					listIndex -= 1;
					showParUp(btn_par_maq,listIndex);
				}
			}
		}

		else if (p_page_hdl->event == EVENT_SIGNAL(btn_down.id,EVENT_CLICK))
		{
			if (page->wt_selected < PAR_IN_SCREEN - 1)
			{
				widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
				page->wt_selected += 1;
				widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
			}
			if (listIndex < NUM_PAR - 1)
			{
				listIndex += 1;
				if (page->wt_selected == PAR_IN_SCREEN - 1)
					showParDown(btn_par_maq,listIndex);
			}
		}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{

		if ((reset_flag & RESET_SET) == RESET_SET)
		{
			if (cm.a[AXIS_X].junction_dev != cm.a[AXIS_Y].junction_dev)
			{
				for (uint8_t i = AXIS_Y; i < AXIS_C + 1; i++)
				{
					cm.a[i].junction_dev = cm.a[AXIS_X].junction_dev;
					nv_save_parameter_flt(&cm.a[i].junction_dev);
				}
			}

			for (uint8_t i = AXIS_X; i < AXIS_C + 1; i++)
			{
				if (cm.a[i].velocity_max != cm.a[i].feedrate_max)
				{
					cm.a[i].feedrate_max = cm.a[i].velocity_max;
					nv_save_parameter_flt(&cm.a[i].feedrate_max);
				}
			}
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

