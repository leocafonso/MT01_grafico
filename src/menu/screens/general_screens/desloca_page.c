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

#define WIDGET_NUM 15

#define TIMER_POS 203

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t btn_play = {.name = "b1", .selectable = true};
//static mn_widget_t btn_stop = {.name = "b1", .selectable = true};
static mn_widget_t btn_volta = {.name = "b0", .selectable = true};
static mn_widget_t btn_thcp = {.name = "b2", .selectable = true};
static mn_widget_t btn_thcm = {.name = "b3", .selectable = true};

static mn_widget_t file_txt = {.name = "t0", .selectable = false};
static mn_widget_t posx_txt = {.name = "t1", .selectable = false};
static mn_widget_t posy_txt = {.name = "t2", .selectable = false};
static mn_widget_t posz_txt = {.name = "t3",.selectable = false};
static mn_widget_t vel_txt = {.name = "t5", .selectable = false};
static mn_widget_t line_txt = {.name = "t4", .selectable = false};
static mn_widget_t thcReal_txt = {.name = "t6", .selectable = false};
static mn_widget_t thcSet_txt = {.name = "t7", .selectable = false};

static mn_widget_t arcook_Led = {.name = "p1", .selectable = false};
static mn_widget_t tocha_Led = {.name = "p0", .selectable = false};
static mn_widget_t ohm_Led = {.name = "p2", .selectable = false};
static mn_warning_t warn_args;
static uint32_t event_args;
static mn_screen_event_t desloca;
static bool machine_is_paused = false;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_play,&btn_volta,&btn_thcp,&btn_thcm,
		&file_txt,&posx_txt,&posy_txt,&posz_txt,&vel_txt,&line_txt,&thcReal_txt,&thcSet_txt,&arcook_Led,&tocha_Led,&ohm_Led,

};

static mn_timer_t timer0 = {.id = TIMER_POS, .name = "tpos"};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
mn_screen_t desloca_page = {.id 		 = SC_PAGE6,
					.wt_selected = 0,
					.name        = "cutPl",
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
static void desloca_key_enter (void *p_arg)
{
	if (machine_is_paused == true)
	{
		widgetClick(&btn_play, NT_PRESS);
		desloca.event = EVENT_SIGNAL(btn_play.id, EVENT_PRESSED);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
}

static void desloca_key_esc (void *p_arg)
{
	if (machine_is_paused == false)
	{
		widgetClick(&btn_play, NT_PRESS);
		desloca.event = EVENT_SIGNAL(btn_play.id, EVENT_PRESSED);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
	else
	{
		widgetClick(&btn_volta, NT_PRESS);
		desloca.event = EVENT_SIGNAL(btn_play.id, EVENT_PRESSED);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
}

static void desloca_key_zdown (void *p_arg)
{
	widgetClick(&btn_thcm, NT_PRESS);
	desloca.event = EVENT_SIGNAL(btn_thcm.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &desloca, 0 );
}

static void desloca_key_zup (void *p_arg)
{
	widgetClick(&btn_thcp, NT_PRESS);
	desloca.event = EVENT_SIGNAL(btn_thcp.id, EVENT_PRESSED);
	xQueueSend( menu.qEvent, &desloca, 0 );
}

static void desloca_key_release (void *p_arg)
{
	if (btn_thcm.click == NT_PRESS)
	{
		widgetClick(&btn_thcm, NT_RELEASE);
		desloca.event = EVENT_SIGNAL(btn_thcm.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
	else if (btn_thcp.click == NT_PRESS)
	{
		widgetClick(&btn_thcp, NT_RELEASE);
		desloca.event = EVENT_SIGNAL(btn_thcp.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
	else if (btn_play.click == NT_PRESS)
	{
		widgetClick(&btn_play, NT_RELEASE);
		desloca.event = EVENT_SIGNAL(btn_play.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}
	else if (btn_volta.click == NT_PRESS)
	{
		widgetClick(&btn_volta, NT_RELEASE);
		desloca.event = EVENT_SIGNAL(btn_volta.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &desloca, 0 );
	}

}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	desloca_page.iif_func[SC_KEY_ENTER] = desloca_key_enter;
	desloca_page.iif_func[SC_KEY_ESC] = desloca_key_esc;
	desloca_page.iif_func[SC_KEY_ZDOWN] = desloca_key_zdown;
	desloca_page.iif_func[SC_KEY_ZUP] = desloca_key_zup;
	desloca_page.iif_func[SC_KEY_RELEASE] = desloca_key_release;
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
		machine_homming_eixos();
		widgetChangePic(&btn_play, IMG_BTN_PAUSE,IMG_BTN_PAUSE_PRESS);
		mn_screen_create_timer(&timer0,200);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == EMERGENCIA_EVENT)
	{
		machine_is_paused = true;
		widgetChangePic(&btn_play, IMG_BTN_PLAY,IMG_BTN_PLAY_PRESS);
		mn_screen_create_timer(&timer0,300);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_play.id,EVENT_CLICK))
	{
		if (machine_is_paused == false)
		{
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
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_volta.id,EVENT_CLICK))
	{
		if (!programEnd)
			machine_stop(programEnd);
		mn_screen_change(&auto_page,EVENT_SHOW);
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
		machine_info_update(THC_REAL_INFO,textstr);
		changeTxt(&thcReal_txt,textstr);
		machine_info_update(THC_SET_INFO,textstr);
		changeTxt(&thcSet_txt,textstr);
		machine_info_update(VELOCIDADE_INFO,textstr);
		changeTxt(&vel_txt,textstr);

		if (machine_alarms_get(TORCH_INFO))
			widgetChangePic(&tocha_Led, IMG_LED_ON,NO_IMG);
		else
			widgetChangePic(&tocha_Led, IMG_LED_OFF,NO_IMG);

		if (machine_alarms_get(MATERIAL_INFO))
			widgetChangePic(&ohm_Led, IMG_LED_ON,NO_IMG);
		else
			widgetChangePic(&ohm_Led, IMG_LED_OFF,NO_IMG);

		if (machine_alarms_get(ARCOOK_INFO))
			widgetChangePic(&arcook_Led, IMG_LED_ON,NO_IMG);
		else
			widgetChangePic(&arcook_Led, IMG_LED_OFF,NO_IMG);
	}
}

static void warning_callback(warn_btn_t btn_type)
{
	if (event_args == PROGRAM_FINISHED_EVENT)
	{
		mn_screen_change(&auto_page,EVENT_SHOW);
	}
}
