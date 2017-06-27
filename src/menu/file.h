/** @file widget.h
 *  @brief Function prototypes for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_FILE_H_
#define MENU_FILE_H_
#include "ff.h"

#define MAX_FILES_BUFFER 8
#define MAX_FILE_NAME 99

/**
 * @brief Widget structure
 *
 */
typedef struct{
	FILINFO fil;
	DIR	  dir;
	char  buffer[MAX_FILES_BUFFER][MAX_FILE_NAME+1];
	uint8_t filesNum;
}mn_file_t;

void find_files (mn_file_t *p_file,const char *path);

#endif /* MENU_FILE_H_ */
