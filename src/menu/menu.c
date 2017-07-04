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
#include "screens/load_page.h"
/* Defines */

/* Static functions */

/* Static variables and const */

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
    if(spiffs_init() == SPIFFS_ERR_NOT_A_FS)
    {
    	spiffs_format();
    }
	mn_screen_bind_keyboard(&splash_page);
	mn_screen_bind_keyboard(&warning_page);
	mn_screen_bind_keyboard(&emergencia_page);
	mn_screen_bind_keyboard(&main_page);
	mn_screen_bind_keyboard(&auto_page);
	mn_screen_bind_keyboard(&selFiles_page);
	mn_screen_bind_keyboard(&cutting_page);
//	mn_screen_bind_keyboard(&jog_page);
	mn_screen_bind_keyboard(&desloca_page);
	mn_screen_bind_keyboard(&sim_page);
	mn_screen_bind_keyboard(&keypad_page);
	mn_screen_bind_keyboard(&cfgCorte_page);
	mn_screen_bind_keyboard(&load_page);
	mn_screen_bind_keyboard(&selLines_page);
	mn_screen_bind_keyboard(&limite_page);
	mn_screen_bind_keyboard(&fileInfo_page);

	screenGetWidgetsInfo(&splash_page);
	screenGetWidgetsInfo(&warning_page);
	screenGetWidgetsInfo(&emergencia_page);
	screenGetWidgetsInfo(&selFiles_page);
	screenGetWidgetsInfo(&main_page);
	screenGetWidgetsInfo(&auto_page);
	screenGetWidgetsInfo(&cutting_page);
	screenGetWidgetsInfo(&jog_page);
	screenGetWidgetsInfo(&desloca_page);
	screenGetWidgetsInfo(&sim_page);
	screenGetWidgetsInfo(&keypad_page);
	screenGetWidgetsInfo(&cfgCorte_page);
	screenGetWidgetsInfo(&load_page);
	screenGetWidgetsInfo(&selLines_page);
	screenGetWidgetsInfo(&limite_page);
	screenGetWidgetsInfo(&fileInfo_page);

	vTaskDelay(500/portTICK_PERIOD_MS);
    xTaskCreate( (pdTASK_CODE)keyboard_task,     "keyboard_task    ",  512, NULL, 2, NULL); /* keyboard_task      */
	if(R_IsFileLoaderAvailable())
	{
		loadfilesNum |= MCU_FILE;
	}
	if (NexUpload_checkFile("MT01_5in_regular.tft"))
	{
		loadfilesNum |= NEXTION_FILE;
	}
	if(loadfilesNum > 0)
	{
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
		if (event.event == EMERGENCIA_SIGNAL_EVENT)
		{
			mn_screen_change(&emergencia_page,EVENT_SHOW);
		}
		else
		{
			page->iif_func[SC_HANDLER](&event);
		}
	}
}

