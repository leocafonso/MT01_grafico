/** @file nextion.h
 *  @brief Function prototypes for the Nextion driver.
 *
 *  This contains the prototypes for the Nextion
 *  interface and eventually any macros, constants,
 *  or global variables you will need.
 *
 *  @author leocafonso
 *  @bug No known bugs.
 */

#ifndef NEXTION_NEXTION_H_
#define NEXTION_NEXTION_H_

/* Defines */
#define NEX_RET_CMD_FINISHED            (0x01)
#define NEX_RET_EVENT_LAUNCHED          (0x88)
#define NEX_RET_EVENT_UPGRADED          (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_INVALID_CMD             (0x00)
#define NEX_RET_INVALID_COMPONENT_ID    (0x02)
#define NEX_RET_INVALID_PAGE_ID         (0x03)
#define NEX_RET_INVALID_PICTURE_ID      (0x04)
#define NEX_RET_INVALID_FONT_ID         (0x05)
#define NEX_RET_INVALID_BAUD            (0x11)
#define NEX_RET_INVALID_VARIABLE        (0x1A)
#define NEX_RET_INVALID_OPERATION       (0x1B)

/**
 * @brief Defines the number of the picture preloaded in Nextion display
 *
 * Defines the number of the picture preloaded in Nextion display.
 * Becareful!!! if the sequence changes on Nextion memory this
 * enum sequence should be changed as well.
 */
typedef enum{
	COMPACTA_IMG = 0,  /*!<Compacta Background image */
	BACKGROUND_IMG,    /*!<Steel Background image */
	SETTINGS_ICON,     /*!<Settings icon image */
	CONFIG_CORTE_ICON, /*!<Cut icon image */
	FILE_ICON,         /*!<file search icon image */
	AUTO_ICON,         /*!<Auto Icon image */
	MANUAL_ICON,       /*!<Manual icon image */
}nt_img_t;

/**
 * @brief Nextion Position typedef
 *
 */
typedef struct{
	uint32_t x;
	uint32_t y;
	uint32_t w;
	uint32_t h;
}nt_pos_t;

typedef struct{
	uint8_t eventType;
	uint8_t pageID;
	uint8_t widgetID;
	uint8_t event;
}nt_touch_t;

/**
 * @brief Defines the touch event
 *
 */
typedef enum{
	NT_RELEASE,
	NT_PRESS,
}nt_touch_event_t;

/**
 * @brief Defines visable function
 *
 */
typedef enum{
	NT_HIDE,
	NT_SHOW,
}nt_vis_event_t;

/**
 * @brief Defines the touch function
 *
 */
typedef enum{
	NT_DISABLE,
	NT_ENABLE,
}nt_enable_t;

/** @brief Initialize Nextion display
 *
 * This function initialize Nextion display to return Success bytes (3 x 0xff) for every command.
 *
 */
bool nexInit(void);

bool Nex_sendXY(nt_enable_t event);

bool NexPage_ChangePic(const char *str,uint16_t pic);

/** @brief Change the page
 *
 * This function change the current page to a page specified by @param *name.
 *
 *  @param pageID - Page ID number. See Nextion project.
 *  @return 0 - Fail or 1 - Success
 */
bool NexPage_show(const char *str);
/** @brief Show a picture on the current page
 *
 * This function shows a picture on the current page. The picture needs to be preload on the Nextion memory.
 *
 *  @param x Picture initial position in X
 *  @param y Picture initial position in Y
 *  @param img_number Picture number. See Nextion picture sequence.
 *  @return 0 - Fail or 1 - Success
 */
bool NexPic_show(uint16_t x,uint16_t y,nt_img_t img_number);
/** @brief Show a string on the current page
 *
 * This function shows a string on the current page.
 *
 *  @param x: x coordinate starting point;
 *  @param y: y coordinate starting point;
 *  @param w: area width;
 *  @param h: area height;
 *  @param fontID: Font ID;
 *  @param fcolor: Font color;
 *  @param bcolor: Background color (when set sta as Crop Image or Image, backcolor means image ID );
 *  @param xcenter: Horizontal alignment (0 is left-aligned, 1 is centered, 2 is right-aligned);
 *  @param ycenter: Vertical alignment (0 is upper-aligned, 1 is centered, 2 is lower-aligned);
 *  @param sta: Background fill(0-crop image;1-solid color;2-Image; 3-No backcolor, when set sta as Crop Image or Image, backcolor means image ID);
 *  @param str: Character content;
 *  @return 0 - Fail or 1 - Success
 */
bool NexStr_show(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t fontID, uint16_t fcolor,uint16_t bcolor,
		uint8_t xcenter,uint8_t ycenter, uint8_t sta, const char *str);
/** @brief Get variable/constant value with format
 *
 * Get variable/constant value with format
 *
 *  @param att: variable name
 *  @param number: return the variable result
 *  @return 0 - Fail or 1 - Success
 */
bool NexGet_num(const char *str,uint32_t *number);
/** @brief Draw a rectangle x,y,x2,y2
 *
 * Draw a rectangle x,y,x2,y2
 *
 *  @param x: x coordinate starting point;
 *  @param y: y coordinate starting point;
 *  @param x2: x coordinate ending point;
 *  @param y2: y coordinate ending point;
 *  @param color: Line color;
 *  @return 0 - Fail or 1 - Success
 */
bool NexDraw_rec(nt_pos_t pos,uint16_t color);
/** @brief Send click command to Nextion display
 *
 *
 *  @param str: widget name;
 *  @param event: event 0 - Release 1 - Press;
 *  @return 0 - Fail or 1 - Success
 */
bool NexWidget_click(const char *str, nt_touch_event_t event);

bool NexWidget_ChangePic(const char *str,uint16_t pic);

bool NexWidget_ChangePic2(const char *str,uint16_t pic)	;

bool NexWidget_ProgressBar(const char *str,uint8_t val);

/** @brief Change the text on a widget
 *
 *
 *  @param str: widget name;
 *  @param txt: widget text;
 *  @return 0 - Fail or 1 - Success
 */
bool NexWidget_txt(const char *str,const char *txt);

bool NexWidget_visible(const char *str,nt_vis_event_t event);

bool NexWidget_touchable(const char *str,nt_enable_t event);

bool NexTouch_recv(nt_touch_t *buf, uint32_t timeout);

bool NexUpload_checkFile(void);

bool NexUpload_setDownloadBaudrate(uint32_t baudrate);

bool NexUpload_downloadTftFile(void);

bool NexUpload_waitingReset(uint16_t timeout);

#endif /* NEXTION_NEXTION_H_ */
