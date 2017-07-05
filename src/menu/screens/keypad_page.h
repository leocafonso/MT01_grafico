/** @file page5.h
 *  @brief
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef KEYPAD_MAIN_H_
#define KEYPAD_MAIN_H_

typedef enum {
	KEY_CONFIG_PL = 0,
	KEY_CONFIG_OX,
	KEY_CONFIG_JOG,
	KEY_CONFIG_MAQ,
	KEY_LINES,
	KEY_MAX
}mn_key_var_t;

typedef struct{
	char  *p_var_name;
	mn_key_var_t key_var;
	float *p_var;
	float step;
	float min;
	float max;
}mn_keypad_t;

#endif /* KEYPAD_MAIN_H_ */
