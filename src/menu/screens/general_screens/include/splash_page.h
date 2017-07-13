/** @file page5.h
 *  @brief
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_SCREEN_WARNING_H_
#define MENU_SCREEN_WARNING_H_

#define EMPTY_STR ""
#define ARCOOK_FAIL_STR1 "PLASMA NÃO"
#define ARCOOK_FAIL_STR2 "TRANSFERIDO"

typedef struct{
	char *btn1_str;
	char *btn2_str;
	char *t0_str;
	char *t1_str;
	char *t2_str;
	bool buttonUse;
}mn_warning_t;

extern mn_warning_t warning_entry;

#endif /* MENU_SCREEN_WARNING_H_ */
