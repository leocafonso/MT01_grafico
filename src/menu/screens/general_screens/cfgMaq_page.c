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

#include "config_maquina.h"
#include "state_functions.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 5

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_alt_desloc = {.name = "b1", .selectable = true};
//static mn_widget_t btn_modo_maq = {.name = "b2", .selectable = true};
static mn_widget_t btn_par_thc = {.name = "b3", .selectable = true};
static mn_widget_t btn_par_maq = {.name = "b4", .selectable = true};
static mn_widget_t btn_modo_maq = {.name = "p0", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_widget_t txt_alt_desloc = {.name = "t0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_alt_desloc,&btn_par_thc,&btn_par_maq,&btn_modo_maq,&btn_voltar
};

mn_keypad_t cfgMaq_keypad_args;
static mn_screen_event_t cfgCorte;
#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t cfgMaq_page = {.id 		 = SC_PAGE9,
					.wt_selected = 0,
					.name        = "cfgMaq",
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
static void cfgMaq_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void cfgMaq_key_release (void *p_arg)
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
	cfgMaq_page.iif_func[SC_KEY_ESC] = cfgMaq_key_esc;
	cfgMaq_page.iif_func[SC_KEY_RELEASE] = cfgMaq_key_release;
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
		decNum = 1;
		decCount = 0;
		decimalCount = 0;
		decCount = get_dec_digits(configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO]);
		decimalCount = get_decimal_digits(pl_init_step[CFG_MAQUINA_ALT_DESLOCAMENTO]);

		if(decimalCount > 0)
			digits = decCount + decimalCount + 1;
		else
			digits = decCount;

		for (uint8_t i = 1; i<decCount; i++)
		{
			decNum = decNum*10;
		}
		sprintf(result_str, "%0*.*f", digits , decimalCount,configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO]);
		changeTxt(&txt_alt_desloc,result_str);
		widgetChangePic(&btn_modo_maq,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_EN) : (IMG_PL_EN)),NO_IMG);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_alt_desloc.id,EVENT_CLICK))
	{
		cfgMaq_keypad_args.p_var = &configVarMaq[CFG_MAQUINA_ALT_DESLOCAMENTO];
		cfgMaq_keypad_args.key_var = KEY_CONFIG_MAQ;
		cfgMaq_keypad_args.step = mq_init_step[CFG_MAQUINA_ALT_DESLOCAMENTO];
		cfgMaq_keypad_args.min = mq_init_min[CFG_MAQUINA_ALT_DESLOCAMENTO];
		cfgMaq_keypad_args.max = mq_init_max[CFG_MAQUINA_ALT_DESLOCAMENTO];
		cfgMaq_keypad_args.p_ret_page = page;
		keypad_page.p_args = &cfgMaq_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_modo_maq.id,EVENT_CLICK))
	{
		machine_flag_set(MODOMAQUINA,!machine_flag_get(MODOMAQUINA));
		widgetChangePic(&btn_modo_maq,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_EN) : (IMG_PL_EN)),NO_IMG);
		widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_par_thc.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgParTHC_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_par_maq.id,EVENT_CLICK))
	{
		mn_screen_change(&cfgMaqPar_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&main_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

