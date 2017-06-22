/*
 * ut_state_choose_file.c
 *
 *  Created on: Nov 2, 2015
 *      Author: Fernando
 */

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "config_kernel.h"

#include "r_usb_basic_if.h"
#include "r_usb_hmsc.h"

#include "ut_context.h"
#include "ut_state.h"
#include "interpreter_if.h"
#include "xio.h"

#include "lcd_menu.h"
#include "lcd.h"
#include "fsystem_spi.h"
#include "spiffs_hw.h"
#include "spiffs.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

// ***********************************************************************
// Global variables
// ***********************************************************************
FATFS  st_usb_fatfs;
DIR	   st_usb_dir;
char gszCurFile[MAX_FILE_PATH_SIZE];
char buff[MAX_FILE_PATH_SIZE];
extern bool drivemountFlag;
extern uint32_t choosedLine;
extern uint32_t choosedLinePosition;
extern uint32_t currentLineSel;
extern USB_UTR_t       msc_utr;

// ***********************************************************************
// Internal variables
// ***********************************************************************
#define MAX_EXT_AVAIL	3
#define MAX_PATH_LENGHT 255
static const char* gaszFileExtensions[MAX_EXT_AVAIL] =
{
		".g",
		".tap",
		".nc"
};

static const char* gszFileMenuTitle = "ESCOLHA UM ARQUIVO";
/* current file header */
fs_image_header_t *g_fs_cur_header;
/* next file header */
fs_image_header_t g_fs_next_header;

// ***********************************************************************
// Global types
// ***********************************************************************

typedef enum ut_fs_navigate_tag
{
	NAVIGATE_CANCELLED = -1,
	NAVIGATE_CONTINUE,
	NAVIGATE_END
} ut_fs_navigate;

// ***********************************************************************
// Global functions
// ***********************************************************************

/**
 * Get first occurrence of find in string in reverse mode.
 * It is the last occurrence of find in string.
 *
 * @param string	String to be searched.
 * @param find		String to search.
 * @return Pointer to the last occurrence.
 */
static char * ut_strrstr(char *string, char *find)
{
  char *cp;
  uint32_t len = strlen(find);
  for (cp = string + strlen(string); cp >= string; cp--)
  {
    if (strncmp(cp, find, len) == 0)
    {
    	return cp;
    }
  }
  return NULL;
}

/**
 * Checks if a word has a valid g extension
 * @param szWord
 * @return
 */
static bool validExtension(const char* szWord)
{
	uint8_t i;
	uint32_t wordLen = strlen(szWord);
	uint32_t suffixLen = 0;

	for(i = 0; i < MAX_EXT_AVAIL; i++)
	{
		suffixLen = strlen(gaszFileExtensions[i]);
		if(wordLen >= suffixLen && !memcmp(&szWord[wordLen - suffixLen], gaszFileExtensions[i], suffixLen))
		{
			return true;
		}
	}

	return false;
}

/**
 * Sort array in ascending order.
 * @param pbArray	Array to be sorted.
 * @param iLen		Array length.
 */
static void sort_array(char* pbArray, int iLen)
{
	char tmp[MAX_PATH_LENGHT + 1];
	char* pbStrNxt;
	int j;
	int i;

	/* Sorting array */
	for(i = 0; i < iLen; ++i)
	{
		/* Next element */
		pbStrNxt = pbArray + (MAX_PATH_LENGHT + 1);
		for(j = i + 1; j < iLen; ++j)
		{
			/* Swap */
			if(strcmp(pbArray, pbStrNxt) > 0)
			{
				strncpy(tmp, pbArray, MAX_PATH_LENGHT);
				strncpy(pbArray, pbStrNxt, MAX_PATH_LENGHT);
				strncpy(pbStrNxt, tmp, MAX_PATH_LENGHT);
			}

			pbStrNxt += MAX_PATH_LENGHT + 1;
		}

		/* Next element */
		pbArray += MAX_PATH_LENGHT + 1;
	}
}

DIR	 st_usb_dir;
FILINFO st_usb_finfo;
#if _USE_LFN
char Lfname[_MAX_LFN + 1];
#endif
/**
 * Scan files in a given directory and returns the selection.
 * If the selection is a directory, it does it recursively
 *
 * @return
 */
static ut_fs_navigate chooseFile()
{
//	FIL File;
//	FRESULT eRes;
//	ut_menu filesMenu;
//	uint8_t i;
//	spiffs_DIR sf_dir;
//	struct spiffs_dirent e;
//	struct spiffs_dirent *pe = &e;
//	s32_t err;
//	void *temp = NULL;
//	uint32_t remain;
//
//	char *fn;
//	char aszFiles[MENU_MAX_ITEMS][MAX_PATH_LENGHT + 1];
//
//#if _USE_LFN
//	st_usb_finfo.lfname = Lfname;
//	st_usb_finfo.lfsize = sizeof Lfname;
//#endif
//	/* Clean */
//	memset(aszFiles, 0, MENU_MAX_ITEMS * (MAX_PATH_LENGHT + 1));
//
//	/* Initialize menu */
//	ut_menu_init(&filesMenu);
//	/* No header */
//	filesMenu.title = gszFileMenuTitle;
////	filesMenu.boShowTitle = true;
////	filesMenu.maxItemsPerPage = MAX_ROW;
//
//	memset(gszCurFile,'\0',sizeof(gszCurFile));
//	/* Open dir */
//	f_getcwd (gszCurFile,sizeof(gszCurFile));
//	eRes = f_opendir(&st_usb_dir, gszCurFile);
//	if(eRes == FR_OK)
//	{
//		/* Check if it is on root */
//		if(strcmp(gszCurFile,"/") != 0)
//		{
//			sprintf(aszFiles[filesMenu.numItems++], "..");
//		}
//		/* Populate menu */
//		while(true)
//		{
//			eRes = f_readdir(&st_usb_dir, &st_usb_finfo);
//			/* Check for end of dir */
//			if(eRes != FR_OK || st_usb_finfo.fname[0] == 0) { break; }
//			if(st_usb_finfo.fname[0] == '.') { continue; } /* Ignore dot entry */
//#if _USE_LFN
//            fn = *st_usb_finfo.lfname ? st_usb_finfo.lfname : st_usb_finfo.fname;
//			if(strstr(fn, "System")) { continue; }
//#else
//            fn = st_usb_finfo.fname;
//			if(strstr(fn, "SYSTEM")) { continue; }
//#endif
//			/* Copy to menu */
//			if(st_usb_finfo.fattrib & AM_DIR)
//			{
//				sprintf(aszFiles[filesMenu.numItems], "/%.*s", MAX_PATH_LENGHT, fn);
//			}
//			else if(validExtension(fn))
//			{
//				sprintf(aszFiles[filesMenu.numItems], "%.*s", MAX_PATH_LENGHT, fn);
//			}
//			else
//			{
//				/* Not a valid file */
//				continue;
//			}
//
//			if(++filesMenu.numItems == MENU_MAX_ITEMS) { break; }
//		}
//
//		/* Sort */
//		sort_array(aszFiles[0], filesMenu.numItems);
//		/* Fill menu */
//		for(i = 0; i < filesMenu.numItems; ++i)
//		{
//			filesMenu.items[i].text = aszFiles[i];
//		}
//
//		/* Show menu */
//		if(ut_menu_browse(&filesMenu, portMAX_DELAY) >= 0)
//		{
//			if(filesMenu.items[filesMenu.selectedItem].text[0] == '/')
//			{
//
//				/* Is a dir, recursively */
//				strcat(gszCurFile, filesMenu.items[filesMenu.selectedItem].text);
//				f_chdir(gszCurFile);
//				return NAVIGATE_CONTINUE;
//			}
//			else if(filesMenu.items[filesMenu.selectedItem].text[0] == '.')
//			{
//				/* It should rise up a level */
//				//char* last = ut_strrstr(gszCurFile, "/");
//				//if(last != 0) *last = 0;
//				f_chdir("..");
//				return NAVIGATE_CONTINUE;
//			}
//			memset(gszCurFile,'\0',sizeof(gszCurFile));
//			/* Is a file - end of routine */
//			strcat(gszCurFile, filesMenu.items[filesMenu.selectedItem].text);
//
//			f_open(&File,gszCurFile,FA_READ);
//
//			ut_lcd_output_warning("CARREGANDO...\n");
//
//			spiffs_file *fd = &uspiffs[0].f;
//			spiffs *fs = &uspiffs[0].gSPIFFS;
//
//			SPIFFS_opendir(fs, "/", &sf_dir);
//			pe = SPIFFS_readdir(&sf_dir, pe);
//			*fd = SPIFFS_open_by_dirent(fs, pe, SPIFFS_RDWR, 0);
//			if(*fd != SPIFFS_ERR_IS_FREE)
//			{
//				err = SPIFFS_fremove(fs, *fd);
//			}
//			ut_strrstr(gszCurFile, "/");
//			*fd = SPIFFS_open(fs, gszCurFile, SPIFFS_CREAT | SPIFFS_RDWR | SPIFFS_DIRECT, 0);
//
//			temp = pvPortMalloc( FS_PAGE_SIZE );
//			while(!f_eof(&File))
//			{
//				f_read(&File,temp,FS_PAGE_SIZE,(UINT *)&remain);
//				err = SPIFFS_write(fs, *fd, (u8_t *)temp, remain);
//			}
//
//			vPortFree(temp);
//			f_close(&File);
//		    SPIFFS_close(fs, *fd);
//
//		    return NAVIGATE_END;
//		}
//	}
//	/* Operation was cancelled */
//	gszCurFile[0] = 0;
//	ut_lcd_output_warning("NENHUM ARQUIVO\n\
//						   DE CORTE\nENCONTRADO\n");
//
//	vTaskDelay(1000 / portTICK_PERIOD_MS);
	return NAVIGATE_CANCELLED;
}

// ***********************************************************************
// Public functions
// ***********************************************************************

/**
 * Choose G file from USB.
 * @param pContext
 * @return
 */
ut_state ut_state_choose_file(ut_context* pContext)
{
	usb_err_t res;
	/* Root dir */
	memset(gszCurFile, 0, sizeof(gszCurFile));
	strcpy(gszCurFile, USB_ROOT);
//	res = R_USB_Open( (usb_ip_t)msc_utr.ip );
//	vTaskDelay(600 / portTICK_PERIOD_MS);

	/* Check if usb is mounted */
	if (drivemountFlag)
  //  if( xSemaphoreTake( xUsbMount, 100 / portTICK_PERIOD_MS ) == pdTRUE )
	{
	/* Check if usb is mounted */
		f_opendir(&st_usb_dir, USB_ROOT);
		f_chdir("/");
	}
	else
	{
//	    R_USB_Close( (usb_ip_t)msc_utr.ip );
//	    R_usb_hmsc_Release((usb_ip_t)msc_utr.ip );
//		UsbTaskDelete();
		ut_lcd_output_warning("NENHUM USB\n\
							   ENCONTRADO\n");

		vTaskDelay(1000 / portTICK_PERIOD_MS);

		return STATE_MAIN_MENU;
	}

	choosedLine = 0;
	choosedLinePosition = 0;
	currentLineSel = 0;
	/* Fat is mounted */
	ut_lcd_clear();

	/* Read */
	ut_lcd_output();

	/* Try to selected a file */
	ut_fs_navigate eErr;
	do
	{
		/* Just a delay */
		vTaskDelay(10 / portTICK_PERIOD_MS);
		/* Navigate through folders */
		eErr = chooseFile();
	} while(eErr == NAVIGATE_CONTINUE);
 //   R_USB_Close( (usb_ip_t)msc_utr.ip );
//    R_usb_hmsc_Release((usb_ip_t)msc_utr.ip );
//	UsbTaskDelete();
	/* Go back to menu */
	return pContext->value[0];
}
