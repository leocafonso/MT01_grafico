/*
 * keyboard.c
 *
 *  Created on: 07/08/2015
 *      Author: LAfonso01
 */

/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
#include "platform.h"
#include "keyboard.h"
//#include "lcd_menu.h"
//#include "hardware.h"
#include "tinyg.h"
#include "config.h"
#include "switch.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"

#ifdef FREE_RTOS_PP
#include "FreeRTOS.h"
#include "task.h"
#endif

void keybord_func(uint16_t keyEntry);
/*****************************************************************************
Macro definitions
 ******************************************************************************/


/******************************************************************************
Section    <Section Definition> , "Data Sections"
 ******************************************************************************/

/******************************************************************************
Private global variables and functions
 ******************************************************************************/


/******************************************************************************
External variables and functions
 ******************************************************************************/


/*****************************************************************************
Enumerated Types
 ******************************************************************************/

/******************************************************************************
Section    <Section Definition> , "Project Sections"
 ******************************************************************************/

/******************************************************************************
Function Name   : keyboard_task
Description     : keyboard scan Task process
Arguments       : none
Return value    : none
 ******************************************************************************/
#define KEY_DEBOUNCE 10
#define KEY_DEBOUNCE_DLYMS 2

void keyboard_task(void)
{
	uint8_t key_buf[3][20];
	uint8_t colPressed = 0xff;
	uint32_t key = 0;
	uint32_t key_old = 0;
	uint8_t i = 0;
	uint8_t k = 0;
	uint8_t j = 0;
	uint8_t matchCount = 0;
	uint8_t col[3] = {KC0,KC1,KC2};
	nt_touch_t btEvent;

	while(1)
	{
		vTaskDelay(KEY_DEBOUNCE_DLYMS / portTICK_RATE_MS);
		if(NexTouch_recv(&btEvent,0))
		{
			mn_screen_event_t touch;
			if (btEvent.eventType == NEX_RET_EVENT_TOUCH_HEAD)
			{
				if (btEvent.event == NT_PRESS)
				{
					touch.event = EVENT_SIGNAL(btEvent.widgetID,EVENT_PRESSED);
				}
				else if (btEvent.event == NT_RELEASE)
				{
					touch.event = EVENT_SIGNAL(btEvent.widgetID,EVENT_CLICK);
				}
			}
			else if (btEvent.eventType == NEX_RET_EVENT_POSITION_HEAD)
			{
				touch.event = PANEL_TOUCH_EVENT;
			}
			xQueueSend( menu.qEvent, &touch, 0 );
		}
		WDT_FEED
		switch_rtc_callback();					// switch debouncing
		for (i = 0; i < 3; i++)
		{
			KCOL = col[i];
			vTaskDelay(1 / portTICK_RATE_MS);
			key_buf[i][k] = (~(KLINE | 0x81)>>4) & 0x0F;
			if (i == colPressed)
			{
				if (key_buf[colPressed][k] == 0x00)
				{
					page->iif_func[SC_KEY_RELEASE](&key);
					key = 0;
					key_old = 0;
					xQueueSend( qKeyboard, &key, 0 );
					colPressed = 0xFF;
				}
			}
			if (key_buf[i][k] != 0x00)
			{
				for (j = 0;j < KEY_DEBOUNCE;j++)
				{
					if (key_buf[i][0] == key_buf[i][j])
					{
						matchCount++;
					}
				}
				if (matchCount == KEY_DEBOUNCE)
				{
					key = key_buf[2][1] << 8 | key_buf[1][1] << 4 | key_buf[0][1];
					colPressed = i;
					if (key != key_old)
					{
						xQueueSend( qKeyboard, &key, 0 );
						keybord_func(key);
					}
					key_old = key;
				}
				matchCount = 0;
			}
		}
		k++;
		if (k == KEY_DEBOUNCE)
		{
			k = 0;
		}
	}
}

void keybord_func(uint16_t keyEntry)
{
	if ((keyEntry & KEY_ENTER) == KEY_ENTER)
	{
		page->iif_func[SC_KEY_ENTER](NULL);
	}
	if ((keyEntry & KEY_ESC) == KEY_ESC)
	{
		page->iif_func[SC_KEY_ESC](NULL);
	}
	if ((keyEntry & KEY_DOWN) == KEY_DOWN)
	{
		page->iif_func[SC_KEY_DOWN](NULL);
	}
	if ((keyEntry & KEY_UP) == KEY_UP)
	{
		page->iif_func[SC_KEY_UP](NULL);
	}
	if ((keyEntry & KEY_RIGHT) == KEY_RIGHT)
	{
		page->iif_func[SC_KEY_RIGHT](NULL);
	}
	if ((keyEntry & KEY_LEFT) == KEY_LEFT)
	{
		page->iif_func[SC_KEY_LEFT](NULL);
	}
	if ((keyEntry & KEY_Z_UP) == KEY_Z_UP)
	{
		page->iif_func[SC_KEY_ZUP](NULL);
	}
	if ((keyEntry & KEY_Z_DOWN) == KEY_Z_DOWN)
	{
		page->iif_func[SC_KEY_ZDOWN](NULL);
	}
}
