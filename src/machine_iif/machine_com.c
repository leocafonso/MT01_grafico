/** @file
 *  @brief
 *
 *
 *
 *  @author leocafonso
 *  @bug No know bugs.
 */

/* Includes */
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "platform.h"
#include "machine_com.h"
#include "keyboard.h"
#include "tinyg.h"				// #1
#include "config.h"				// #2
#include "controller.h"
#include "hardware.h"
#include "json_parser.h"
#include "text_parser.h"
#include "gcode_parser.h"
#include "canonical_machine.h"
#include "plan_arc.h"
#include "planner.h"
#include "stepper.h"
#include "macros.h"
#include "plasma.h"
#include "xio.h"

/* Defines */


/* Static functions */


/* Static variables and const */
static bool ltorchBuffer = false;
static uint32_t arco = 0;

/* extern variables */
extern TaskHandle_t xCncTaskHandle;
extern bool lstop;
extern bool intepreterRunning;
extern bool zinhibitor;
extern bool sim;
extern bool simTorch;
extern float zeroPiecebuffer[3];

/************************** Static functions *********************************************/


/************************** Public functions *********************************************/

void machine_enable(void)
{
	pl_emergencia_init();
}

void machine_start(void)
{
	ltorchBuffer = false;
	restart_stepper();
	eepromReadConfig(CONFIGVAR_MAQ);
	if (configFlags[MODOMAQUINA] == MODO_PLASMA)
		eepromReadConfig(CONFIGVAR_PL);
	else
		eepromReadConfig(CONFIGVAR_OX);
	lstop = false;
	cm.gmx.feed_rate_override_enable = true;
	cm.gmx.feed_rate_override_factor = 1;
	stopDuringCut_Set(false);
	cm.machine_state = MACHINE_READY;
	tg_set_primary_source(CNC_MEDIA);
	xio_close(cs.primary_src);
	macro_func_ptr = _command_dispatch;
	xio_open(cs.primary_src,0,1);
	xTaskNotifyGive(xCncTaskHandle);
}

void machine_start_sim(void)
{
	sim = true;
	ltorchBuffer = false;
	restart_stepper();
	eepromReadConfig(CONFIGVAR_MAQ);
	if (configFlags[MODOMAQUINA] == MODO_PLASMA)
		eepromReadConfig(CONFIGVAR_PL);
	else
		eepromReadConfig(CONFIGVAR_OX);
	lstop = false;
	cm.gmx.feed_rate_override_enable = true;
	cm.gmx.feed_rate_override_factor = 1;
	stopDuringCut_Set(false);
	cm.machine_state = MACHINE_READY;
	tg_set_primary_source(CNC_MEDIA);
	xio_close(cs.primary_src);
	macro_func_ptr = _command_dispatch;
	xio_open(cs.primary_src,0,1);
	xTaskNotifyGive(xCncTaskHandle);
}


void machine_pause(void)
{
	if (isCuttingGet() == true)
			stopDuringCut_Set(true);
	lstop = true;
	cm_request_feedhold();
	pl_arcook_stop();
	if(isDwell == true)
	{
		st_command_dwell(DWELL_PAUSE);
	}
	while(cm.feedhold_requested == true)
	{
		vTaskDelay(1/portTICK_PERIOD_MS);
		WDT_FEED
	}
	ltorchBuffer = TORCH;
	TORCH = FALSE;
}

void machine_pause_arcoOKinit(void)
{
	TORCH = FALSE;
	pl_arcook_stop();
//	isCuttingSet(false);
	arco = ARCO_OK_FAILED;

	lstop = true;
	cm_request_feedhold();
	pl_arcook_stop();
	if(isDwell == true)
	{
		st_command_dwell(DWELL_PAUSE);
	}
	while(cm.feedhold_requested == true)
	{
		vTaskDelay(1/portTICK_PERIOD_MS);
		WDT_FEED
	}
	ltorchBuffer = TORCH;
	TORCH = FALSE;

	ltorchBuffer = TRUE;
}

void machine_pause_ohmico(void)
{
	machine_pause_arcoOKinit();
	state = 0;
}

void machine_restart(void)
{
	lstop = false;
	if(isDwell == true)
	{
		st_command_dwell(DWELL_RESTART);
	}
	if (arco == ARCO_OK_FAILED || ltorchBuffer == TRUE)
	{
		arco = 0;
		if(stopDuringCut_Get())
		{
			stopDuringCut_Set(false);
			if (cm.probe_state == PROBE_WAITING)
			{
				simTorch = false;
				macro_func_ptr = macro_buffer;
			}
			xMacroArcoOkSync = true;
		}
		else
		{
			simTorch = false;
			macro_func_ptr = macro_buffer;
			cm_request_cycle_start();
		}
		TORCH = ltorchBuffer;
	}
	else
	{
		cm_request_cycle_start();
		TORCH = ltorchBuffer;
	}
}

void machine_restart_sim(void)
{
	if(isDwell == true)
	{
		st_command_dwell(DWELL_RESTART);
	}
	cm_request_cycle_start();
}

void machine_sim_to_auto(uint8_t event)
{
	if(isDwell == true)
	{
		st_command_dwell(DWELL_EXIT);
	}
	if(sim){
		sim = false;
		if (simTorch)
		{
			TORCH = TRUE;
			simTorch = false;
			if(configFlags[MODOMAQUINA] == MODO_PLASMA){
				pl_arcook_start();
				/* Necessario para ligar o THC */
				delay_thcStartStop(true);
				isCuttingSet(true);
			}
		}
	}
}

void machine_stop(uint8_t programEnd)
{
		cm_request_feedhold();
		cm_request_queue_flush();
		xio_close(cs.primary_src);
		if(isDwell == true)
		{

			st_command_dwell(DWELL_ZERO);
			mr.move_state = MOVE_OFF;
			cm.queue_flush_requested = false;
			cm_queue_flush();
			cm.motion_state = MOTION_STOP;
		}
		state = 0;
		cm.cycle_state = CYCLE_OFF;
		pl_arcook_stop();
		cm.gmx.feed_rate_override_enable = true;
		cm.gmx.feed_rate_override_factor = 1;
		while(cm.queue_flush_requested == true && !programEnd)
		//while(cm.queue_flush_requested == true)
		{
			vTaskDelay(1/portTICK_PERIOD_MS);
		}
		cm.probe_state = PROBE_FAILED;
		TORCH = FALSE;
		macro_func_ptr = command_idle;
		intepreterRunning = false;
		sim = false;
		currentLine = 0;
		zinhibitor = false;
}

void machine_torch_state(mc_torch_state_t state)
{
	TORCH = state;
}

void machine_jog(uint8_t axis, mc_dir_t direction)
{
	if(direction == DIR_FORWARD)
		jogMaxDistance[axis] = 100000;
	else
		jogMaxDistance[axis] = -100000;
	macro_func_ptr = jog_Macro;
}

void machine_jog_start(void)
{
	restart_stepper();
	macro_func_ptr = command_idle;
	xTaskNotifyGive(xCncTaskHandle);
	intepreterRunning = true;
}

void machine_jog_stop(void)
{
	cm_request_feedhold();
	cm_request_queue_flush();
}

void machine_zerar_maquina(void)
{
	xTaskNotifyGive(xCncTaskHandle);
	macro_func_ptr = ZerarMaquina_Macro;
}

void machine_zerar_peca(void)
{
	if ((zero_flags & ZERO_PECA_FLAG) ==  ZERO_PECA_FLAG)
	{
		eepromReadConfig(ZEROPIECE);
		zeroPiece[AXIS_X] += mp_get_runtime_absolute_position(AXIS_X);
		zeroPiece[AXIS_Y] += mp_get_runtime_absolute_position(AXIS_Y);
		zeroPiece[AXIS_Z] = 0;
	}
	else
	{
		zeroPiece[AXIS_X] = mp_get_runtime_absolute_position(AXIS_X);
		zeroPiece[AXIS_Y] = mp_get_runtime_absolute_position(AXIS_Y);
		zeroPiece[AXIS_Z] = 0;
	}
	eepromWriteConfig(ZEROPIECE);
	xTaskNotifyGive(xCncTaskHandle);
	macro_func_ptr = ZerarPeca_Macro;
}

void machine_homming_eixos(void)
{
	restart_stepper();
	macro_func_ptr = homming_Macro;
	xTaskNotifyGive(xCncTaskHandle);
	intepreterRunning = true;

}

void machine_limite_eixos(void)
{
	restart_stepper();
	//macro_func_ptr = homming_Macro;
	xTaskNotifyGive(xCncTaskHandle);
	intepreterRunning = true;
}

void machine_info_update(uint8_t info, char * textstr)
{
	float vel;
	switch (info)
	{
		case AXIS_X_INFO: 	sprintf(textstr, "%4.2f", mp_get_runtime_absolute_position(AXIS_X));		break;
		case AXIS_Y_INFO: 	sprintf(textstr, "%4.2f", mp_get_runtime_absolute_position(AXIS_Y));		break;
		case AXIS_Z_INFO: 	sprintf(textstr, "%4.2f", mp_get_runtime_absolute_position(AXIS_Z));		break;
		case LINE_INFO:   	sprintf(textstr, "%d",  cm_get_linenum(RUNTIME));					break;
		case THC_REAL_INFO: sprintf(textstr, "%.0f",  THC_realGet());						break;
		case THC_MANUAL_INFO: 	pl_thc_read();
								sprintf(textstr, "%.0f",  THC_realGet());
			break;
		case THC_SET_INFO: 	sprintf(textstr, "%.0f",  configVarPl[PL_CONFIG_TENSAO_THC]); 	break;
		case VELOCIDADE_INFO:
			vel = cm_get_feed_rate(RUNTIME);
			if (vel == 0)
			{
				vel = mp_get_runtime_velocity();
			}
			sprintf(textstr, "%.0f",  vel);
		break;
		case VELOCIDADE_JOG:
			vel = *velocidadeJog;
			sprintf(textstr, "%.0f",  vel);
		break;
	}
}

bool machine_alarms_get(uint8_t info)
{
	bool alarm;
	switch (info)
	{
		case ARCOOK_INFO:    alarm = (arcoOkGet() ? (1) : (0));		break;
		case ARCOOK_MANUAL_INFO:    alarm = (ARCO_OK ? (0) : (1));		break;
		case MATERIAL_INFO: alarm = (MATERIAL ? (1) : (0)); 		break;
		case TORCH_INFO:   alarm = (TORCH ? (0) : (1));		   break;
	}
	return alarm;
}
