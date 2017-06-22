/** @file page5.h
 *  @brief
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_SCREEN_WARNING_H_
#define MENU_SCREEN_WARNING_H_

enum{
	BTN_NO_USE = 0,
	BTN_OK,
	BTN_ASK
};

typedef enum{
	BTN_PRESSED_NAO = 0,
	BTN_PRESSED_SIM,
	BTN_PRESSED_OK
}warn_btn_t;

typedef struct{
	uint16_t img_txt[6];
	uint16_t msg_count;
	uint16_t time_interval;
	uint8_t buttonUseInit;
	uint8_t buttonUseEnd;
	void (*func_callback)(warn_btn_t btn_type);
}mn_warning_t;


#endif /* MENU_SCREEN_WARNING_H_ */
