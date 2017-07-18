/** @file menu.h
 *  @brief Function prototypes for the graphical menu.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MN_MENU_H_
#define MN_MENU_H_

#include "warning_page.h"

#define EVENT_SHOW 					0
#define EMERGENCIA_EVENT 			1
#define SIM_ENTRY_EVENT 			2
#define PANEL_TOUCH_EVENT			10
#define FILE_LOAD_EVENT				11

#define ARCO_OK_INIT_FAILED_EVENT 	100
#define ARCO_OK_FAILED_EVENT  		101
#define ARCO_OK_OFF_EVENT     		102
#define MATERIAL_FAILED_EVENT	 	103
#define EMERGENCIA_SIGNAL_EVENT 	104
#define PROGRAM_FINISHED_EVENT 		105

#define EVENT_CLICK 				23
#define EVENT_PRESSED 				24
#define EVENT_TIMER 				25

#define EVENT_SIGNAL(_a, _b)   (((_a) | (_b)<< 8))
#define GET_ID(_a)   ((_a)&0x000000FF)
#define GET_EVENT(_b)   ((_b)&0x0000FF00)

/**
 * @brief Menu structure
 *
 */
typedef struct{
	//uint32_t eventType;
	xQueueHandle qEvent;
}mn_menu_t;

typedef struct{
	uint32_t event;
	void *p_arg;
}mn_screen_event_t;

extern mn_menu_t menu;
void menu_task(void);

#endif /* MN_MENU_H_ */
