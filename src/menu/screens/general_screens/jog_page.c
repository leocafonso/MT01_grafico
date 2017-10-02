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

/* Defines */

#define TIMER_NUM 1

#define WIDGET_NUM 18

#define TIMER_POS 204

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

static void jog_key_right (void *p_arg);
static void jog_key_left (void *p_arg);
static void jog_key_up (void *p_arg);
static void jog_key_down (void *p_arg);
static void jog_key_enter (void *p_arg);
static void jog_key_esc (void *p_arg);
static void jog_key_release (void *p_arg);
static void jog_key_zdown (void *p_arg);
static void jog_key_zup (void *p_arg);

static void warning_zerarpeca_callback(warn_btn_t btn_type);
static void warning_semzeromaquina_callback(warn_btn_t btn_type);
static void warning_limites_callback(warn_btn_t btn_type);


/* Static variables and const */
static mn_widget_t btn_cima = {.name = "b0", .selectable = true};
static mn_widget_t btn_baixo = {.name = "b1", .selectable = true};
static mn_widget_t btn_direita = {.name = "b2", .selectable = true};
static mn_widget_t btn_esquerda = {.name = "b3", .selectable = true};
static mn_widget_t btn_zup = {.name = "b7", .selectable = true};
static mn_widget_t btn_zdown = {.name = "b8", .selectable = true};
static mn_widget_t btn_tocha = {.name = "p0", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b4", .selectable = true};
static mn_widget_t btn_zpeca = {.name = "b6", .selectable = true};

static mn_widget_t posx_txt = {.name = "t0", .selectable = false};
static mn_widget_t posy_txt = {.name = "t1", .selectable = false};
static mn_widget_t posz_txt = {.name = "t2",.selectable = false};
static mn_widget_t vel_txt = {.name = "t3", .selectable = true};
static mn_widget_t thcReal_txt = {.name = "t4", .selectable = false};

static mn_widget_t tocha_Led = {.name = "p1", .selectable = false};
static mn_widget_t arcook_Led = {.name = "p2", .selectable = false};
static mn_widget_t ohm_Led = {.name = "p3", .selectable = false};
static mn_widget_t emg_Led = {.name = "p8", .selectable = false};
static mn_widget_t lim1_Led = {.name = "p17", .selectable = false};
static mn_widget_t lim2_Led = {.name = "p19", .selectable = false};
static mn_screen_event_t jog;

static mn_keypad_t jog_keypad_args;

static mn_warning_t warn_zerarpeca_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_ZERO_PECA,
											.msg_count = 1,
											.func_callback = warning_zerarpeca_callback
										   };
static mn_warning_t warn_semzeromaquina_args = { .buttonUseInit = BTN_OK,
											.img_txt[0] = IMG_SEM_ZERO_MAQ,
											.msg_count = 1,
											.func_callback = warning_semzeromaquina_callback
										   };

static mn_warning_t warn_limites_args = { .buttonUseInit = BTN_OK,
											.img_txt[0] = IMG_LIMITES,
											.msg_count = 1,
											.func_callback = warning_limites_callback
										   };

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_cima,&btn_baixo,&btn_direita,&btn_esquerda,&btn_zup,&btn_zdown,&btn_tocha,&btn_voltar,&btn_zpeca,
		&posx_txt,&posy_txt,&posz_txt,&vel_txt,&thcReal_txt,&tocha_Led,&arcook_Led,&ohm_Led
};

static mn_timer_t timer0 = {.id = TIMER_POS, .name = "tpos"};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t jog_page = {.id 		 = SC_PAGE6,
					.wt_selected = 0,
					.name        = "jog",
					.p_widget = p_widget,
#if (TIMER_NUM > 0)
					.p_timer = p_timer,
#else
					.p_timer = NULL,
#endif
					.widgetSize = WIDGET_NUM,
					.iif_func 	 = {
										[SC_HANDLER] = page_handler,
										[SC_ATTACH] = page_attach,
										[SC_DETACH] = page_detach,
									}};
/* extern variables */
extern uint32_t JogkeyPressed;
/************************** Static functions *********************************************/
static void jog_key_right (void *p_arg)
{
	//widgetClick(&btn_direita, NT_PRESS);
	btn_direita.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_direita.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_left (void *p_arg)
{
	//widgetClick(&btn_esquerda, NT_PRESS);
	btn_esquerda.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_esquerda.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_up (void *p_arg)
{
	//widgetClick(&btn_cima, NT_PRESS);
	btn_cima.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_cima.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_down (void *p_arg)
{
	//widgetClick(&btn_baixo, NT_PRESS);
	btn_baixo.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_baixo.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_zdown (void *p_arg)
{
	//widgetClick(&btn_zdown, NT_PRESS);
	btn_zdown.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_zdown.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_zup (void *p_arg)
{
	//widgetClick(&btn_zup, NT_PRESS);
	btn_zup.click = NT_PRESS;
	jog.event = EVENT_SIGNAL(btn_zup.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_enter (void *p_arg)
{
	widgetClick(&btn_tocha, NT_PRESS);
	jog.event = EVENT_SIGNAL(btn_tocha.id, EVENT_CLICK);
	xQueueSend( menu.qEvent, &jog, 0 );
}

static void jog_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void jog_key_release (void *p_arg)
{

	if (btn_cima.click == NT_PRESS)
	{
		btn_cima.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_cima.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_baixo.click == NT_PRESS)
	{
		btn_baixo.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_baixo.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_direita.click == NT_PRESS)
	{
		btn_direita.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_direita.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_esquerda.click == NT_PRESS)
	{
		btn_esquerda.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_esquerda.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_zup.click == NT_PRESS)
	{
		btn_zup.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_zup.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_zdown.click == NT_PRESS)
	{
		btn_zdown.click = NT_RELEASE;
		jog.event = EVENT_SIGNAL(btn_zdown.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}
	if (btn_voltar.click == NT_PRESS)
	{
		widgetClick(&btn_voltar, NT_RELEASE);
		jog.event = EVENT_SIGNAL(btn_voltar.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &jog, 0 );
	}

}

/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	//widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	jog_page.iif_func[SC_KEY_ENTER] = jog_key_enter;
	jog_page.iif_func[SC_KEY_ESC] = jog_key_esc;
	jog_page.iif_func[SC_KEY_DOWN] = jog_key_down;
	jog_page.iif_func[SC_KEY_UP] = jog_key_up;
	jog_page.iif_func[SC_KEY_RIGHT] = jog_key_right;
	jog_page.iif_func[SC_KEY_LEFT] = jog_key_left;
	jog_page.iif_func[SC_KEY_ZDOWN] = jog_key_zdown;
	jog_page.iif_func[SC_KEY_ZUP] = jog_key_zup;
	jog_page.iif_func[SC_KEY_RELEASE] = jog_key_release;
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
	static mc_torch_state_t torch;
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW ||
		p_page_hdl->event == EMERGENCIA_EVENT)
	{
		char textstr[20];
		machine_info_update(AXIS_X_INFO,textstr);
		changeTxt(&posx_txt,textstr);
		machine_info_update(AXIS_Y_INFO,textstr);
		changeTxt(&posy_txt,textstr);
		machine_info_update(AXIS_Z_INFO,textstr);
		changeTxt(&posz_txt,textstr);
		machine_info_update(THC_REAL_INFO,textstr);
		changeTxt(&thcReal_txt,textstr);
		machine_info_update(VELOCIDADE_JOG,textstr);
		changeTxt(&vel_txt,textstr);

		torch = MC_TORCH_OFF;
		machine_torch_state(torch);
		widgetChangePic(&btn_tocha, IMG_TOCHA_OFF,NO_IMG);
		machine_jog_start();
		mn_screen_create_timer(&timer0,200);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == LIMITES_EVENT)
	{
		machine_jog_pause();
		machine_torch_state(MC_TORCH_OFF);
		warning_page.p_args = &warn_limites_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_cima.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_UP;
		machine_jog(AXIS_Y, DIR_FORWARD);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_baixo.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_DOWN;
		machine_jog(AXIS_Y, DIR_REVERSE);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_direita.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_RIGHT;
		machine_jog(AXIS_X, DIR_FORWARD);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_esquerda.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_LEFT;
		machine_jog(AXIS_X, DIR_REVERSE);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zup.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_Z_UP;
		machine_jog(AXIS_Z, DIR_FORWARD);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zdown.id,EVENT_PRESSED))
	{
		JogkeyPressed = KEY_Z_DOWN;
		machine_jog(AXIS_Z, DIR_REVERSE);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_cima.id,EVENT_CLICK)     ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_baixo.id,EVENT_CLICK)    ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_direita.id,EVENT_CLICK)  ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_esquerda.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_zup.id,EVENT_CLICK)    ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_zdown.id,EVENT_CLICK))
	{
		JogkeyPressed = 0;
		machine_jog_pause();
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_tocha.id,EVENT_CLICK))
	{
		if (torch == MC_TORCH_ON)
		{
			torch = MC_TORCH_OFF;
			widgetChangePic(&btn_tocha, IMG_TOCHA_OFF,NO_IMG);
		}
		else
		{
			torch = MC_TORCH_ON;
			widgetChangePic(&btn_tocha, IMG_TOCHA_ON,NO_IMG);
		}
		machine_torch_state(torch);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zpeca.id,EVENT_CLICK))
	{
		warning_page.p_args = &warn_zerarpeca_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(vel_txt.id,EVENT_CLICK))
	{
		jog_keypad_args.p_var = &configVarJog[JOG_RAPIDO];
		jog_keypad_args.step = 1;
		jog_keypad_args.min = 10;
		jog_keypad_args.max = 10000;
		jog_keypad_args.p_ret_page = page;
		jog_keypad_args.p_next_page = page;
		keypad_page.p_args = &jog_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		JogkeyPressed = 0;
		machine_jog_stop();
		machine_torch_state(MC_TORCH_OFF);
		mn_screen_change(&manual_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		machine_torch_state(MC_TORCH_OFF);
		JogkeyPressed = 0;
		machine_jog_pause();
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(timer0.id,EVENT_TIMER))
		{
			char textstr[20];
			machine_info_update(AXIS_X_INFO,textstr);
			changeTxt(&posx_txt,textstr);
			machine_info_update(AXIS_Y_INFO,textstr);
			changeTxt(&posy_txt,textstr);
			machine_info_update(AXIS_Z_INFO,textstr);
			changeTxt(&posz_txt,textstr);
			machine_info_update(THC_MANUAL_INFO,textstr);
			changeTxt(&thcReal_txt,textstr);
			machine_info_update(VELOCIDADE_JOG,textstr);
			changeTxt(&vel_txt,textstr);

			if (machine_alarms_get(TORCH_INFO))
				widgetChangePic(&tocha_Led, IMG_LED_ON,NO_IMG);
			else
				widgetChangePic(&tocha_Led, IMG_LED_OFF,NO_IMG);

			if (machine_alarms_get(MATERIAL_INFO))
				widgetChangePic(&ohm_Led, IMG_LED_ON,NO_IMG);
			else
				widgetChangePic(&ohm_Led, IMG_LED_OFF,NO_IMG);

			if (machine_alarms_get(ARCOOK_MANUAL_INFO))
				widgetChangePic(&arcook_Led, IMG_LED_ON,NO_IMG);
			else
				widgetChangePic(&arcook_Led, IMG_LED_OFF,NO_IMG);

			if (machine_alarms_get(EMG_INFO))
				widgetChangePic(&emg_Led, IMG_LED_OFF,NO_IMG);
			else
				widgetChangePic(&emg_Led, IMG_LED_ON,NO_IMG);

			if (machine_alarms_get(LIM1_INFO))
				widgetChangePic(&lim1_Led, IMG_LED_ON,NO_IMG);
			else
				widgetChangePic(&lim1_Led, IMG_LED_OFF,NO_IMG);

			if (machine_alarms_get(LIM2_INFO))
				widgetChangePic(&lim2_Led, IMG_LED_ON,NO_IMG);
			else
				widgetChangePic(&lim2_Led, IMG_LED_OFF,NO_IMG);
		}
}

static void warning_zerarpeca_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: machine_zerar_peca(); break;
		case BTN_PRESSED_NAO: break;
	}
	mn_screen_change(&jog_page,EVENT_SHOW);
}

static void warning_semzeromaquina_callback(warn_btn_t btn_type)
{
	mn_screen_change(&jog_page,EVENT_SHOW);
}

static void warning_limites_callback(warn_btn_t btn_type)
{
	mn_screen_change(&jog_page,EMERGENCIA_EVENT);
}
