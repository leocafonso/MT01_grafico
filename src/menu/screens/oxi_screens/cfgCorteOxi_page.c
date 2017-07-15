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
#include "config_menu_pl.h"
#include "state_functions.h"
#include "eeprom.h"
#include "keyboard.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 6

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_alt_per = {.name = "b1", .selectable = true};
static mn_widget_t btn_alt_corte = {.name = "b2", .selectable = true};
static mn_widget_t btn_vel_corte = {.name = "b3", .selectable = true};
static mn_widget_t btn_tempo_aquecimento = {.name = "b4", .selectable = true};
static mn_widget_t btn_tempo_per = {.name = "b5", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

//static mn_widget_t txt_alt_per = {.name = "t0", .selectable = false};
//static mn_widget_t txt_alt_corte = {.name = "t1", .selectable = false};
//static mn_widget_t txt_vel_corte = {.name = "t2", .selectable = false};
//static mn_widget_t txt_tempo_per = {.name = "t3", .selectable = false};
//static mn_widget_t txt_tensao_thc = {.name = "t4", .selectable = false};

static mn_widget_t cfg_txt[PL_CONFIG_MAX] = { 	{.name = "t0", .selectable = false},
												{.name = "t1", .selectable = false},
												{.name = "t2", .selectable = false},
												{.name = "t3", .selectable = false},
												{.name = "t4", .selectable = false} };

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_alt_per,&btn_alt_corte,&btn_vel_corte,&btn_tempo_per,&btn_tempo_aquecimento,&btn_voltar
};

static mn_keypad_t cfgCorte_keypad_args;
static mn_screen_event_t cfgCorte;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t cfgCutOx_page = {.id 		 = SC_PAGE9,
					.wt_selected = 0,
					.name        = "cfgCutOx",
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
static void cfgCorte_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void cfgCorte_key_release (void *p_arg)
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
	cfgCutOx_page.iif_func[SC_KEY_ESC] = cfgCorte_key_esc;
	cfgCutOx_page.iif_func[SC_KEY_RELEASE] = cfgCorte_key_release;
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
		for (uint8_t index = 0; index < OX_CONFIG_MAX; index++)
		{
			decNum = 1;
			decCount = 0;
			decimalCount = 0;
			decCount = get_dec_digits(configVarOx[index]);
			decimalCount = get_decimal_digits(ox_init_step[index]);

			if(decimalCount > 0)
				digits = decCount + decimalCount + 1;
			else
				digits = decCount;

			for (uint8_t i = 1; i<decCount; i++)
			{
				decNum = decNum*10;
			}
			sprintf(result_str, "%0*.*f", digits , decimalCount,configVarOx[index]);
			changeTxt(&cfg_txt[index],result_str);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_alt_per.id,EVENT_CLICK))
	{
		cfgCorte_keypad_args.p_var = &configVarOx[OX_CONFIG_ALTURA_PERFURACAO];
		cfgCorte_keypad_args.key_var = KEY_CONFIG_OX;
		cfgCorte_keypad_args.step = ox_init_step[OX_CONFIG_ALTURA_PERFURACAO];
		cfgCorte_keypad_args.min = ox_init_min[OX_CONFIG_ALTURA_PERFURACAO];
		cfgCorte_keypad_args.max = ox_init_max[OX_CONFIG_ALTURA_PERFURACAO];
		keypad_page.p_args = &cfgCorte_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_alt_corte.id,EVENT_CLICK))
	{
		cfgCorte_keypad_args.p_var = &configVarOx[OX_CONFIG_ALTURA_CORTE];
		cfgCorte_keypad_args.key_var = KEY_CONFIG_OX;
		cfgCorte_keypad_args.step = ox_init_step[OX_CONFIG_ALTURA_CORTE];
		cfgCorte_keypad_args.min = ox_init_min[OX_CONFIG_ALTURA_CORTE];
		cfgCorte_keypad_args.max = ox_init_max[OX_CONFIG_ALTURA_CORTE];
		keypad_page.p_args = &cfgCorte_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_vel_corte.id,EVENT_CLICK))
	{
		cfgCorte_keypad_args.p_var = &configVarOx[OX_CONFIG_VELOC_CORTE];
		cfgCorte_keypad_args.key_var = KEY_CONFIG_OX;
		cfgCorte_keypad_args.step = ox_init_step[OX_CONFIG_VELOC_CORTE];
		cfgCorte_keypad_args.min = ox_init_min[OX_CONFIG_VELOC_CORTE];
		cfgCorte_keypad_args.max = ox_init_max[OX_CONFIG_VELOC_CORTE];
		keypad_page.p_args = &cfgCorte_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_tempo_per.id,EVENT_CLICK))
	{
		cfgCorte_keypad_args.p_var = &configVarOx[OX_CONFIG_TEMPO_PERFURACAO];
		cfgCorte_keypad_args.key_var = KEY_CONFIG_OX;
		cfgCorte_keypad_args.step = ox_init_step[OX_CONFIG_TEMPO_PERFURACAO];
		cfgCorte_keypad_args.min = ox_init_min[OX_CONFIG_TEMPO_PERFURACAO];
		cfgCorte_keypad_args.max = ox_init_max[OX_CONFIG_TEMPO_PERFURACAO];
		keypad_page.p_args = &cfgCorte_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_tempo_aquecimento.id,EVENT_CLICK))
	{
		cfgCorte_keypad_args.p_var = &configVarOx[OX_CONFIG_TEMPO_AQUECIMENTO];
		cfgCorte_keypad_args.key_var = KEY_CONFIG_OX;
		cfgCorte_keypad_args.step = ox_init_step[OX_CONFIG_TEMPO_AQUECIMENTO];
		cfgCorte_keypad_args.min = ox_init_min[OX_CONFIG_TEMPO_AQUECIMENTO];
		cfgCorte_keypad_args.max = ox_init_max[OX_CONFIG_TEMPO_AQUECIMENTO];
		keypad_page.p_args = &cfgCorte_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&main_page, EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

