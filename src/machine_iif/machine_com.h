/*
 * machine_com.h
 *
 *  Created on: Apr 28, 2017
 *      Author: LAfonso01
 */

#ifndef MACHINE_IIF_MACHINE_COM_H_
#define MACHINE_IIF_MACHINE_COM_H_

enum{
	AXIS_X_INFO,
	AXIS_Y_INFO,
	AXIS_Z_INFO,
	LINE_INFO,
	THC_REAL_INFO,
	THC_SET_INFO,
	VELOCIDADE_INFO,
	VELOCIDADE_JOG,
	TORCH_INFO,
	MATERIAL_INFO,
	ARCOOK_INFO,
	MAX_MACHINE_INFO
};

typedef enum{
	DIR_REVERSE,
	DIR_FORWARD
}mc_dir_t;

typedef enum{
	MC_TORCH_ON,
	MC_TORCH_OFF
}mc_torch_state_t;

void machine_enable(void);
void machine_start(void);
void machine_start_sim(void);
void machine_pause(void);
void machine_pause_arcoOKinit(void);
void machine_pause_ohmico(void);
void machine_restart(void);
void machine_restart_sim(void);
void machine_stop(uint8_t programEnd);
void machine_torch_state(mc_torch_state_t state);
void machine_jog(uint8_t axis, mc_dir_t direction);
void machine_jog_start(void);
void machine_jog_stop(void) ;
void machine_zerar_maquina(void);
void machine_zerar_peca(void);
void machine_homming_eixos(void);
void machine_limite_eixos(void);
void machine_info_update(uint8_t axis, char * textstr);
bool machine_alarms_get(uint8_t info);

#endif /* MACHINE_IIF_MACHINE_COM_H_ */
