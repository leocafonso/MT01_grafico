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

#define WIDGET_NUM 2

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_sim = {.name = "b0", .selectable = true};
static mn_widget_t btn_ok = {.name = "b1", .selectable = true};
static mn_widget_t btn_nao = {.name =  "b2", .selectable = true};
static mn_widget_t msg_pic = {.name =  "p0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_ok,&msg_pic
};
static mn_screen_t *p_previous_page;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer_warning};
#endif
/* Global variables and const */
mn_emergencia_t emergencia_args;

mn_screen_t emergencia_page = {.id 		 = SC_PAGE1,
					.wt_selected = 0,
					.name        = "warning",
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
extern TaskHandle_t xEmergenciaTaskHandle;
/************************** Static functions *********************************************/
static void emergencia_key_enter (void *p_arg)
{
	widgetClick(&btn_ok, NT_PRESS);
}

static void emergencia_key_esc (void *p_arg)
{
	widgetClick(&btn_ok, NT_PRESS);
}

static void emergencia_key_release (void *p_arg)
{
	uint32_t *key_pressed = p_arg;
	mn_screen_event_t touch;
	if (*key_pressed == KEY_ENTER)
	{
		widgetClick(&btn_ok, NT_RELEASE);
		touch.event = EVENT_SIGNAL(btn_ok.id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
	else if (*key_pressed == KEY_ESC)
	{
		widgetClick(&btn_ok, NT_RELEASE);
		touch.event = EVENT_SIGNAL(btn_ok.id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}

}
/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	mn_screen_t *p_page_hdl = p_arg;
	mn_emergencia_t *p_emg_arg = p_page_hdl->p_args;
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	p_previous_page = p_emg_arg->p_ret_page;
	emergencia_page.iif_func[SC_KEY_ENTER] = emergencia_key_enter;
	emergencia_page.iif_func[SC_KEY_ESC] = emergencia_key_esc;
	emergencia_page.iif_func[SC_KEY_DOWN] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_UP] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_RIGHT] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_LEFT] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_ZDOWN] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_ZUP] = mn_screen_idle;
	emergencia_page.iif_func[SC_KEY_RELEASE] = emergencia_key_release;
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
		widgetChangePic(&msg_pic, IMG_EMERGENCIA,NO_IMG);
		widgetVisible(&msg_pic, NT_SHOW);
		widgetTouchable(&btn_sim, NT_DISABLE);
		widgetTouchable(&btn_nao, NT_DISABLE);
		widgetTouchable(&btn_ok, NT_ENABLE);
		widgetVisible(&btn_sim, NT_HIDE);
		widgetVisible(&btn_nao, NT_HIDE);
		widgetVisible(&btn_ok, NT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_ok.id, EVENT_CLICK))
	{
		xTaskNotifyGive( xEmergenciaTaskHandle );
		mn_screen_change(p_previous_page,EMERGENCIA_EVENT);
	}
}
