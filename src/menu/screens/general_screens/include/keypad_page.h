/** @file page5.h
 *  @brief
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef KEYPAD_MAIN_H_
#define KEYPAD_MAIN_H_

typedef struct{
	char  *p_var_name;
	float *p_var;
	float step;
	float min;
	float max;
	mn_screen_t *p_ret_page;
	mn_screen_t *p_next_page;
}mn_keypad_t;

#endif /* KEYPAD_MAIN_H_ */
