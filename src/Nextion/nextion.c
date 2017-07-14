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
#include "ff.h"

#define NEX_RET_TIMEOUT       			(500Ul)

/* Static functions */
static void sciCallback(void *p_args);
static bool sendCommand(const char* cmd);
static bool recvRetCommandFinished(uint32_t timeout);
static bool recvRetNumber(uint32_t *number, uint32_t timeout);
static uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout);
static uint16_t NexUpload_recvRetString(char *buffer, uint32_t timeout,bool recv_flag);

/* Static variables and const */
static bool recvRetOk = false, recvOk = false;
static uint8_t recvBuffer[20];
static uint8_t recvBufferTouch[20];
static uint8_t recvBufferRetOK[20];
static sci_hdl_t console;
static const sci_uart_t config = {
	    .baud_rate = 9600,     // ie 9600, 19200, 115200
		.clk_src = SCI_CLK_INT,
		.data_size = SCI_DATA_8BIT,
		.parity_en = SCI_PARITY_OFF,
		.parity_type = SCI_EVEN_PARITY,
		.stop_bits =SCI_STOPBITS_1,
	    .int_priority = 4,   // txi, tei, rxi INT priority; 1=low, 15=high
};
static FIL         fileNextion;
static uint32_t _undownloadByte;
char *cmd;
char *string;
/* extern variables */
extern bool drivemountFlag;


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
						memcpy(recvBufferTouch,recvBuffer,20);
						recvOk = true;
					}
					else
					{
						memcpy(recvBufferRetOK,recvBuffer,20);
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
	//recvRetOk = false;
	while(recvRetOk == false && (timelapsed < timeout))
	{
		timelapsed++;
		vTaskDelay(1/portTICK_PERIOD_MS);
	}
	recvRetOk = false;
	if (recvBufferRetOK[0] == NEX_RET_CMD_FINISHED
		&& recvBufferRetOK[1] == 0xFF
		&& recvBufferRetOK[2] == 0xFF
		&& recvBufferRetOK[3] == 0xFF)
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
	//recvRetOk = false;
	while(recvRetOk == false && (timelapsed < timeout))
	{
		timelapsed++;
		vTaskDelay(1/portTICK_PERIOD_MS);
	}
	recvRetOk = false;
	if (recvBufferRetOK[0] == NEX_RET_NUMBER_HEAD
		&& recvBufferRetOK[5] == 0xFF
		&& recvBufferRetOK[6] == 0xFF
		&& recvBufferRetOK[7] == 0xFF)
	{
		*number = ((uint32_t)recvBufferRetOK[4] << 24) | ((uint32_t)recvBufferRetOK[3] << 16) | (recvBufferRetOK[2] << 8) | (recvBufferRetOK[1]);
		ret = true;
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

static uint16_t NexUpload_recvRetString(char *buffer, uint32_t timeout,bool recv_flag)
{
	uint16_t ret = 0;
	uint8_t c = 0;
	bool exit_flag = false;
	uint32_t start;
	start = xTaskGetTickCount();
	while(xTaskGetTickCount() - start <= timeout)
	{
		while (R_SCI_Receive(console,&c,1) == SCI_SUCCESS)
		{
			if (c == 0)
			{
				continue;
			}
			snprintf(buffer,50,"%s%s",buffer,&c);
			if(recv_flag)
			{
				if(strstr(buffer,"\x05") != NULL)
				{
					exit_flag = true;
				}
			}
		}
		if(exit_flag)
		{
			break;
		}
	}
	ret = strlen(buffer);
	return ret;
}

/************************** Public functions *********************************************/
uint16_t num = 0;
bool nexInit(void)
{
	sci_baud_t baud;
    bool ret1 = false;
	R_SCI_Open(SCI_CH2,SCI_MODE_ASYNC,(void *)&config,sciCallback,&console);
    sendCommand("");
	sendCommand("bkcmd=1");
	recvRetCommandFinished(NEX_RET_TIMEOUT);
    sendCommand("baud=115200");
	vTaskDelay(50/portTICK_PERIOD_MS);
	baud.pclk = 48000000;
	baud.rate = 115200;
	R_SCI_Control(console,SCI_CMD_CHANGE_BAUD,&baud);
    ret1 = recvRetCommandFinished(NEX_RET_TIMEOUT);
    sendCommand("");
    sendCommand("page 0");
    ret1 = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret1;
}

bool Nex_sendXY(nt_enable_t event)
{
	bool ret = false;
 //   char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"sendxy=%d",event);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexPage_ChangePic(const char *str,uint16_t pic)
{
	bool ret = false;
	 // char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"%s.pic=%d",str,pic);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexPage_show(const char *str)
{
	bool ret = false;
   // char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"page %s",str);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexPic_show(uint16_t x,uint16_t y,nt_img_t img_number)
{
	bool ret = false;
   // char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"pic %d,%d,%d",x,y,img_number);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexStr_show(uint16_t x,uint16_t y,uint16_t w,uint16_t h, uint8_t fontID, uint16_t fcolor,uint16_t bcolor,
		uint8_t xcenter,uint8_t ycenter, uint8_t sta, const char *str)
{
	bool ret = false;
//    char *cmd;
    cmd = pvPortMalloc( 100 );
	snprintf(cmd,100,"xstr %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\"%s\"",x,y,w,h,fontID,fcolor,bcolor,xcenter,ycenter,sta,str);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexGet_num(const char *str,uint32_t *number)
{
	bool ret = false;
 //   char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"get %s",str);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetNumber(number,NEX_RET_TIMEOUT);
    return ret;
}

bool NexDraw_rec(nt_pos_t pos,uint16_t color)
{
	bool ret = false;
 //   char *cmd;
    cmd = pvPortMalloc( 50 );
	uint16_t x2, y2;
	x2 = pos.x + pos.w;
	y2 = pos.y + pos.h;
	snprintf(cmd,50,"draw %d,%d,%d,%d,%d",pos.x,pos.y,x2,y2,color);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_click(const char *str, nt_touch_event_t event)
{
	bool ret = false;
 //   char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"click %s,%d",str,event);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ChangePic(const char *str,uint16_t pic)
{
	bool ret = false;
//    char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"%s.pic=%d",str,pic);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ChangePic2(const char *str,uint16_t pic)
{
	bool ret = false;
//    char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"%s.pic2=%d",str,pic);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_ProgressBar(const char *str,uint8_t val)
{
	bool ret = false;
    char *//char *cmd;;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"%s.val=%d",str,val);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}


bool NexWidget_txt(const char *str,const char *txt)
{
	bool ret = false;
    //char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"%s.txt=\"%s\"",str,txt);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_visible(const char *str,nt_vis_event_t event)
{
	bool ret = false;
    //char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"vis %s,%d",str,event);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}

bool NexWidget_touchable(const char *str,nt_enable_t event)
{
	bool ret = false;
    //char *cmd;
    cmd = pvPortMalloc( 50 );
	snprintf(cmd,50,"tsw %s,%d",str,event);
	sendCommand(cmd);
	vPortFree(cmd);
	ret = recvRetCommandFinished(NEX_RET_TIMEOUT);
    return ret;
}



bool NexTouch_recv(nt_touch_t *buf, uint32_t timeout)
{
    bool ret = false;
    if(recvOk == true)
    {
    	recvOk = false;
		if (recvBufferTouch[0] == NEX_RET_EVENT_TOUCH_HEAD
			&& recvBufferTouch[4] == 0xFF
			&& recvBufferTouch[5] == 0xFF
			&& recvBufferTouch[6] == 0xFF)
		{
			buf->eventType = recvBufferTouch[0];
			buf->pageID = recvBufferTouch[1];
			buf->widgetID = recvBufferTouch[2];
			buf->event = recvBufferTouch[3];
			ret = true;
		}
		else  if (recvBufferTouch[0] == NEX_RET_EVENT_POSITION_HEAD
				&& recvBufferTouch[6] == 0xFF
				&& recvBufferTouch[7] == 0xFF
				&& recvBufferTouch[8] == 0xFF)
		{
			buf->eventType = recvBufferTouch[0];
			buf->event = recvBufferTouch[5];
			ret = true;
		}
    }
    return ret;
}

FILINFO     file_info;
bool NexUpload_checkFile(void)
{
	bool ret = false;

	DIR			dj;
    FRESULT     res;
	if(drivemountFlag){
		if (f_findfirst(&dj, &file_info, "", "MT02*.tft"))
		{
			return ret;
		}
		if (f_open(&fileNextion, file_info.fname, FA_READ) != FR_OK)
		{
			return ret;
		}
		if (f_close(&fileNextion) != FR_OK)
		{
			return ret;
		}
		_undownloadByte = file_info.fsize;
		ret = true;
	}
	else
	{
		//TODO USB not enumerated
	}
	return ret;
}

bool NexUpload_searchBaudrate(uint32_t baudrate)
{
	char string[50] = "";
    sendCommand("");
    sendCommand("connect");
    NexUpload_recvRetString(string, 100,false);
    if(strstr(string,"comok") != NULL)
    {
        return 1;
    }
    return 0;
}

bool NexUpload_setDownloadBaudrate(uint32_t baudrate)
{
	bool ret = false;
	sci_baud_t baud;
    //char *string;
    //char *cmd;
    string = pvPortMalloc( 50 );
    cmd = pvPortMalloc( 50 );
	memset(string,'\0',50);
	memset(cmd,'\0',50);
    snprintf(cmd,50,"whmi-wri %d,%d,0",_undownloadByte,baudrate);
    sendCommand("");
    sendCommand(cmd);
	vTaskDelay(50/portTICK_PERIOD_MS);
	baud.pclk = 48000000;
	baud.rate = baudrate;
	if (R_SCI_Control(console,SCI_CMD_CHANGE_BAUD,&baud) != SCI_SUCCESS)
	{
		while(1);
	}
    NexUpload_recvRetString(string, 1000,false);
	if(strstr(string,"\x05") != NULL)
    {
		ret = true;
    }
	vPortFree(string);
	vPortFree(cmd);
    return ret;
}

bool NexUpload_downloadTftFile(void)
{
	char str[20];
	uint8_t *temp = NULL;
	uint32_t remain;
	char *p_str;
    //char *string;
    string = pvPortMalloc( 50 );
	temp = pvPortMalloc( 4096 );
	memset(string,'\0',50);
	memset(temp,'\0',4096);
	f_open(&fileNextion, file_info.fname, FA_READ | FA_WRITE);
	while(!f_eof(&fileNextion))
	{
		WDT_FEED
		f_read (&fileNextion, temp, 4096, (UINT *)&remain);			/* Read data from the file */
		if (remain == 0)
		{
			for (uint16_t i = 0; i < 4096; i++)
			{
				while(R_SCI_Send(console,&temp[i],1) != SCI_SUCCESS);
			}
		}
		else
		{
			for (uint16_t i = 0; i < remain; i++)
			{
				while(R_SCI_Send(console,&temp[i],1) != SCI_SUCCESS);
			}
		}
	    NexUpload_recvRetString(string,500,true);
		if(strstr(string,"\x05") != NULL)
	    {
	    	strcpy(string,"");
	    }
	    else
	    {
	    	//break;
	    }
	}
	strcpy(str,file_info.fname);
	p_str = strstr(str,".tft");
	strncpy(p_str,".TDN",4);
	f_rename(file_info.fname, str);
	f_close(&fileNextion);
	vPortFree(temp);
	return 0;
}

bool NexUpload_waitingReset(uint16_t timeout)
{
	uint8_t c = 0;
	bool exit_flag = false;
	uint32_t start;
	start = xTaskGetTickCount();
	while(xTaskGetTickCount() - start <= timeout)
	{
		while (R_SCI_Receive(console,&c,1) == SCI_SUCCESS)
		{
			if (c == 0)
			{
				continue;
			}
			if(c==0x88)
			{
				exit_flag = true;
			}
		}
		if(exit_flag)
		{
			while(1);
			break;
		}
	}
	return exit_flag;
}

