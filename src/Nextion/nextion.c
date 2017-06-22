/** @file Nextion.c
 *  @brief A Nextion driver.
 *
 *  This file contains some implementation of Nextion commands.
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
#include "r_sci_async_rx_if.h"
#include "nextion.h"

#define NEX_RET_TIMEOUT       			(500Ul)

/* Static functions */
static void sciCallback(void *p_args);
static bool sendCommand(const char* cmd);
static bool recvRetCommandFinished(uint32_t timeout);
static bool recvRetNumber(uint32_t *number, uint32_t timeout);
static uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout);

/* Static variables and const */
static bool recvRetOk = false, recvOk = false;
uint8_t recvBuffer[20];
static sci_hdl_t console;
const sci_uart_t config = {
	    .baud_rate = 9600,     // ie 9600, 19200, 115200
		.clk_src = SCI_CLK_INT,
		.data_size = SCI_DATA_8BIT,
		.parity_en = SCI_PARITY_OFF,
		.parity_type = SCI_EVEN_PARITY,
		.stop_bits =SCI_STOPBITS_1,
	    .int_priority = 4,   // txi, tei, rxi INT priority; 1=low, 15=high
};

/* extern variables */



/************************** Static functions *********************************************/
static void sciCallback(void *p_args)
{
	sci_cb_args_t *p_sci_args = p_args;
	uint8_t remain = 0;
	static uint8_t cnt0xff = 0;
	if (p_sci_args->event == SCI_EVT_RX_CHAR)
	{
		if (p_sci_args->byte == 0xff)
		{
			cnt0xff++;
			if (cnt0xff == 3)
			{
			    R_SCI_Control(console,SCI_CMD_RX_Q_BYTES_AVAIL_TO_READ,&remain);
				if ((R_SCI_Receive(console,recvBuffer,remain)) == SCI_SUCCESS)
				{
					if (recvBuffer[0] == NEX_RET_EVENT_TOUCH_HEAD || recvBuffer[0] == NEX_RET_EVENT_POSITION_HEAD)
					{
						recvOk = true;
					}
					else
					{
						recvRetOk = true;
					}

				}
			}
		}
		else
		{
			cnt0xff = 0;
		}
	}
}

static bool sendCommand(const char* cmd)
{
	uint8_t p = 0xFF;
	bool ret = false;
	R_SCI_Send(console,(uint8_t *)cmd,strlen(cmd));
	R_SCI_Send(console,&p,1);
	R_SCI_Send(console,&p,1);
	R_SCI_Send(console,&p,1);
	ret = true;
	return ret;
}

static bool recvRetCommandFinished(uint32_t timeout)
{
    bool ret = false;
	uint32_t timelapsed = 0;
	while(recvRetOk == false && (timelapsed < timeout))
	{
		timelapsed++;
		vTaskDelay(1/portTICK_PERIOD_MS);
	}
	recvRetOk = false;
	if (recvBuffer[0] == NEX_RET_CMD_FINISHED
		&& recvBuffer[1] == 0xFF
		&& recvBuffer[2] == 0xFF
		&& recvBuffer[3] == 0xFF)
	{
		ret = true;
	}
	else
	{
		ret = false;
	}
    return ret;
}

static bool recvRetNumber(uint32_t *number, uint32_t timeout)
{
	uint32_t timelapsed = 0;
    bool ret = false;
	while(recvRetOk == false && (timelapsed < timeout))
	{
		timelapsed++;
		vTaskDelay(1/portTICK_PERIOD_MS);
	}
	recvRetOk = false;
	if (recvBuffer[0] == NEX_RET_NUMBER_HEAD
		&& recvBuffer[5] == 0xFF
		&& recvBuffer[6] == 0xFF
		&& recvBuffer[7] == 0xFF)
	{
		*number = ((uint32_t)recvBuffer[4] << 24) | ((uint32_t)recvBuffer[3] << 16) | (recvBuffer[2] << 8) | (recvBuffer[1]);
	}
    return ret;
}

static uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout)
{
	uint32_t lRet = pdFALSE;
	uint16_t ret = 0;

//    memset(temp,0x00,sizeof(temp));
//    rxIndex = 0;
//    cnt_0xff = 0;
//    lRet = xSemaphoreTake(xSemSCI,timeout);
//    cnt_0xff = 0;
//    if(lRet == true)
//    {
//        if (temp[0] == NEX_RET_STRING_HEAD)
//        {
//            ret = strlen((const char *)temp);
//            ret = ret > len ? len : ret;
//            strncpy(buffer, (const char *)temp, ret);
//        }
//    }
//    memset(temp,0x00,sizeof(temp));
    return ret;
}

/************************** Public functions *********************************************/
uint16_t num = 0;
bool nexInit(void)
{
    bool ret1 = false;
	R_SCI_Open(SCI_CH2,SCI_MODE_ASYNC,(void *)&config,sciCallback,&console);
    sendCommand("");
    sendCommand("bkcmd=1");
    recvRetCommandFinished(NEX_RET_TIMEOUT);
    sendCommand("page 0");
    ret1 = recvRetCommandFinished(NEX_RET_TIMEOUT);
    Nex_sendXY(NT_DISABLE);
    return ret1;
}

bool Nex_sendXY(nt_enable_t event)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"sendxy=%d",event);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexPage_ChangePic(const char *str,uint16_t pic)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"%s.pic=%d",str,pic);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexPage_show(const char *str)
{
	bool ret = false;
	char cmd[20];
	snprintf(cmd,sizeof(cmd),"page %s",str);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexPic_show(uint16_t x,uint16_t y,nt_img_t img_number)
{
	bool ret = false;
	char cmd[20];
	snprintf(cmd,sizeof(cmd),"pic %d,%d,%d",x,y,img_number);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexStr_show(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t fontID, uint16_t fcolor,uint16_t bcolor,
		uint8_t xcenter,uint8_t ycenter, uint8_t sta, const char *str)
{
	bool ret = false;
	char cmd[60];
	snprintf(cmd,sizeof(cmd),"xstr %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\"",x,y,w,h,fontID,fcolor,bcolor,xcenter,ycenter,sta,str);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexGet_num(const char *str,uint32_t *number)
{
	bool ret = false;
	char cmd[20];
	snprintf(cmd,sizeof(cmd),"get %s",str);
	sendCommand(cmd);
	ret = recvRetNumber(number,NEX_RET_TIMEOUT);
    return ret;
}

bool NexDraw_rec(nt_pos_t pos,uint16_t color)
{
	bool ret = false;
	char cmd[50];
	uint16_t x2, y2;
	x2 = pos.x + pos.w;
	y2 = pos.y + pos.h;
	snprintf(cmd,sizeof(cmd),"draw %d,%d,%d,%d,%d",pos.x,pos.y,x2,y2,color);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_click(const char *str, nt_touch_event_t event)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"click %s,%d",str,event);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ChangePic(const char *str,uint16_t pic)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"%s.pic=%d",str,pic);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ChangePic2(const char *str,uint16_t pic)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"%s.pic2=%d",str,pic);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ProgressBar(const char *str,uint8_t val)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"%s.val=%d",str,val);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexWidget_txt(const char *str,const char *txt)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"%s.txt=\"%s\"",str,txt);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_visible(const char *str,nt_vis_event_t event)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"vis %s,%d",str,event);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_touchable(const char *str,nt_enable_t event)
{
	bool ret = false;
	char cmd[50];
	snprintf(cmd,sizeof(cmd),"tsw %s,%d",str,event);
	sendCommand(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}



bool NexTouch_recv(nt_touch_t *buf, uint32_t timeout)
{
    bool ret = false;
    if(recvOk == true)
    {
    	recvOk = false;
		if (recvBuffer[0] == NEX_RET_EVENT_TOUCH_HEAD
			&& recvBuffer[4] == 0xFF
			&& recvBuffer[5] == 0xFF
			&& recvBuffer[6] == 0xFF)
		{
			buf->eventType = recvBuffer[0];
			buf->pageID = recvBuffer[1];
			buf->widgetID = recvBuffer[2];
			buf->event = recvBuffer[3];
			ret = true;
		}
		else  if (recvBuffer[0] == NEX_RET_EVENT_POSITION_HEAD
				&& recvBuffer[6] == 0xFF
				&& recvBuffer[7] == 0xFF
				&& recvBuffer[8] == 0xFF)
		{
			buf->eventType = recvBuffer[0];
			buf->event = recvBuffer[5];
			ret = true;
		}
    }
    return ret;
}

