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

#include "state_functions.h"

/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 3

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_volta = {.name = "b0", .selectable = true};
static mn_widget_t btn_line1 = {.name = "b1", .selectable = true};
static mn_widget_t btn_line2 = {.name =  "b2", .selectable = true};
static mn_widget_t txt_Select = {.name =  "p0", .selectable = false};
static mn_widget_t txt_info = {.name =  "t0", .selectable = false};

static mn_keypad_t selLines_keypad_args;

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_volta,&btn_line1,&btn_line2
};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer_warning};
#endif
/* Global variables and const */
mn_screen_t selLines_page = {.id 		 = SC_PAGE12,
					.wt_selected = 0,
					.name        = "SelLines",
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
extern uint32_t choosedLine;
extern uint32_t choosedLinePosition;
/************************** Static functions *********************************************/

/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
}

void page_detach (void *p_arg)
{
#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page->p_timer[i]);
#endif
}
uint32_t line_M5_num[2];
uint32_t linepos_M5_num[2];
void page_handler (void *p_arg)
{
	char str[50];
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		line_M5_num[0] = 0;
		line_M5_num[1] = 0;
		widgetVisible(&btn_line1, NT_HIDE);
		widgetVisible(&btn_line2, NT_HIDE);
		widgetVisible(&btn_volta, NT_HIDE);
		widgetVisible(&txt_Select, NT_HIDE);
		widgetVisible(&txt_info, NT_SHOW);
		snprintf(str,sizeof(str), "Carregando linha...");
		changeTxt(&txt_info,str);
		selecionarlinhas(line_M5_num,linepos_M5_num);
		widgetVisible(&btn_line1, NT_SHOW);
		widgetVisible(&btn_line2, NT_SHOW);
		widgetVisible(&btn_volta, NT_SHOW);
		widgetVisible(&txt_Select, NT_SHOW);
		widgetVisible(&txt_info, NT_HIDE);
		sprintf(str, "ENTRADA LINHA %d", line_M5_num[0]);
		changeTxt(&btn_line1,str);
		sprintf(str, "ENTRADA LINHA %d", line_M5_num[1]);
		changeTxt(&btn_line2,str);
	}

	else if (p_page_hdl->event == EVENT_SIGNAL(btn_line1.id, EVENT_CLICK))
	{
		choosedLine = line_M5_num[0];
		choosedLinePosition = linepos_M5_num[0];
		mn_screen_change(&auto_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_line2.id, EVENT_CLICK))
	{
		choosedLine = line_M5_num[1];
		choosedLinePosition = linepos_M5_num[1];
		mn_screen_change(&auto_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(btn_volta.id, EVENT_CLICK))
	{
		line_M5_num[0] = 0;
		line_M5_num[1] = 0;
		selLines_keypad_args.p_var = &selecionarLinhas;
		selLines_keypad_args.step = 1;
		selLines_keypad_args.min = 0;
		selLines_keypad_args.max = selecionarlinhasMax();
		selLines_keypad_args.p_ret_page = &auto_page;
		selLines_keypad_args.p_next_page = &selLines_page;
		keypad_page.p_args = &selLines_keypad_args;
		mn_screen_change(&keypad_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}

}
