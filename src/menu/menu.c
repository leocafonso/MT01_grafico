/** @file widget.c
 *  @brief Function implementation for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

/* Includes */
#include "platform.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"
#include "interpreter_if.h"
#include "spiffs_hw.h"
#include "r_flash_loader_rx_if.h"
#include "load_page.h"
/* Defines */

/* Static functions */

/* Static variables and const */
static mn_load_t load_arg;
/* Global variables and const */
mn_menu_t menu;
/* extern variables */
extern void keyboard_task(void);

/************************** Static functions *********************************************/

/************************** Public functions *********************************************/
void menu_task(void)
{
	mn_screen_event_t event;
	vTaskDelay(1000/portTICK_PERIOD_MS);
	nexInit();
	menu.qEvent = xQueueCreate(10,sizeof(mn_screen_event_t));
//	if (NexUpload_checkFile())
//	{
//		NexUpload_setDownloadBaudrate(1500000);
//		NexUpload_downloadTftFile();
//		NexUpload_waitingReset(10000);
//		if(R_IsFileLoaderAvailable())
//		{
//			while(R_loader_progress() < 16)
//			{
//				WDT_FEED
//			}
//		}
//		RESET
//	}
//	if(R_IsFileLoaderAvailable())
//	{
//		uint8_t val = 0;
//		NexPage_show(load_page.name);
//		NexWidget_visible("p0",NT_HIDE);
//		NexWidget_visible("b0",NT_HIDE);
//		NexWidget_visible("b1",NT_HIDE);
//		NexWidget_visible("j0",NT_SHOW);
//		while(R_loader_progress() < 16)
//		{
//			NexWidget_ProgressBar("j0",val*15);
//			val++;
//		}
//		RESET
//	}

//    if(spiffs_init() == SPIFFS_ERR_NOT_A_FS)
//    {
//    	spiffs_format();
//    }
	mn_screen_bind_keyboard(&splash_page);
	mn_screen_bind_keyboard(&warning_page);
	mn_screen_bind_keyboard(&emergencia_page);
	mn_screen_bind_keyboard(&main_page);
	mn_screen_bind_keyboard(&auto_page);
	mn_screen_bind_keyboard(&selFiles_page);
	mn_screen_bind_keyboard(&cutPl_page);
	mn_screen_bind_keyboard(&cutOxi_page);
//	mn_screen_bind_keyboard(&jog_page);
	mn_screen_bind_keyboard(&desloca_page);
	mn_screen_bind_keyboard(&simPl_page);
	mn_screen_bind_keyboard(&simOxi_page);
	mn_screen_bind_keyboard(&keypad_page);
	mn_screen_bind_keyboard(&cfgCutPl_page);
	mn_screen_bind_keyboard(&cfgCutOx_page);
	mn_screen_bind_keyboard(&load_page);
	mn_screen_bind_keyboard(&selLines_page);
	mn_screen_bind_keyboard(&limite_page);
	mn_screen_bind_keyboard(&fInfoPl_page);
	mn_screen_bind_keyboard(&fInfoOX_page);
	mn_screen_bind_keyboard(&cfgMaq_page);
	mn_screen_bind_keyboard(&manual_page);
	mn_screen_bind_keyboard(&cfgParTHC_page);
	mn_screen_bind_keyboard(&zerarmaq_page);
	mn_screen_bind_keyboard(&cfgMaqPar_page);

	screenGetWidgetsInfo(&splash_page);
	screenGetWidgetsInfo(&warning_page);
	screenGetWidgetsInfo(&emergencia_page);
	screenGetWidgetsInfo(&selFiles_page);
	screenGetWidgetsInfo(&main_page);
	screenGetWidgetsInfo(&auto_page);
	screenGetWidgetsInfo(&cutPl_page);
	screenGetWidgetsInfo(&cutOxi_page);
	screenGetWidgetsInfo(&jog_page);
	screenGetWidgetsInfo(&desloca_page);
	screenGetWidgetsInfo(&simPl_page);
	screenGetWidgetsInfo(&simOxi_page);
	screenGetWidgetsInfo(&keypad_page);
	screenGetWidgetsInfo(&cfgCutPl_page);
	screenGetWidgetsInfo(&cfgCutOx_page);
	screenGetWidgetsInfo(&load_page);
	screenGetWidgetsInfo(&selLines_page);
	screenGetWidgetsInfo(&limite_page);
	screenGetWidgetsInfo(&fInfoPl_page);
	screenGetWidgetsInfo(&fInfoOX_page);
	screenGetWidgetsInfo(&cfgMaq_page);
	screenGetWidgetsInfo(&manual_page);
	screenGetWidgetsInfo(&cfgParTHC_page);
	screenGetWidgetsInfo(&zerarmaq_page);
	screenGetWidgetsInfo(&cfgMaqPar_page);

	vTaskDelay(500/portTICK_PERIOD_MS);
    xTaskCreate( (pdTASK_CODE)keyboard_task,     "keyboard_task    ",  512, NULL, 3, NULL); /* keyboard_task      */

    if(spiffs_init() == SPIFFS_ERR_NOT_A_FS)
    {
    	load_arg.type = MEM_FORMAT;
    	load_page.p_args = &load_arg;
		mn_screen_change(&load_page,EVENT_SHOW);
    }
	else
	{
		mn_screen_change(&splash_page,EVENT_SHOW);
	}
	while(1)
	{
		xQueueReceive(menu.qEvent, &event, portMAX_DELAY);
		event.p_arg = page->p_args;
		page->iif_func[SC_HANDLER](&event);
//		if (event.event == EMERGENCIA_SIGNAL_EVENT)
//		{
//			mn_screen_change(&emergencia_page,EVENT_SHOW);
//		}
//		else
//		{
//			page->iif_func[SC_HANDLER](&event);
//		}
	}
}

