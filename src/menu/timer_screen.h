/** @file screen.h
 *  @brief Function prototypes for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_TIMER_SCREEN_H_
#define MENU_TIMER_SCREEN_H_

typedef enum{
	MN_TIMER_SUCCESS = 0,
	MN_TIMER_ALREADY_CREATED,
	MN_TIMER_ALREADY_STARTED,
	MN_TIMER_ALREADY_STOPPED,
	MN_TIMER_ERROR
}mn_timer_err_t;

/**
 * @brief Widget structure
 *
 */
typedef struct{
	uint32_t id;
	char name[5];
	xTimerHandle hdl;
}mn_timer_t;

mn_timer_err_t mn_screen_create_timer (mn_timer_t *p_screen,uint32_t period);
mn_timer_err_t mn_screen_delete_timer (mn_timer_t *p_timer);
mn_timer_err_t mn_screen_start_timer (mn_timer_t *p_timer);
mn_timer_err_t mn_screen_stop_timer (mn_timer_t *p_timer);

#endif /* MENU_TIMER_SCREEN_H_ */
