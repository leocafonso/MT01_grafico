/** @file widget.c
 *  @brief Function implementation for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

/* Includes */
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "platform.h"
#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"
/* Defines */

/* Static functions */

/* Static variables and const */

/* Global variables and const */
mn_widget_t maq_mode_label = {.name =  "p8", .selectable = false};

/* extern variables */

/************************** Static functions *********************************************/

/************************** Public functions *********************************************/
void changeTxt(mn_widget_t *wg, const char *str)
{
	NexWidget_txt(wg->name,str);
}

void widgetClick(mn_widget_t *wg, nt_touch_event_t event)
{
	NexWidget_click(wg->name,event);
	wg->click = event;
}

void widgetChangePic(mn_widget_t *wg, uint16_t pic, uint16_t pic2)
{
	NexWidget_ChangePic(wg->name,pic);
	if 	(pic2 < 0xFFFF)
	{
		NexWidget_ChangePic2(wg->name,pic2);
	}
}

void widgetProgressBar(mn_widget_t *wg, uint8_t val)
{
	if (val > 100)
	{
		val = 100;
	}
	NexWidget_ProgressBar(wg->name,val);
}


void widgetVisible(mn_widget_t *wg, nt_vis_event_t event)
{
	NexWidget_visible(wg->name,event);
}

void widgetTouchable(mn_widget_t *wg, nt_enable_t event)
{
	NexWidget_touchable(wg->name,event);
}

void widgetGetPos(mn_widget_t *wg)
{
	char temp[10];
	vTaskDelay(10/portTICK_PERIOD_MS);
	sprintf(temp,"%s.x",wg->name);
	NexGet_num(temp,&wg->position.x);
	vTaskDelay(10/portTICK_PERIOD_MS);
	sprintf(temp,"%s.y",wg->name);
	NexGet_num(temp,&wg->position.y);
	vTaskDelay(10/portTICK_PERIOD_MS);
	sprintf(temp,"%s.h",wg->name);
	NexGet_num(temp,&wg->position.h);
	vTaskDelay(10/portTICK_PERIOD_MS);
	sprintf(temp,"%s.w",wg->name);
	NexGet_num(temp,&wg->position.w);
}

void widgetSelRec(mn_widget_t *wg, uint8_t thickness, uint16_t color)
{
	nt_pos_t recPos;
	for (uint8_t i = 0; i < thickness; i++)
	{
		recPos.x = wg->position.x - i;
		recPos.y = wg->position.y - i;
		recPos.h = wg->position.h + 2*i;
		recPos.w = wg->position.w + 2*i;
		NexDraw_rec(recPos,color);
	}
}
