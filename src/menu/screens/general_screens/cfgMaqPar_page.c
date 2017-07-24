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
/* Defines */

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

/************************** Static functions *********************************************/
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
	static char result_str[20];
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW ||
		p_page_hdl->event == EMERGENCIA_EVENT)
	{
		uint32_t decNum;
		uint16_t decCount;
		uint16_t decimalCount;
		uint16_t digits;
		for (uint8_t index = 0; index < 5; index++)
		{
			decNum = 1;
			decCount = 0;
			decimalCount = 0;
			decCount = get_dec_digits(configVarParMaq[index]);
			decimalCount = get_decimal_digits(pm_init_step[index]);

			if(decimalCount > 0)
				digits = decCount + decimalCount + 1;
			else
				digits = decCount;

			for (uint8_t i = 1; i<decCount; i++)
			{
				decNum = decNum*10;
			}
			sprintf(result_str, "%0*.*f", digits , decimalCount,configVarParMaq[index]);
			changeTxt(&cfg_txt[index],result_str);
		}
	}
	for (uint8_t index = 0; index < 5; index++)
	{
		if (p_page_hdl->event == EVENT_SIGNAL(btn_par[index].id,EVENT_CLICK))
		{
			cfgPar_keypad_args.p_var = &configVarParMaq[index];
			cfgPar_keypad_args.key_var = KEY_CONFIG_PAR_MAQ;
			cfgPar_keypad_args.step = pm_init_step[index];
			cfgPar_keypad_args.min = pm_init_min[index];
			cfgPar_keypad_args.max = pm_init_max[index];
			cfgPar_keypad_args.p_ret_page = page;
			keypad_page.p_args = &cfgPar_keypad_args;
			mn_screen_change(&keypad_page,EVENT_SHOW);
		}
	}
	if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgMaq_page, EVENT_SHOW);
	}
	if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

