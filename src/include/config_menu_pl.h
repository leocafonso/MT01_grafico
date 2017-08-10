/*
 * config_menu_pl.h
 *
 *  Created on: Jun 15, 2016
 *      Author: LAfonso01
 */

#ifndef INCLUDE_CONFIG_MENU_PL_H_
#define INCLUDE_CONFIG_MENU_PL_H_

#include "ut_state_config_var.h"

#define PL_ALTURA_PERFURACAO	4.8                               //!< Altura de perfuração
#define PL_ALTURA_CORTE			2.8                               //!< Altura de corte
#define PL_VEL_CORTE			2500                             //!< Velocidade de corte
#define PL_TEMPO_PERFURACAO		0.1                               //!< Tempo de Perfuração
#define PL_TENSAO_THC			120                              //!< Tensao do THC

extern ut_config_var configsPl[PL_CONFIG_MAX];
extern ut_config_type pl_init_types[PL_CONFIG_MAX];
extern char* pl_init_names[PL_CONFIG_MAX];
extern float pl_init_max[PL_CONFIG_MAX];
extern float pl_init_min[PL_CONFIG_MAX];
extern float pl_init_step[PL_CONFIG_MAX];
extern uint8_t pl_init_point[PL_CONFIG_MAX];
extern char* pl_init_unit[PL_CONFIG_MAX];
extern void initPl(void);

#endif /* INCLUDE_CONFIG_MENU_PL_H_ */
