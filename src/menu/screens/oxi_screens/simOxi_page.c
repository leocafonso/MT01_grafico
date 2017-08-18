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

#include "pages_includes.h"
#include "config_menu_ox.h"
/* Defines */

#define TIMER_NUM 1

#define WIDGET_NUM 18

#define TIMER_POS 207

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_callback(warn_btn_t btn_type);
static void warning_esc_callback(warn_btn_t btn_type);
static void warning_limites_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_play = {.name = "b1", .selectable = true};
static mn_widget_t btn_volta = {.name = "b0", .selectable = true};
static mn_widget_t btn_zup = {.name = "b2", .selectable = true};
static mn_widget_t btn_zdown = {.name = "b3", .selectable = true};
static mn_widget_t btn_timeup = {.name = "b4", .selectable = true};
static mn_widget_t btn_timefw = {.name = "b7", .selectable = true};
static mn_widget_t btn_timedown = {.name = "b6", .selectable = true};
static mn_widget_t btn_auto = {.name = "b5", .selectable = true};

static mn_widget_t file_txt = {.name = "t0", .selectable = false};
static mn_widget_t posx_txt = {.name = "t1", .selectable = false};
static mn_widget_t posy_txt = {.name = "t2", .selectable = false};
static mn_widget_t posz_txt = {.name = "t3",.selectable = false};
static mn_widget_t vel_txt = {.name = "t5", .selectable = false};
static mn_widget_t line_txt = {.name = "t4", .selectable = false};
static mn_widget_t thcReal_txt = {.name = "t6", .selectable = false};

static mn_widget_t arcook_Led = {.name = "p1", .selectable = false};
static mn_widget_t tocha_Led = {.name = "p0", .selectable = false};
static mn_widget_t ohm_Led = {.name = "p2", .selectable = false};
static mn_warning_t warn_args;
static mn_warning_t warn_esc_args = { .buttonUseInit = BTN_ASK,
											.img_txt[0] = IMG_SAIR,
											.msg_count = 1,
											.func_callback = warning_esc_callback};

static mn_warning_t warn_limites_args = { .buttonUseInit = BTN_OK,
											.img_txt[0] = IMG_LIMITES,
											.msg_count = 1,
											.func_callback = warning_limites_callback
										   };
static uint32_t event_args;
static uint32_t btn_id;
static bool machine_is_paused = false;
static uint8_t programEnd;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_play,&btn_volta,&btn_zup,&btn_zdown,&btn_timeup,&btn_timedown,&btn_timefw,&btn_auto,
		&file_txt,&posx_txt,&posy_txt,&posz_txt,&vel_txt,&line_txt,&thcReal_txt,&arcook_Led,&tocha_Led,&ohm_Led
};

static mn_timer_t timer0 = {.id = TIMER_POS, .name = "tpos"};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t simOxi_page = {.id 		 = SC_PAGE7,
					.wt_selected = 0,
					.name        = "simOxi",
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
extern float zmove;
/************************** Static functions *********************************************/
static void sim_key_zdown (void *p_arg)
{
	mn_screen_event_t touch;
	touch.event = EVENT_SIGNAL(btn_zdown.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &touch, 0 );
}

static void sim_key_zup (void *p_arg)
{
	mn_screen_event_t touch;
	touch.event = EVENT_SIGNAL(btn_zup.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &touch, 0 );
}

static void sim_key_right (void *p_arg)
{
	if (machine_is_paused == true)
	{
		if (page->wt_selected == mn_screen_select_widget(page,&btn_play))
		{
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
			page->wt_selected = mn_screen_select_widget(page,&btn_auto);
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
		}
	}
}

static void sim_key_left (void *p_arg)
{
	if (machine_is_paused == true)
	{
		if (page->wt_selected == mn_screen_select_widget(page,&btn_auto))
		{
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
			page->wt_selected = mn_screen_select_widget(page,&btn_play);
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
		}
	}
}

static void sim_key_release (void *p_arg)
{
	uint32_t *key_pressed = p_arg;
	mn_screen_event_t touch;
	if (*key_pressed == KEY_ENTER)
	{
		if (machine_is_paused == true)
		{
			touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id, EVENT_CLICK);
		}
		else
		{
			touch.event = EVENT_SIGNAL(btn_auto.id,EVENT_CLICK);
		}
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_ESC)
	{
		if (machine_is_paused == false)
		{
			touch.event = EVENT_SIGNAL(btn_play.id,EVENT_CLICK);
		}
		else
		{
			touch.event = EVENT_SIGNAL(btn_volta.id,EVENT_CLICK);
		}
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_Z_UP)
	{
		touch.event = EVENT_SIGNAL(btn_zup.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_Z_DOWN)
	{
		touch.event = EVENT_SIGNAL(btn_zdown.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
}

/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	simOxi_page.iif_func[SC_KEY_ENTER] = mn_screen_idle;
	simOxi_page.iif_func[SC_KEY_ESC] = mn_screen_idle;
	simOxi_page.iif_func[SC_KEY_DOWN] = mn_screen_idle;
	simOxi_page.iif_func[SC_KEY_UP] = mn_screen_idle;
	simOxi_page.iif_func[SC_KEY_RIGHT] = sim_key_right;
	simOxi_page.iif_func[SC_KEY_LEFT] = sim_key_left;
	simOxi_page.iif_func[SC_KEY_ZDOWN] = sim_key_zdown;
	simOxi_page.iif_func[SC_KEY_ZUP] = sim_key_zup;
	simOxi_page.iif_func[SC_KEY_RELEASE] = sim_key_release;
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
	uint8_t programEnd = 0;
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		spiffs_stat fileStat;
		xio_open(cs.primary_src,0,0);
		SPIFFS_fstat(&uspiffs[0].gSPIFFS, uspiffs[0].f, &fileStat);
		changeTxt(&file_txt,(const char *)fileStat.name);
		xio_close(cs.primary_src);
		machine_start_sim();
		machine_is_paused = false;
		programEnd = 0;
		widgetChangePic(&btn_play, IMG_BTN_PAUSE,IMG_BTN_PAUSE_PRESS);
		mn_screen_create_timer(&timer0,300);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == EMERGENCIA_EVENT)
	{
		machine_is_paused = true;
		widgetChangePic(&btn_play, IMG_BTN_PLAY,IMG_BTN_PLAY_PRESS);
		mn_screen_create_timer(&timer0,300);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == LIMITES_EVENT)
	{
		widgetChangePic(&btn_play, IMG_BTN_PLAY,IMG_BTN_PLAY_PRESS);
		machine_is_paused = true;
		machine_pause();
		warning_page.p_args = &warn_limites_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_play.id,EVENT_CLICK))
	{
		if (machine_is_paused == false)
		{
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
			page->wt_selected = mn_screen_select_widget(page,&btn_play);
			widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
			machine_pause();
			widgetChangePic(&btn_play, IMG_BTN_PLAY,IMG_BTN_PLAY_PRESS);
			machine_is_paused = true;
		}
		else
		{
			machine_restart();
			widgetChangePic(&btn_play, IMG_BTN_PAUSE,IMG_BTN_PAUSE_PRESS);
			machine_is_paused = false;
		}

	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zup.id,EVENT_PRESSED) ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_zdown.id,EVENT_PRESSED))
	{
		btn_id = GET_ID(p_page_hdl->event);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_zup.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(btn_zdown.id,EVENT_CLICK))
	{
		zmove = 0;
		btn_id = 0;
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_auto.id,EVENT_CLICK))
	{
		if (machine_is_paused == true)
		{
			machine_restart();
		}
		machine_sim_to_auto(NULL);
		mn_screen_change(&cutOxi_page,SIM_ENTRY_EVENT);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_volta.id,EVENT_CLICK))
	{
//		if (!programEnd)
//			machine_stop(programEnd);
//		mn_screen_change(&auto_page,EVENT_SHOW);
		warning_page.p_args = &warn_esc_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == MATERIAL_FAILED_EVENT)
	{
		event_args = MATERIAL_FAILED_EVENT;
		machine_pause_ohmico();
		warn_args.buttonUseInit = BTN_OK;
		warn_args.img_txt[0] = IMG_OHMICO;
		warn_args.msg_count = 1;
		warn_args.func_callback = warning_callback;
		warning_page.p_args = &warn_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == PROGRAM_FINISHED_EVENT)
	{
		programEnd = 1;
		machine_is_paused = false;
		event_args = PROGRAM_FINISHED_EVENT;
		machine_stop(programEnd);
		warn_args.buttonUseInit = BTN_OK;
		warn_args.img_txt[0] = IMG_FINALIZADO;
		warn_args.msg_count = 1;
		warn_args.func_callback = warning_callback;
		warning_page.p_args = &warn_args;
		mn_screen_change(&warning_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		if (machine_is_paused == false)
		{
			machine_pause();
			widgetChangePic(&btn_play, IMG_BTN_PLAY,IMG_BTN_PLAY_PRESS);
			machine_is_paused = true;
		}
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
		machine_info_update(LINE_INFO,textstr);
		changeTxt(&line_txt,textstr);
		machine_info_update(TIMER_REAL_OXI,textstr);
		changeTxt(&thcReal_txt,textstr);
		machine_info_update(VELOCIDADE_INFO,textstr);
		changeTxt(&vel_txt,textstr);

		if (machine_alarms_get(TORCH_INFO))
			widgetChangePic(&tocha_Led, IMG_LED_ON,NO_IMG);
		else
			widgetChangePic(&tocha_Led, IMG_LED_OFF,NO_IMG);


		if (btn_id == btn_zup.id)
		{
			zmove = 0.005;
		}
		else if (btn_id == btn_zdown.id)
		{
			zmove = -0.005;
		}
//		else if (btn_id == btn_timeup.id)
//		{
//			machine_oxi_timer_up();
//		}
//		else if (btn_id == btn_timedown.id)
//		{
//			machine_oxi_timer_down();
//		}
	}
}

static void warning_callback(warn_btn_t btn_type)
{
	if (event_args == PROGRAM_FINISHED_EVENT)
	{

		mn_screen_change(&jog_page,EVENT_SHOW);
	}
	else if (event_args == MATERIAL_FAILED_EVENT)
	{
		mn_screen_change(&simOxi_page,EMERGENCIA_EVENT);
	}
}

static void warning_esc_callback(warn_btn_t btn_type)
{
	switch (btn_type)
	{
		case BTN_PRESSED_SIM:
			if (!programEnd)
			{
				machine_pause();
				machine_stop(programEnd);
			}
			mn_screen_change(&auto_page,EVENT_SHOW);
		break;
		case BTN_PRESSED_NAO: 	mn_screen_change(&simOxi_page,EMERGENCIA_EVENT);break;
	}
}

static void warning_limites_callback(warn_btn_t btn_type)
{
	mn_screen_change(&simOxi_page,EMERGENCIA_EVENT);
}
