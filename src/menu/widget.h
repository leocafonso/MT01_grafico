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
	IMG_WARNING_1 = 55,
	IMG_WARNING_2 = 56,
	IMG_WARNING_3 = 57,
	IMG_WARNING_4 = 58,
	IMG_WARNING_5 = 59,
	IMG_WARNING_6 = 60,
	IMG_EMERGENCIA = 62,
	IMG_CONTINUAR = 61,
	IMG_CUIDADO = 61,
	IMG_FINALIZADO = 63,
	IMG_BTN_OK = 33,
	IMG_BTN_OK_PRESS = 32,
	IMG_BTN_PAUSE = 37,
	IMG_BTN_PAUSE_PRESS = 36,
	IMG_BTN_PLAY = 39,
	IMG_BTN_PLAY_PRESS = 38,
	IMG_LED_OFF = 28,
	IMG_LED_ON = 29,
	IMG_TOCHA_OFF = 53,
	IMG_TOCHA_ON = 52,
	IMG_PLASMA_NAO_TRANSF = 69,
	IMG_OHMICO = 70,
	IMG_ZERO_MAQ = 99,
	IMG_ZERO_MAQ_INIT = 100,
	IMG_ZERO_PECA = 101,
	IMG_SEM_ZERO_MAQ = 102,
	NO_IMG = 0xFFFF
};

/**
 * @brief Defines the type of widget
 *
 */
typedef enum{
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

void changeTxt(mn_widget_t *wg, const char *str);
void widgetClick(mn_widget_t *wg, nt_touch_event_t event);
void widgetChangePic(mn_widget_t *wg, uint16_t pic, uint16_t pic2);
void widgetProgressBar(mn_widget_t *wg, uint8_t val);
void widgetGetPos(mn_widget_t *wg);
void widgetVisible(mn_widget_t *wg, nt_vis_event_t event);
void widgetTouchable(mn_widget_t *wg, nt_enable_t event);
void widgetSelRec(mn_widget_t *wg, uint8_t thickness, uint16_t color);

#endif /* MENU_WIDGET_H_ */
