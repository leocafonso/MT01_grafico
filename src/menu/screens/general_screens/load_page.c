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

#define WIDGET_NUM 2

#define TIMER_PROGRESS 210

#define FS_PAGE_SIZE              	 (256)

/* Static functions */
static void format_mem_task(void);
static void page_handler (void *p_arg);
static void page_attach (void *p_arg);
static void page_detach (void *p_arg);

/* Static variables and const */
static mn_widget_t btn_sim = {.name = "b0", .selectable = true};
static mn_widget_t btn_nao = {.name =  "b1", .selectable = true};
static mn_widget_t load_bar = {.name =  "j0", .selectable = false};
static mn_widget_t txt_info = {.name =  "p0", .selectable = false};
static mn_widget_t txt_version = {.name =  "t0", .selectable = false};

static mn_widget_t *p_widget[WIDGET_NUM] =
{
		&btn_sim,&btn_nao
};

static TaskHandle_t xHdlTaskFilecopy = NULL;
static mn_timer_t timer0 = {.id = TIMER_PROGRESS, .name = "tpro"};
static char *p_file_path;

#if (TIMER_NUM > 0)
static mn_timer_t *p_timer[TIMER_NUM] = {&timer0};
#endif
/* Global variables and const */
uint8_t loadfilesNum = 0;

mn_screen_t load_page = {.id = SC_PAGE11,
					.wt_selected = 0,
					.name        = "Load",
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
static void format_mem_task(void)
{
	spiffs_format();
	RESET;
	while(1)
	{

	}
}

static void copy_file_task( void * pvParameters )
{
	FIL File;
	spiffs_DIR sf_dir;
	struct spiffs_dirent e;
	struct spiffs_dirent *pe = &e;
	s32_t err;
	void *temp = NULL;
	uint32_t remain;
	char *p_path_file = pvParameters;
	mn_screen_event_t fileLoad;

	f_open(&File,p_path_file,FA_READ);

	spiffs_file *fd = &uspiffs[0].f;
	spiffs *fs = &uspiffs[0].gSPIFFS;

	SPIFFS_opendir(fs, "/", &sf_dir);

	do{
		pe = SPIFFS_readdir(&sf_dir, pe);
	}while(strcmp((const char *)pe->name,"config.met") == 0);


	*fd = SPIFFS_open_by_dirent(fs, pe, SPIFFS_RDWR, 0);
	if(*fd >= SPIFFS_OK)
	{
		err = SPIFFS_fremove(fs, *fd);
	}

	*fd = SPIFFS_open(fs, p_path_file, SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_DIRECT, 0);

	temp = pvPortMalloc( FS_PAGE_SIZE );
	while(!f_eof(&File))
	{
		f_read(&File,temp,FS_PAGE_SIZE,(UINT *)&remain);
		err = SPIFFS_write(fs, *fd, (u8_t *)temp, remain);
	}

	vPortFree(temp);
	vPortFree(p_path_file);
	f_close(&File);
	SPIFFS_close(fs, *fd);

	fileLoad.event = FILE_LOAD_EVENT;
	xQueueSend( menu.qEvent, &fileLoad, 0 );

    vTaskDelete( xHdlTaskFilecopy );

	while(1)
	{

	}
}
/************************** Public functions *********************************************/
void page_attach (void *p_arg)
{
	mn_screen_t *p_page_hdl = p_arg;
	mn_load_t *p_load_arg = p_page_hdl->p_args;
	widgetChangePic(&maq_mode_label,(machine_flag_get(MODOMAQUINA) ? (IMG_OXI_LABEL) : (IMG_PL_LABEL)),NO_IMG);
	if (p_load_arg->type == FILE_LOAD)
	{
		p_file_path = pvPortMalloc(MAX_FILE_NAME + 1);
		strcpy(p_file_path,p_load_arg->p_path);
	}
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
	static uint8_t progress_val;
	mn_screen_event_t *p_page_hdl = p_arg;
	mn_load_t *p_load_arg = p_page_hdl->p_arg;
	char str[20];

	if (p_page_hdl->event == EVENT_SHOW)
	{
		if (p_load_arg->type == MEM_FORMAT)
		{
			widgetVisible(&btn_sim, NT_HIDE);
			widgetVisible(&btn_nao, NT_HIDE);
			widgetVisible(&txt_info, NT_HIDE);
			widgetVisible(&load_bar, NT_SHOW);
			snprintf(str,sizeof(str), "Formatando mem ext");
		    xTaskCreate( (pdTASK_CODE)format_mem_task,     "format_mem_task    ",  256, NULL, 1, NULL); /* keyboard_task      */
		}
		if (p_load_arg->type == FILE_LOAD)
		{
			widgetVisible(&btn_sim, NT_HIDE);
			widgetVisible(&btn_nao, NT_HIDE);
			widgetVisible(&txt_info, NT_HIDE);
			widgetVisible(&load_bar, NT_SHOW);
			snprintf(str,sizeof(str), "Carregando arquivo");
		    xTaskCreate( (pdTASK_CODE)copy_file_task,     "format_mem_task    ",  512, p_file_path, 1, &xHdlTaskFilecopy);
		}
		changeTxt(&txt_version,str);
		mn_screen_create_timer(&timer0,200);
		mn_screen_start_timer(&timer0);
	}
	else if (p_page_hdl->event == EVENT_SIGNAL(timer0.id,EVENT_TIMER))
	{
		if(progress_val <= 10)
		{
			widgetProgressBar(&load_bar,(progress_val*100)/10);
			progress_val++;
		}
		else
		{
			progress_val = 0;
		}
	}
	else if (p_page_hdl->event == FILE_LOAD_EVENT)
	{
		mn_screen_change(&main_page,EVENT_SHOW);
	}
	else if (p_page_hdl->event == EMERGENCIA_SIGNAL_EVENT)
	{
		emergencia_args.p_ret_page = page;
		emergencia_page.p_args = &emergencia_args;
		mn_screen_change(&emergencia_page,EVENT_SHOW);
	}
}
