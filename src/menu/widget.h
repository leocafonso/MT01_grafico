/** @file widget.h
 *  @brief Function prototypes for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_WIDGET_H_
#define MENU_WIDGET_H_

#define SELECT_COLOR       0
#define DESELECT_COLOR     65535

/**
 * @brief Defines the type of widget
 *
 */
enum{
	IMG_SPLASH_1 = 0,
	IMG_WARNING_1 = 53,
	IMG_WARNING_2 = 54,
	IMG_WARNING_3 = 55,
	IMG_WARNING_4 = 56,
	IMG_WARNING_5 = 57,
	IMG_WARNING_6 = 58,
	IMG_EMERGENCIA = 60,
	IMG_CONTINUAR = 59,
	IMG_FINALIZADO = 61,
	IMG_BTN_OK = 31,
	IMG_BTN_OK_PRESS = 30,
	IMG_BTN_PAUSE = 35,
	IMG_BTN_PAUSE_PRESS = 34,
	IMG_BTN_PLAY = 37,
	IMG_BTN_PLAY_PRESS = 36,
	IMG_LED_OFF = 26,
	IMG_LED_ON = 27,
	IMG_TOCHA_OFF = 51,
	IMG_TOCHA_ON = 50,
	IMG_PLASMA_NAO_TRANSF = 67,
	IMG_OHMICO = 68,
	IMG_ZERO_MAQ = 97,
	IMG_ZERO_MAQ_INIT = 98,
	IMG_ZERO_PECA = 99,
	IMG_SEM_ZERO_MAQ = 100,
	IMG_OXI_EN = 109,
	IMG_PL_EN =  110,
	IMG_OXI_LABEL = 111,
	IMG_PL_LABEL =  112,
	IMG_DESL_EN = 113,
	IMG_LIG_EN =  114,
	IMG_SAIR =  138,
	IMG_LIMITES =  142,
	IMG_PAR_MAQ_AC_JUNCAO_PRESSED = 148,
	IMG_PAR_MAQ_AC_JUNCAO = 149,
	IMG_PAR_MAQ_DESVIO_JUNCAO_PRESSED = 150,
	IMG_PAR_MAQ_DESVIO_JUNCAO = 151,
	IMG_PAR_MAQ_EIXOX1_PRESSED = 152,
	IMG_PAR_MAQ_EIXOX1 = 153,
	IMG_PAR_MAQ_EIXOX2_PRESSED = 154,
	IMG_PAR_MAQ_EIXOX2 = 155,
	IMG_PAR_MAQ_EIXOY_PRESSED = 156,
	IMG_PAR_MAQ_EIXOY = 157,
	IMG_PAR_FORMAT_PRESSED = 158,
	IMG_PAR_FORMAT = 159,
	IMG_SEM_ARQUIVO =  177,
	IMG_SEM_INFO_LINHA =  178,
	NO_IMG = 0xFFFF
};

/**
 * @brief Defines the type of widget
 *
 */
typedef enum{
	U8_TEXT = 0UL,  /*!<Button widget */
	U8_LINE = 1UL,  /*!<Button widget */
	WT_BUTTON = 98UL,  /*!<Button widget */
	WT_TEXTBOX = 116UL,    /*!<Textbox widget */
}mn_wt_type_t;

typedef struct{
	uint8_t line;
	uint8_t col;
}mn_matrix_t;

/**
 * @brief Widget structure
 *
 */
typedef struct{
	uint32_t id;
	char name[5];
	char str[10];
	nt_pos_t position;
	bool selectable;
	nt_touch_event_t click;
	mn_wt_type_t wtType;
}mn_widget_t;

extern mn_widget_t maq_mode_label;

void changeTxt(mn_widget_t *wg, const char *str);
void widgetClick(mn_widget_t *wg, nt_touch_event_t event);
void widgetChangePic(mn_widget_t *wg, uint16_t pic, uint16_t pic2);
void widgetProgressBar(mn_widget_t *wg, uint8_t val);
void widgetGetPos(mn_widget_t *wg);
void widgetVisible(mn_widget_t *wg, nt_vis_event_t event);
void widgetTouchable(mn_widget_t *wg, nt_enable_t event);
void widgetSelRec(mn_widget_t *wg, uint8_t thickness, uint16_t color);

#endif /* MENU_WIDGET_H_ */
