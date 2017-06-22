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
#include <stdlib.h>
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
#include "file.h"


/* Defines */
#define MAX_EXT_AVAIL	3
/* Static functions */

/* Static variables and const */
static const char* gaszFileExtensions[MAX_EXT_AVAIL] =
{
		".g",
		".tap",
		".nc"
};
/* Global variables and const */


/* extern variables */

/************************** Static functions *********************************************/
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
	char tmp[MAX_FILE_NAME + 1];
	char* pbStrNxt;
	int j;
	int i;

	/* Sorting array */
	for(i = 0; i < iLen; ++i)
	{
		/* Next element */
		pbStrNxt = pbArray + (MAX_FILE_NAME + 1);
		for(j = i + 1; j < iLen; ++j)
		{
			/* Swap */
			if(strcmp(pbArray, pbStrNxt) > 0)
			{
				strncpy(tmp, pbArray, MAX_FILE_NAME);
				strncpy(pbArray, pbStrNxt, MAX_FILE_NAME);
				strncpy(pbStrNxt, tmp, MAX_FILE_NAME);
			}

			pbStrNxt += MAX_FILE_NAME + 1;
		}

		/* Next element */
		pbArray += MAX_FILE_NAME + 1;
	}
}
/************************** Public functions *********************************************/
void find_files (mn_file_t *p_file,const char *path)
{
   FRESULT fr;     /* Return value */
   fr = f_opendir(&p_file->dir, path);
   p_file->filesNum = 0;
   for (;;) {
	   fr = f_readdir(&p_file->dir, &p_file->fil);                   /* Read a directory item */
	   if (fr != FR_OK || p_file->fil.fname[0] == 0) break;  /* Break on error or end of dir */
	   if (p_file->fil.fattrib & AM_DIR)
	   {                    /* It is a directory */
		  snprintf(p_file->buffer[p_file->filesNum],MAX_FILE_NAME, "/%s", p_file->fil.fname);
		  p_file->filesNum++;
	   }
	   else if(validExtension(p_file->fil.fname))
	   {
		  snprintf(p_file->buffer[p_file->filesNum],MAX_FILE_NAME, "%s", p_file->fil.fname);
		  p_file->filesNum++;
	   }
   }
   sort_array(p_file->buffer[0],p_file->filesNum);
   f_closedir(&p_file->dir);
}

