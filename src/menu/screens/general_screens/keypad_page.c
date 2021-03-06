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
#include "config.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 15

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_num_1 = {.name = "b0", .selectable = true};
static mn_widget_t btn_num_2 = {.name = "b1", .selectable = true};
static mn_widget_t btn_num_3 = {.name = "b2", .selectable = true};
static mn_widget_t btn_num_4 = {.name = "b3", .selectable = true};
static mn_widget_t btn_num_5 = {.name = "b4", .selectable = true};
static mn_widget_t btn_num_6 = {.name = "b5", .selectable = true};
static mn_widget_t btn_num_7 = {.name = "b6", .selectable = true};
static mn_widget_t btn_num_8 = {.name = "b7", .selectable = true};
static mn_widget_t btn_num_9 = {.name = "b8", .selectable = true};
static mn_widget_t btn_num_0 = {.name = "b9", .selectable = true};
static mn_widget_t btn_point = {.name = "b10", .selectable = true};
static mn_widget_t btn_erase = {.name = "b11", .selectable = true};
static mn_widget_t btn_cancel = {.name = "b12", .selectable = true};
static mn_widget_t btn_enter = {.name = "b13", .selectable = true};

static mn_widget_t num_txt = {.name = "t0", .selectable = false};


static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_num_1,
		&btn_num_2,
		&btn_num_3,
		&btn_num_4,
		&btn_num_5,
		&btn_num_6,
		&btn_num_7,
		&btn_num_8,
		&btn_num_9,
		&btn_num_0,
		&btn_point,
		&btn_erase,
		&btn_cancel,
		&btn_enter,
		&num_txt
};

static mn_screen_t *p_previous_page;
static mn_screen_t *p_back_page;
static mn_screen_event_t keypad;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t keypad_page = {.id 		 = SC_PAGE10,
					.wt_selected = 0,
					.name        = "keypad",
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
static void keypad_key_release (void *p_arg)
{
	uint32_t *key_pressed = p_arg;
	mn_screen_event_t touch;
	if (*key_pressed == KEY_ENTER)
	{
		touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_ESC)
	{
		touch.event = EVENT_SIGNAL(btn_cancel.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_UP)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		switch (page->wt_selected)
		{
			case 0:  page->wt_selected = 0;  break;
			case 1:  page->wt_selected = 1; break;
			case 2:  page->wt_selected = 2; break;
			case 3:  page->wt_selected = 0; break;
			case 4:  page->wt_selected = 1; break;
			case 5:  page->wt_selected = 2; break;
			case 6:  page->wt_selected = 3; break;
			case 7:  page->wt_selected = 4; break;
			case 8:  page->wt_selected = 5; break;
			case 9:  page->wt_selected = 7; break;
			case 10: page->wt_selected = 8; break;
			case 11: page->wt_selected = 11; break;
			case 13: page->wt_selected = 11; break;
			default: break;
		}
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
	else if (*key_pressed == KEY_DOWN)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		switch (page->wt_selected)
		{
			case 0:  page->wt_selected = 3;  break;
			case 1:  page->wt_selected = 4; break;
			case 2:  page->wt_selected = 5; break;
			case 3:  page->wt_selected = 6; break;
			case 4:  page->wt_selected = 7; break;
			case 5:  page->wt_selected = 8; break;
			case 6:  page->wt_selected = 6; break;
			case 7:  page->wt_selected = 9; break;
			case 8:  page->wt_selected = 10; break;
			case 9: page->wt_selected = 9; break;
			case 10: page->wt_selected = 10; break;
			case 11: page->wt_selected = 13; break;
			case 13: page->wt_selected = 13; break;
			default: break;
		}
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
	else if (*key_pressed == KEY_RIGHT)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		switch (page->wt_selected)
		{
			case 0:  page->wt_selected = 1;  break;
			case 1:  page->wt_selected = 2; break;
			case 2:  page->wt_selected = 11; break;
			case 3:  page->wt_selected = 4; break;
			case 4:  page->wt_selected = 5; break;
			case 5:  page->wt_selected = 13; break;
			case 6:  page->wt_selected = 7; break;
			case 7:  page->wt_selected = 8; break;
			case 8:  page->wt_selected = 8; break;
			case 9: page->wt_selected = 10; break;
			case 10: page->wt_selected = 10; break;
			case 11: page->wt_selected = 11; break;
			case 13: page->wt_selected = 13; break;
			default: break;
		}
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
	else if (*key_pressed == KEY_LEFT)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		switch (page->wt_selected)
		{
			case 0:  page->wt_selected = 0;  break;
			case 1:  page->wt_selected = 0; break;
			case 2:  page->wt_selected = 1; break;
			case 3:  page->wt_selected = 3; break;
			case 4:  page->wt_selected = 3; break;
			case 5:  page->wt_selected = 4; break;
			case 6:  page->wt_selected = 6; break;
			case 7:  page->wt_selected = 6; break;
			case 8:  page->wt_selected = 7; break;
			case 9: page->wt_selected = 9; break;
			case 10: page->wt_selected = 9; break;
			case 11: page->wt_selected = 2; break;
			case 13: page->wt_selected = 5; break;
			default: break;
		}
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	keypad_page.iif_func[SC_KEY_ENTER] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_ESC] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_DOWN] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_UP] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_RIGHT] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_LEFT] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_ZDOWN] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_ZUP] = mn_screen_idle;
	keypad_page.iif_func[SC_KEY_RELEASE] = keypad_key_release;
	//p_previous_page = page;
}

void page_detach (void *p_arg)
{
#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page->p_timer[i]);
#endif
}

uint16_t decimalCount = 0;
uint16_t digits;
bool flagKeypad;

void page_handler (void *p_arg)
{
	static char result_str[20] = "0";
	static float var_initial;
	float check_boundary;
	uint8_t btn_num;
	mn_screen_event_t *p_page_hdl = p_arg;
	mn_keypad_t *p_keypad = p_page_hdl->p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		uint32_t decNum = 1;
		uint16_t decCount = 0;
		page->wt_selected = 0;
		flagKeypad = false;
		decimalCount = 0;
		var_initial = *p_keypad->p_var;
		decCount = get_dec_digits(*p_keypad->p_var);
		decimalCount = get_decimal_digits(p_keypad->step);

		if(decimalCount > 0)
			digits = decCount + decimalCount + 1;
		else
			digits = decCount;

		for (uint8_t i = 1; i<decCount; i++)
		{
			decNum = decNum*10;
		}
		sprintf(result_str, "%0*.*f", digits , decimalCount,*p_keypad->p_var);
		changeTxt(&num_txt,result_str);
		snprintf(result_str, sizeof(result_str), "%s", "0");
		p_back_page = p_keypad->p_ret_page;
	}
	else if (p_page_hdl->event == EMERGENCIA_EVENT)
	{
		snprintf(result_str, strlen(result_str), "%s", result_str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_point.id,EVENT_CLICK))
	{
		snprintf(result_str, sizeof(result_str), "%s%s", result_str, ".");
		changeTxt(&num_txt, result_str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_erase.id,EVENT_CLICK))
	{
		if(strlen(result_str) > 1)
			snprintf(result_str, strlen(result_str), "%s", result_str);
		else
			snprintf(result_str, sizeof(result_str), "%s", "0");
		changeTxt(&num_txt, result_str);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_cancel.id,EVENT_CLICK))
	{
		mn_screen_change(p_keypad->p_ret_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_enter.id,EVENT_CLICK))
	{
		if (flagKeypad == true)
		{
			*p_keypad->p_var = atof(result_str);
			if (*p_keypad->p_var <= p_keypad->min)
				*p_keypad->p_var = p_keypad->min;
			nv_save_parameter_flt(p_keypad->p_var);
			mn_screen_change(p_keypad->p_next_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
	else
	{
		btn_num = GET_ID(p_page_hdl->event);
		if (btn_num == 10)
			btn_num = 0;
		if(strlen(result_str) > 1 || flagKeypad == true)
			snprintf(result_str, sizeof(result_str), "%s%d", result_str, btn_num);
		else
			snprintf(result_str, sizeof(result_str), "%d", btn_num);
		check_boundary = atof(result_str);
		if (check_boundary >= p_keypad->max)
			sprintf(result_str, "%0*.*f", digits , decimalCount,p_keypad->max);
		flagKeypad = true;
		changeTxt(&num_txt, result_str);
	}
}

