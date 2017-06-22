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

/* Includes */
#include "platform.h"
#include "nextion.h"
#include "widget.h"
#include "timer_screen.h"
#include "screen.h"
#include "menu.h"
#include "selFiles_page.h"
#include "file.h"
#include "spiffs.h"
#include "xio.h"
/* Defines */

#define TIMER_NUM 0

#define WIDGET_NUM 13

#define FS_PAGE_SIZE              	 (256)
/* Static functions */
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t bvolta = {.name = "b0", .selectable = true};
static mn_widget_t bfolder = {.name = "p0", .selectable = true};
static mn_widget_t t0 = {.name = "t0", .selectable = true};
static mn_widget_t t1 = {.name = "t1", .selectable = true};
static mn_widget_t t2 = {.name = "t2", .selectable = true};
static mn_widget_t t3 = {.name = "t3", .selectable = true};
static mn_widget_t t4 = {.name = "t4", .selectable = true};
static mn_widget_t t5 = {.name = "t5", .selectable = true};
static mn_widget_t t6 = {.name = "t6", .selectable = true};
static mn_widget_t t7 = {.name = "t7", .selectable = true};
static mn_widget_t t8 = {.name = "t8", .selectable = true};
static mn_widget_t t9 = {.name = "t9", .selectable = true};
static mn_widget_t t10 = {.name = "t10", .selectable = true};


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
		&t8,
		&t9,
		&t10,
		&bfolder,
		&bvolta
};

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
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
bool showFileDir(mn_file_t *p_file, const char *path)
{
	bool ret = false;
	if (path[0] == '/' || !strcmp(path,".."))
	{
		for (uint8_t i = 0; i < WIDGET_NUM -1; i++)
			changeTxt(selFiles_page.p_widget[i],"");

		find_files(p_file,path);
		if (p_file->filesNum  < WIDGET_NUM - 1 )
		{
			selFiles_page.widgetSize = p_file->filesNum;
		}

		for (uint8_t i = 0; i < selFiles_page.widgetSize; i++)
			changeTxt(selFiles_page.p_widget[i],p_file->buffer[i]);
		ret = true;
	}
	return ret;
}


/************************** Public functions *********************************************/

void page_attach (void *p_arg)
{
}

void page_detach (void *p_arg)
{

#if (TIMER_NUM > 0)
	for (uint8_t i = 0; i < TIMER_NUM; i++)
		mn_screen_stop_timer(page4.p_timer[i]);
#endif
	selFiles_page.widgetSize = WIDGET_NUM - 1;
}

mn_file_t *gcode_file;
void page_handler (void *p_arg)
{
	mn_screen_event_t *p_page_hdl = p_arg;
	if (p_page_hdl->event == EVENT_SHOW)
	{
		gcode_file = pvPortMalloc(sizeof(mn_file_t));
		showFileDir(gcode_file, "/");
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(t0.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t1.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t2.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t3.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t4.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t5.id,EVENT_CLICK) ||
			 p_page_hdl->event == EVENT_SIGNAL(t6.id,EVENT_CLICK)
	)
	{
		uint16_t lineSel = selFiles_page.wt_selected;
		if (showFileDir(gcode_file, gcode_file->buffer[lineSel]) == false)
		{
			FIL File;
			spiffs_DIR sf_dir;
			struct spiffs_dirent e;
			struct spiffs_dirent *pe = &e;
			s32_t err;
			void *temp = NULL;
			uint32_t remain;

			f_open(&File,gcode_file->buffer[lineSel],FA_READ);

			spiffs_file *fd = &uspiffs[0].f;
			spiffs *fs = &uspiffs[0].gSPIFFS;

			SPIFFS_opendir(fs, "/", &sf_dir);
			pe = SPIFFS_readdir(&sf_dir, pe);

			*fd = SPIFFS_open_by_dirent(fs, pe, SPIFFS_RDWR, 0);
			if(*fd != SPIFFS_ERR_IS_FREE)
			{
				err = SPIFFS_fremove(fs, *fd);
			}

			*fd = SPIFFS_open(fs, gcode_file->buffer[lineSel], SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_DIRECT, 0);

			temp = pvPortMalloc( FS_PAGE_SIZE );
			while(!f_eof(&File))
			{
				f_read(&File,temp,FS_PAGE_SIZE,(UINT *)&remain);
				err = SPIFFS_write(fs, *fd, (u8_t *)temp, remain);
			}

			vPortFree(temp);
			f_close(&File);
			SPIFFS_close(fs, *fd);
			vPortFree(gcode_file);
			mn_screen_change(&main_page,EVENT_SHOW);
		}
		else
		{
			widgetVisible(&bfolder,NT_SHOW);
			widgetTouchable(&bfolder,NT_ENABLE);
		}
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(bfolder.id,EVENT_CLICK))
	{
		showFileDir(gcode_file, "..");
	}

	else if (p_page_hdl->event == EVENT_SIGNAL(bvolta.id,EVENT_CLICK))
	{
		vPortFree(gcode_file);
		mn_screen_change(&main_page,EVENT_SHOW);
	}

}
