/** @file screen.h
 *  @brief Function prototypes for the graphical widgets.
 *
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef MENU_SCREEN_H_
#define MENU_SCREEN_H_

#define SELECT_WIDTH 3

/**
 * @brief Defines the type of widget
 *
 */
typedef enum{
	SC_PAGE0 = 0,  	/*!<page0 screen */
	SC_PAGE1,    	/*!<page1 screen */
	SC_PAGE2,		/*!<page2 screen */
	SC_PAGE3,		/*!<page3 screen */
	SC_PAGE4,		/*!<page4 screen */
	SC_PAGE5,		/*!<page5 screen */
	SC_PAGE6,		/*!<page5 screen */
	SC_PAGE7,		/*!<page5 screen */
	SC_PAGE8,		/*!<page5 screen */
	SC_PAGE9,		/*!<page5 screen */
	SC_PAGE10,		/*!<page5 screen */
	SC_PAGE11,		/*!<page5 screen */
	SC_PAGE12,		/*!<page5 screen */
	SC_PAGE13,		/*!<page5 screen */
	SC_PAGE14,		/*!<page5 screen */
	SC_TOTAL
}mn_sc_id_t;

/**
 * @brief Defines the key pressed
 *
 */
typedef enum{
	SC_HANDLER = 0,		/*!< */
	SC_ATTACH,		/*!< */
	SC_DETACH,		/*!< */
	SC_KEY_ENTER,		/*!< */
	SC_KEY_ESC,		/*!< */
	SC_KEY_UP,  	/*!< */
	SC_KEY_DOWN,    	/*!< */
	SC_KEY_LEFT,		/*!< */
	SC_KEY_RIGHT,		/*!< */
	SC_KEY_ZUP,		/*!< */
	SC_KEY_ZDOWN,		/*!< */
	SC_KEY_RELEASE,		/*!< */
	SC_KEY_TOTAL,		/*!< */
}mn_sc_key_t;

/**
 * @brief Widget structure
 *
 */
typedef struct{
	mn_sc_id_t id;
	char name[10];
	int16_t wt_selected;
	mn_widget_t **p_widget;
	uint8_t widgetSize;
	mn_timer_t  **p_timer;
	void *p_args;
	void (*iif_func[SC_KEY_TOTAL])(void *p_arg);
}mn_screen_t;

extern mn_screen_t *page;
extern mn_screen_t splash_page;
extern mn_screen_t main_page;
extern mn_screen_t auto_page;
extern mn_screen_t warning_page;
extern mn_screen_t emergencia_page;
extern mn_screen_t jog_page;
extern mn_screen_t desloca_page;
extern mn_screen_t simPl_page;
extern mn_screen_t simOxi_page;
extern mn_screen_t selFiles_page;
extern mn_screen_t cutPl_page;
extern mn_screen_t cutOxi_page;
extern mn_screen_t keypad_page;
extern mn_screen_t cfgCutPl_page;
extern mn_screen_t cfgCutOx_page;
extern mn_screen_t load_page;
extern mn_screen_t selLines_page;
extern mn_screen_t limite_page;
extern mn_screen_t fInfoPl_page;
extern mn_screen_t fInfoOX_page;
extern mn_screen_t cfgMaq_page;
extern mn_screen_t manual_page;
extern mn_screen_t cfgParTHC_page;
extern mn_screen_t zerarmaq_page;
extern mn_screen_t cfgMaqPar_page;


extern void (*mn_func[SC_KEY_TOTAL])(void *p_arg);

void mn_screen_init (void);
void screenGetWidgetsInfo(mn_screen_t *p_screen);
void mn_screen_bind_keyboard(mn_screen_t *p_screen);
void mn_screen_change (mn_screen_t *p_screen, uint32_t event_in_change);
void mn_screen_change_image(mn_screen_t *p_screen, uint16_t pic);
void mn_screen_idle(void *p_arg);

#endif /* MENU_SCREEN_H_ */
