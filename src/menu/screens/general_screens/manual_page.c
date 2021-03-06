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

#define TIMER_NUM 0

#define WIDGET_NUM 6

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_zerarmaquina_callback(warn_btn_t btn_type);
static void warning_zerarpeca_callback(warn_btn_t btn_type);
static void warning_semzeromaquina_callback(warn_btn_t btn_type);
static void warning_desloca_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_manual = {.name = "b1", .selectable = true};
static mn_widget_t btn_vel_manual = {.name = "b2", .selectable = true};
static mn_widget_t btn_zerar_peca = {.name = "b3", .selectable = true};
static mn_widget_t btn_deslocar = {.name = "b4", .selectable = true};
static mn_widget_t btn_zerar_maq = {.name = "b5", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_warning_t warn_zerarmaquina_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_CONTINUAR,
											.msg_count = 1,
											.func_callback = warning_zerarmaquina_callback};

static mn_warning_t warn_zerarpeca_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_ZERO_PECA,
											.msg_count = 1,
											.func_callback = warning_zerarpeca_callback
										   };
static mn_warning_t warn_semzeromaquina_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_SEM_ZERO_MAQ,
											.msg_count = 1,
											.func_callback = warning_semzeromaquina_callback
										   };

static mn_warning_t warn_args;

static mn_keypad_t vel_keypad_args;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_manual,&btn_vel_manual,&btn_deslocar,&btn_zerar_peca,&btn_zerar_maq,&btn_voltar
};

static uint8_t warning_case;

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
extern bool zeromaq_flag;
/************************** Static functions *********************************************/
static void manual_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void manual_key_release (void *p_arg)
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
		page->wt_selected = mn_screen_select_widget(page,&btn_manual);

	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_manual.id,EVENT_CLICK))
	{
		mn_screen_change(&jog_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_vel_manual.id,EVENT_CLICK))
	{
		vel_keypad_args.p_var = &configVarJog[JOG_RAPIDO];
		vel_keypad_args.step = 1;
		vel_keypad_args.min = 10;
		vel_keypad_args.max = 10000;
		vel_keypad_args.p_ret_page = page;
		vel_keypad_args.p_next_page = page;
		keypad_page.p_args = &vel_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_deslocar.id,EVENT_CLICK))
	{
		if (zeromaq_flag == true)
		{
			xio_close(cs.primary_src);
			warn_args.buttonUseInit = BTN_ASK;
			warn_args.img_txt[0] = IMG_CONTINUAR;
			warn_args.msg_count = 1;
			warn_args.func_callback = warning_desloca_callback;
			warning_page.p_args = &warn_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
		}
		else
		{
			warning_case = 0;
			warning_page.p_args = &warn_semzeromaquina_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zerar_maq.id,EVENT_CLICK))
	{
		warning_page.p_args = &warn_zerarmaquina_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zerar_peca.id,EVENT_CLICK))
	{
		if (zeromaq_flag == true)
		{
			warning_page.p_args = &warn_zerarpeca_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
		}
		else
		{
			warning_case = 1;
			warning_page.p_args = &warn_semzeromaquina_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_voltar.id,EVENT_CLICK))
	{
		mn_screen_change(&main_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}

static void warning_zerarmaquina_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: mn_screen_change(&zerarmaq_page,EVENT_SHOW);break;
		case BTN_PRESSED_NAO: mn_screen_change(&manual_page,EVENT_SHOW);break;
	}
}

static void warning_zerarpeca_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: machine_zerar_peca(); break;
		case BTN_PRESSED_NAO: break;
	}
	mn_screen_change(&manual_page,EVENT_SHOW);
}

static void warning_semzeromaquina_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM:
			if (warning_case == 0)
			{
				xio_close(cs.primary_src);
				warn_args.buttonUseInit = BTN_ASK;
				warn_args.img_txt[0] = IMG_CONTINUAR;
				warn_args.msg_count = 1;
				warn_args.func_callback = warning_desloca_callback;
				warning_page.p_args = &warn_args;
				mn_screen_change(&warning_page,EVENT_SHOW);
			}
			else if (warning_case == 1)
			{
				warning_page.p_args = &warn_zerarpeca_args;
				mn_screen_change(&warning_page,EVENT_SHOW);
			}
			break;
		case BTN_PRESSED_NAO: mn_screen_change(&manual_page,EVENT_SHOW); break;
	}
}

static void warning_desloca_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM: mn_screen_change(&desloca_page,EVENT_SHOW);break;
		case BTN_PRESSED_NAO: mn_screen_change(&manual_page,EVENT_SHOW);break;
	}
}
