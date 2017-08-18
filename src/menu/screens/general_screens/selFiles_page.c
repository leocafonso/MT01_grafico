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

#define WIDGET_NUM 13

#define FILE_IN_SCREEN 8

/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);
static void warning_file_callback(warn_btn_t btn_type);

/* Static variables and const */
static mn_widget_t bvolta = {.name = "b0", .selectable = true};
static mn_widget_t bfolder = {.name = "p0", .selectable = true};
static mn_widget_t bcima = {.name = "b2", .selectable = true};
static mn_widget_t bok = {.name = "b1", .selectable = true};
static mn_widget_t bbaixo = {.name = "b3", .selectable = true};
static mn_widget_t t0 = {.name = "t0", .selectable = true};
static mn_widget_t t1 = {.name = "t1", .selectable = true};
static mn_widget_t t2 = {.name = "t2", .selectable = true};
static mn_widget_t t3 = {.name = "t3", .selectable = true};
static mn_widget_t t4 = {.name = "t4", .selectable = true};
static mn_widget_t t5 = {.name = "t5", .selectable = true};
static mn_widget_t t6 = {.name = "t6", .selectable = true};
static mn_widget_t t7 = {.name = "t7", .selectable = true};
static uint16_t new_index = 0;
static mn_screen_event_t selFiles;
static mn_file_t *gcode_file;
static mn_load_t load_arg;



static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&t0,
		&t1,
		&t2,
		&t3,
		&t4,
		&t5,
		&t6,
		&t7,
		&bfolder,
		&bvolta,
		&bcima,
		&bok,
		&bbaixo
};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif

static mn_warning_t warn_files_args = { .buttonUseInit = BTN_OK,
											.img_txt[0] = IMG_SEM_ARQUIVO,
											.msg_count = 1,
											.func_callback = warning_file_callback
										   };

/* Global variables and const */
mn_screen_t selFiles_page = {.id 		 = SC_PAGE5,
					.wt_selected = 0,
					.name        = "SelFiles",
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
static bool showFileDir(mn_file_t *p_file, const char *path)
{
	bool ret = false;
	if (path[0] == '/' || !strcmp(path,".") || !strcmp(path,".."))
	{
		for (uint8_t i = 0; i < FILE_IN_SCREEN; i++)
			changeTxt(selFiles_page.p_widget[i],"");

		find_files(p_file,path);
		if(p_file->filesNum > 0)
		{
			if (p_file->filesNum  < FILE_IN_SCREEN )
			{
				selFiles_page.widgetSize = p_file->filesNum;
			}
			else
			{
				selFiles_page.widgetSize = FILE_IN_SCREEN;
			}

			for (uint8_t i = 0; i < selFiles_page.widgetSize; i++)
				changeTxt(selFiles_page.p_widget[i],p_file->buffer[i]);
			ret = true;
		}
	}
	return ret;
}


static void showFileDown(mn_file_t *p_file, uint16_t index)
{
	if (index >= FILE_IN_SCREEN)
	{
		for (uint8_t i = 0; i < FILE_IN_SCREEN; i++)
			changeTxt(selFiles_page.p_widget[i],"");

		new_index = index - (FILE_IN_SCREEN - 1);

		for (uint8_t i = 0; i < FILE_IN_SCREEN; i++)
			changeTxt(selFiles_page.p_widget[i],p_file->buffer[i + new_index]);
	}
}

static void showFileUp(mn_file_t *p_file, uint16_t index)
{
	for (uint8_t i = 0; i < FILE_IN_SCREEN; i++)
		changeTxt(selFiles_page.p_widget[i],"");

	new_index = index;

	for (uint8_t i = 0; i < FILE_IN_SCREEN; i++)
		changeTxt(selFiles_page.p_widget[i],p_file->buffer[i + new_index]);
}

static void selFiles_key_enter (void *p_arg)
{
	widgetClick(&bok, NT_PRESS);
}

static void selFiles_key_up (void *p_arg)
{
	widgetClick(&bcima, NT_PRESS);
}

static void selFiles_key_down (void *p_arg)
{
	widgetClick(&bbaixo, NT_PRESS);
}

static void selFiles_key_esc (void *p_arg)
{
	TCHAR str[10];
	if (FR_OK == f_getcwd(str, sizeof str / sizeof *str))
	{
		if (!strcmp(str, "/"))
		{
			widgetClick(&bvolta, NT_PRESS);
		}
		else
		{
			widgetClick(&bfolder, NT_PRESS);
		}
	}
	else
	{
		widgetClick(&bvolta, NT_PRESS);
	}
}

static void selFiles_key_release (void *p_arg)
{
	if (bok.click == NT_PRESS)
	{
		widgetClick(&bok, NT_RELEASE);
		selFiles.event = EVENT_SIGNAL(bok.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &selFiles, 0 );
	}
	else if (bcima.click == NT_PRESS)
	{
		widgetClick(&bcima, NT_RELEASE);
		selFiles.event = EVENT_SIGNAL(bcima.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &selFiles, 0 );
	}
	else if (bbaixo.click == NT_PRESS)
	{
		widgetClick(&bbaixo, NT_RELEASE);
		selFiles.event = EVENT_SIGNAL(bbaixo.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &selFiles, 0 );
	}
	else if (bfolder.click == NT_PRESS)
	{
		widgetClick(&bfolder, NT_RELEASE);
		selFiles.event = EVENT_SIGNAL(bfolder.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &selFiles, 0 );
	}
	else if (bvolta.click == NT_PRESS)
	{
		widgetClick(&bvolta, NT_RELEASE);
		selFiles.event = EVENT_SIGNAL(bvolta.id, EVENT_CLICK);
		xQueueSend( menu.qEvent, &selFiles, 0 );
	}
}


/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	selFiles_page.iif_func[SC_KEY_ENTER] = selFiles_key_enter;
	selFiles_page.iif_func[SC_KEY_ESC] = selFiles_key_esc;
	selFiles_page.iif_func[SC_KEY_DOWN] = selFiles_key_down;
	selFiles_page.iif_func[SC_KEY_UP] = selFiles_key_up;
	selFiles_page.iif_func[SC_KEY_RIGHT] = mn_screen_idle;
	selFiles_page.iif_func[SC_KEY_LEFT] = mn_screen_idle;
	selFiles_page.iif_func[SC_KEY_ZDOWN] = mn_screen_idle;
	selFiles_page.iif_func[SC_KEY_ZUP] = mn_screen_idle;
	selFiles_page.iif_func[SC_KEY_RELEASE] = selFiles_key_release;
}

void page_detach (void *p_arg)
{

#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page4.p_timer[i]);
#endif
	selFiles_page.widgetSize = WIDGET_NUM - 1;
}

uint32_t size_heap;

void page_handler (void *p_arg)
{
	static int16_t listIndex = 0;
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		listIndex = 0;
		new_index = 0;
		page->wt_selected = 0;
		widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
		if (gcode_file == NULL)
		{
			size_heap = xPortGetFreeHeapSize();
			if (size_heap > sizeof(mn_file_t))
			{
				gcode_file = pvPortMalloc(sizeof(mn_file_t));
			}
			else
			{
				mn_screen_change(&main_page,EVENT_SHOW);
				return;
			}
		}
		else
		{
			vPortFree(gcode_file);
			gcode_file = NULL;
			size_heap = xPortGetFreeHeapSize();
			if (size_heap > sizeof(mn_file_t))
			{
				gcode_file = pvPortMalloc(sizeof(mn_file_t));
			}
			else
			{

				mn_screen_change(&main_page,EVENT_SHOW);
				return;
			}
		}
		f_chdir("/");
		if (showFileDir(gcode_file, "/") == false)
		{
			vPortFree(gcode_file);
			gcode_file = NULL;
			warning_page.p_args = &warn_files_args;
			mn_screen_change(&warning_page,EVENT_SHOW);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(t0.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t1.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t2.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t3.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t4.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t5.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t6.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t7.id,EVENT_CLICK)
	)
	{
		for (uint8_t i = 0; i < page->widgetSize; i++)
		{
			if (page->p_widget[i]->id == GET_ID(p_page_hdl->event))
			{
				widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
				page->wt_selected = i;
				widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
				break;
			}
		}
		listIndex = (GET_ID(p_page_hdl->event) - 3) + new_index;
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(bfolder.id,EVENT_CLICK))
	{
		listIndex = 0;
		new_index = 0;
		widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
		page->wt_selected = 0;
		widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);

		showFileDir(gcode_file, "..");
		f_chdir("..");
	}

	else if (p_page_hdl->event == EVENT_SIGNAL(bcima.id,EVENT_CLICK))
	{
		if (page->wt_selected > 0)
		{
			widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
			page->wt_selected -= 1;
			widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
			listIndex -= 1;
		}
		else if (page->wt_selected == 0)
		{
			if (listIndex > 0)
			{
				listIndex -= 1;
				showFileUp(gcode_file,listIndex);
			}
		}
	}

	else if (p_page_hdl->event == EVENT_SIGNAL(bbaixo.id,EVENT_CLICK))
	{
		if (page->wt_selected < FILE_IN_SCREEN - 1)
		{
			widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
			page->wt_selected += 1;
			widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);
		}
		if (listIndex < gcode_file->filesNum - 1)
		{
			listIndex += 1;
			showFileDown(gcode_file,listIndex);
		}
	}

	else if (p_page_hdl->event == EVENT_SIGNAL(bok.id,EVENT_CLICK))
	{

		//uint16_t lineSel = selFiles_page.wt_selected;
		uint16_t lineSel = listIndex;

		if (gcode_file->buffer[lineSel][0] == '/')
		{
			listIndex = 0;
			new_index = 0;
			widgetSelRec(page->p_widget[page->wt_selected],3, DESELECT_COLOR);
			page->wt_selected = 0;
			widgetSelRec(page->p_widget[page->wt_selected],3, SELECT_COLOR);

			f_chdir(&gcode_file->buffer[lineSel][1]);
			showFileDir(gcode_file, ".");
			widgetVisible(&bfolder,NT_SHOW);
			widgetTouchable(&bfolder,NT_ENABLE);
		}
		else
		{
	    	load_arg.type = FILE_LOAD;
	    	load_arg.p_path = gcode_file->buffer[lineSel];
	    	load_page.p_args = &load_arg;
			mn_screen_change(&load_page,EVENT_SHOW);
			vPortFree(gcode_file);
			gcode_file = NULL;
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(bvolta.id,EVENT_CLICK))
	{
		vPortFree(gcode_file);
		gcode_file = NULL;
		mn_screen_change(&main_page,EVENT_SHOW);
	}

	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}

}

static void warning_file_callback(warn_btn_t btn_type)
{
	mn_screen_change(&main_page,EVENT_SHOW);
}
