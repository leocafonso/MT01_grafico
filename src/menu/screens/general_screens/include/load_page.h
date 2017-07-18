/** @file page5.h
 *  @brief
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef LOAD_PAGE_H_
#define LOAD_PAGE_H_

#define MCU_FILE  		0x01
#define NEXTION_FILE  	0x02
#define MEM_FORMAT  	0x04
#define FILE_LOAD  		0x08

typedef struct {
	uint8_t type;
	char *p_path;
}mn_load_t;

extern uint8_t loadfilesNum;

#endif /* LOAD_PAGE_H_ */
