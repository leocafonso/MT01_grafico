/*
 * ut_states_map.c
 *
 *  Created on: Oct 30, 2015
 *      Author: Fernando
 */


#include "ut_context.h"
#include "ut_state.h"

/**
 * State table
 */
const state_func_ptr states_table[STATE_NUMBER] =
{
		&ut_state_splash,						//!< Fun�ao da tela de entrada
		&ut_state_warning,                      //!< Fun�ao da tela de warnings
		&ut_state_main_menu,                    //!< Fun�ao da tela de principal
		&ut_state_choose_file,                  //!< Fun�ao da tela de escolha de arquivos
		&ut_state_config_menu_ox,               //!< Fun�ao da tela de configura��o de corte - Oxicorte
		&ut_state_config_menu_pl,               //!< Fun�ao da tela de configura��o de corte - Plasma
		&ut_state_config_manual_menu,           //!< Fun�ao da tela do menu de corte manual
		&ut_state_config_jog,          			//!< Fun�ao da tela da configura�ao de jog
		&ut_state_config_auto_menu,             //!< Fun�ao da tela do menu de corte automatico
		&ut_state_config_maquina,				//!< Fun�ao da tela de da configura�ao da maquina
		&ut_state_config_par_maq,				//!< Fun�ao da tela de parametros da maquina
		&ut_state_config_maq_thc,				//!< Fun�ao da tela de parametros do thc
		&ut_state_config_var,                   //!< Fun�ao da tela de manipula��o de variaveis
		&ut_state_manual_mode,                  //!< Fun�ao da tela de corte manual
		&ut_state_deslocaZero_mode,             //!< Fun�ao da tela de deslocar para zero
		&ut_state_auto_mode,                    //!< Fun�ao da tela de corte automatico
		&ut_state_line_selection,               //!< Fun�ao da tela de selecionar linhas
		&ut_state_config_maq_model,				//!< Fun�ao da tela de selecionar modelo de maquina
};
