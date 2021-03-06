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

#define TIME_SCREENS 2000

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_desloca_callback(warn_btn_t btn_type);
static void warning_limites_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_rodar = {.name = "b1", .selectable = true};
static mn_widget_t btn_sim = {.name = "b2", .selectable = true};
//static mn_widget_t btn_deslocar = {.name = "b3", .selectable = true};
static mn_widget_t btn_sel = {.name = "b4", .selectable = true};
static mn_widget_t btn_limites = {.name = "b5", .selectable = true};
static mn_widget_t btn_voltar = {.name = "b0", .selectable = true};

static mn_warning_t warn_args;

static mn_keypad_t selLines_keypad_args;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_rodar,&btn_sim,&btn_sel,&btn_limites,&btn_voltar
};

mn_file_var_t  g_runCase;
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
static void auto_key_esc (void *p_arg)
{
	widgetClick(&btn_voltar, NT_PRESS);
}

static void auto_key_release (void *p_arg)
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
	auto_page.iif_func[SC_KEY_ESC] = auto_key_esc;
	auto_page.iif_func[SC_KEY_RELEASE] = auto_key_release;
}

void page_detach (void *p_arg)
{
#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page->p_timer[i]);
#endif
//	mn_screen_bind_keyboard(page);
}

void page_handler (void *p_arg)
{
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		page->wt_selected = mn_screen_select_widget(page,&btn_rodar);
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
			g_runCase = FILE_AUTO;
			mn_screen_change((machine_flag_get(MODOMAQUINA) ? (&fInfoOX_page) : (&fInfoPl_page)) ,EVENT_SHOW);
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
			g_runCase = FILE_SIM;
			mn_screen_change((machine_flag_get(MODOMAQUINA) ? (&fInfoOX_page) : (&fInfoPl_page)) ,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_sel.id,EVENT_CLICK))
	{
		selLines_keypad_args.p_var = &selecionarLinhas;
		selLines_keypad_args.step = 1;
		selLines_keypad_args.min = 0;
		selLines_keypad_args.max = selecionarlinhasMax();
		selLines_keypad_args.p_ret_page = page;
		selLines_keypad_args.p_next_page = &selLines_page;
		keypad_page.p_args = &selLines_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_limites.id,EVENT_CLICK))
	{
		warn_args.buttonUseInit = BTN_ASK;
		warn_args.img_txt[0] = IMG_CONTINUAR;
		warn_args.msg_count = 1;
		warn_args.func_callback = warning_limites_callback;
		warning_page.p_args = &warn_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
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

static void warning_desloca_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
	case BTN_PRESSED_SIM: mn_screen_change(&desloca_page,EVENT_SHOW);break;
	case BTN_PRESSED_NAO: mn_screen_change(&auto_page,EVENT_SHOW);break;
	}
}

static void warning_limites_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
	case BTN_PRESSED_SIM: mn_screen_change(&limite_page,EVENT_SHOW);break;
	case BTN_PRESSED_NAO: mn_screen_change(&auto_page,EVENT_SHOW);break;
	}
}
