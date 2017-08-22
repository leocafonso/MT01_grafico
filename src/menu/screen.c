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

#include "platform.h"
#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"
#include "keyboard.h"
#include "interpreter_if.h"
/* Defines */


/* Static functions */
static void page_key_up (void *p_arg);
static void page_key_down (void *p_arg);
static void page_key_right (void *p_arg);
static void page_key_left (void *p_arg);
static void page_key_zdown (void *p_arg);
static void page_key_zup (void *p_arg);
static void page_key_enter (void *p_arg);
static void page_key_esc (void *p_arg);
static void page_key_release (void *p_arg);


/* Static variables and const */

/* Global variables and const */
mn_screen_t *page = NULL;
/* extern variables */

/************************** Static functions *********************************************/
static void page_key_right (void *p_arg)
{
	uint16_t index_posmin = 0xFFFF;
	int16_t posx_cur,posx_diff,posx_diffmin = 1000;

	posx_cur = page->p_widget[page->wt_selected]->position.x + page->p_widget[page->wt_selected]->position.w;
	for (uint8_t i = 0;  i < page->widgetSize; i++)
	{
		if (i != page->wt_selected && page->p_widget[i]->selectable == true)
		{
			posx_diff = page->p_widget[i]->position.x - posx_cur;
			if (posx_diff >= 0)
			{
				if (posx_diff < posx_diffmin)
				{
					posx_diffmin = posx_diff;
					index_posmin = i;
				}
			}

		}
	}
	if (index_posmin < 0xFFFF)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		page->wt_selected = index_posmin;
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
}

static void page_key_left (void *p_arg)
{
	uint16_t index_posmin = 0xFFFF;
	int16_t posx_cur,posx_diff,posx_diffmin = 1000;
	posx_cur = page->p_widget[page->wt_selected]->position.x;
	for (uint8_t i = 0;  i < page->widgetSize; i++)
	{
		if (i != page->wt_selected && page->p_widget[i]->selectable == true)
		{
			posx_diff = posx_cur - (page->p_widget[i]->position.x + page->p_widget[i]->position.w);
			if (posx_diff >= 0)
			{
				if (posx_diff < posx_diffmin)
				{
					posx_diffmin = posx_diff;
					index_posmin = i;
				}
			}

		}
	}
	if (index_posmin < 0xFFFF)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		page->wt_selected = index_posmin;
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
}

static void page_key_up (void *p_arg)
{
	uint16_t index_posmin = 0xFFFF;
	int16_t posy_cur,posy_diff,posy_diffmin = 1000;
	posy_cur = page->p_widget[page->wt_selected]->position.y;
	for (uint8_t i = 0;  i < page->widgetSize; i++)
	{
		if (i != page->wt_selected && page->p_widget[i]->selectable == true)
		{
			posy_diff = posy_cur - page->p_widget[i]->position.y;
			if (posy_diff > 0)
			{
				if (posy_diff < posy_diffmin)
				{
					posy_diffmin = posy_diff;
					index_posmin = i;
				}
			}

		}
	}
	if (index_posmin < 0xFFFF)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		page->wt_selected = index_posmin;
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
}

static void page_key_down (void *p_arg)
{
	uint16_t index_posmin = 0xFFFF;
	int16_t posy_cur,posy_diff,posy_diffmin = 1000;
	posy_cur = page->p_widget[page->wt_selected]->position.y;
	for (uint8_t i = 0;  i < page->widgetSize; i++)
	{
		if (i != page->wt_selected && page->p_widget[i]->selectable == true)
		{
			posy_diff =  page->p_widget[i]->position.y - posy_cur;
			if (posy_diff > 0)
			{
				if (posy_diff < posy_diffmin)
				{
					posy_diffmin = posy_diff;
					index_posmin = i;
				}
			}

		}
	}
	if (index_posmin < 0xFFFF)
	{
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, DESELECT_COLOR);
		page->wt_selected = index_posmin;
		widgetSelRec(page->p_widget[page->wt_selected],SELECT_WIDTH, SELECT_COLOR);
	}
}

static void page_key_zdown (void *p_arg)
{

}

static void page_key_zup (void *p_arg)
{

}

static void page_key_enter (void *p_arg)
{
	widgetClick(page->p_widget[page->wt_selected], NT_PRESS);
}

static void page_key_esc (void *p_arg)
{

}

static void page_key_release (void *p_arg)
{
	uint32_t *key_pressed = p_arg;
	if (*key_pressed == KEY_ENTER)
	{
		mn_screen_event_t touch;
		widgetClick(page->p_widget[page->wt_selected], NT_RELEASE);
		touch.event = EVENT_SIGNAL(page->p_widget[page->wt_selected]->id,EVENT_CLICK);
		xQueueSend( menu.qEvent, &touch, 0 );
	}
}

/************************** Public functions *********************************************/
void mn_screen_change (mn_screen_t *p_screen, uint32_t event_in_change)
{
	mn_screen_event_t changePage;
	if (page != NULL)
	{
		mn_screen_bind_keyboard(page);
		page->iif_func[SC_DETACH](page);
	}
	page = p_screen;
	NexPage_show(p_screen->name);
	p_screen->iif_func[SC_ATTACH](page);
	changePage.event = event_in_change;
	xQueueSend(menu.qEvent, &changePage, 0 );
}

bool screenGetWidgetsInfo(mn_screen_t *p_screen)
{
	bool ret = false;
	char temp[20];
	uint8_t i;
	for (i = 0; i < p_screen->widgetSize; i++)
	{
		WDT_FEED
		if (p_screen->p_widget[i]->selectable == true)
		{
			sprintf(temp,"%s.%s.x",p_screen->name,p_screen->p_widget[i]->name);
			if(NexGet_num(temp,&p_screen->p_widget[i]->position.x) == false)
				break;
			sprintf(temp,"%s.%s.y",p_screen->name,p_screen->p_widget[i]->name);
			NexGet_num(temp,&p_screen->p_widget[i]->position.y);
			sprintf(temp,"%s.%s.h",p_screen->name,p_screen->p_widget[i]->name);
			NexGet_num(temp,&p_screen->p_widget[i]->position.h);
			sprintf(temp,"%s.%s.w",p_screen->name,p_screen->p_widget[i]->name);
			NexGet_num(temp,&p_screen->p_widget[i]->position.w);
			sprintf(temp,"%s.%s.id",p_screen->name,p_screen->p_widget[i]->name);
			NexGet_num(temp,&p_screen->p_widget[i]->id);
			sprintf(temp,"%s.%s.type",p_screen->name,p_screen->p_widget[i]->name);
			NexGet_num(temp,(uint32_t *)&p_screen->p_widget[i]->wtType);
			ret = true;
		}
	}
	return ret;
}

void mn_screen_change_image(mn_screen_t *p_screen, uint16_t pic)
{
	NexPage_ChangePic(p_screen->name,pic);
}

void mn_screen_bind_keyboard(mn_screen_t *p_screen)
{
	if (p_screen->p_widget != NULL)
	{
		p_screen->iif_func[SC_KEY_ENTER] = &page_key_enter;
		p_screen->iif_func[SC_KEY_ESC] = &page_key_esc;
		p_screen->iif_func[SC_KEY_DOWN] = &page_key_down;
		p_screen->iif_func[SC_KEY_UP] = &page_key_up;
		p_screen->iif_func[SC_KEY_RIGHT] = &page_key_right;
		p_screen->iif_func[SC_KEY_LEFT] = &page_key_left;
		p_screen->iif_func[SC_KEY_RELEASE] = &page_key_release;
		p_screen->iif_func[SC_KEY_ZDOWN] = &page_key_zdown;
		p_screen->iif_func[SC_KEY_ZUP] = &page_key_zup;
	}
	else
	{
		p_screen->iif_func[SC_KEY_ENTER] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_ESC] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_DOWN] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_UP] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_RIGHT] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_LEFT] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_RELEASE] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_ZDOWN] = &mn_screen_idle;
		p_screen->iif_func[SC_KEY_ZUP] = &mn_screen_idle;
	}
}

uint8_t mn_screen_select_widget(mn_screen_t *p_screen, mn_widget_t *widget)
{
	uint8_t ret = 255;
	for (uint8_t i = 0; i < p_screen->widgetSize; i++)
	{
		if (p_screen->p_widget[i] == widget)
		{
			ret = i;
		}
	}
	return ret;
}

uint8_t mn_screen_draw(mn_screen_t *p_screen)
{
	return 1;
}


void mn_screen_idle(void *p_arg) {}
