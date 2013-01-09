//*****************************************************************************
//
//! \file ReadAccData.c
//! \brief Show how to read accelerometer data.
//! \version V2.1.1.0
//! \date 4/1/2013
//! \author CooCox
//! \copy
//!
//! Copyright (c)  2011, CooCox
//! All rights reserved.
//!
//! Redistribution and use in source and binary forms, with or without
//! modification, are permitted provided that the following conditions
//! are met:
//!
//!     * Redistributions of source code must retain the above copyright
//! notice, this list of conditions and the following disclaimer.
//!     * Redistributions in binary form must reproduce the above copyright
//! notice, this list of conditions and the following disclaimer in the
//! documentation and/or other materials provided with the distribution.
//!     * Neither the name of the <ORGANIZATION> nor the names of its
//! contributors may be used to endorse or promote products derived
//! from this software without specific prior written permission.
//!
//! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//! AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//! IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//! ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//! LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//! CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//! SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//! INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//! CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//! ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
//! THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

#include <stdio.h>

#if defined(gcc) || defined(__GNUC__) || defined(coide)
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#undef errno
extern int errno;
extern int  _end;
#endif

#include "xhw_ints.h"
#include "xhw_nvic.h"
#include "xhw_memmap.h"
#include "xhw_types.h"
#include "xdebug.h"
#include "xcore.h"
#include "xsysctl.h"
#include "xhw_sysctl.h"
#include "xhw_gpio.h"
#include "xgpio.h"
#include "xhw_i2c.h"
#include "xi2c.h"
#include "xuart.h"
#include "LIS302DL.h"

#define TICK_S 0xFFFFF
#define TICK_L 0x2FFFFF

//*****************************************************************************
//
//! Internal Function (Used in this file only!)
//
//*****************************************************************************

static void SysInit(void);
static void UartInit(void);

char * WelcomeInfo =
{
"\t---------------------------------------------------------\r\n"
"\t|  CooCox Open Source Driver Project                    |\r\n"
"\t|  Module : Accelerometer KIS302DL Driver               |\r\n"
"\t|  author : CooCox Cedar                                |\r\n"
"\t|  version: V1.0                                        |\r\n"
"\t|  More information, please visit www.coocox.org        |\r\n"
"\t---------------------------------------------------------\r\n"
"\r\nSelftest Beginning!\r\n"
};

#if defined(gcc) || defined(__GNUC__) || defined(coide)
caddr_t _sbrk ( int incr )
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&_end;
  }
  prev_heap = heap;

  heap += incr;

  return (caddr_t) prev_heap;
}

int link(char *old, char *new) {
return -1;
}

int _close(int file)
{
  return -1;
}

int _fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int file)
{
  return 1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _read(int file, char *ptr, int len)
{
  return 0;
}

int _write(int file, char *ptr, int len)
{

	int i = 0;
	(void) file;

	for(i = 0; i < len; i++)
	{
		xUARTCharPut(USART2_BASE,*ptr++);
	}

	return len;
}

void abort(void)
{
  /* Abort called */
  while(1);
}
#endif

#if defined(ewarm) || defined(__ICCARM__)
int putchar(int c)
{
    xUARTCharPut(USART2_BASE, (char)c);
    return (1);
}
#endif

void ReadAccData(void)
{
    uint8_t value = 0;
    Result   retv = SUCCESS;

    //
    // Init the LED
    //
    //xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOD);
    //xGPIODirModeSet(xGPIO_PORTD_BASE, xGPIO_PIN_3, xGPIO_DIR_MODE_OUT);

    SysInit();
    UartInit();

    printf("%s\r\n", WelcomeInfo);

    //Initialize low hardware interface
    retv = LIS302DLInit();
    if(retv != SUCCESS)
    {
        printf("LIS302 Init Failure\r\n");
        return;
    }

    //Wait LIS302DL boot, this time must be >= 3ms
    SysCtlDelay(TICK_L);

    //Running LIS302DL
    LIS302DLStart();

    printf("Initial success, Now begin to read data\r\n");

    while (1)
    {
        //Read X Axis Accelerometer data
        retv = LIS302DLAccDataRead(AXIS_X, &value);
        if(retv != SUCCESS)
        {
            printf("Read X Axis data Failure\r\n");
        }
        else
        {
            printf("X Axis data:%d\r\n", (int8_t)value);
        }

        //Read Y Axis Accelerometer data
        retv = LIS302DLAccDataRead(AXIS_Y, &value);
        if(retv != SUCCESS)
        {
            printf("Read Y Axis data Failure\r\n");
        }
        else
        {
            printf("Y Axis data:%d\r\n", (int8_t)value);
        }

        //Read Z Axis Accelerometer data
        retv = LIS302DLAccDataRead(AXIS_Z, &value);
        if(retv != SUCCESS)
        {
            printf("Read Z Axis data Failure\r\n");
        }
        else
        {
            printf("Z Axis data:%d\r\n\r\n", (int8_t)value);
        }
        
        SysCtlDelay(TICK_S);
    }
}


////////////////////////////////////////////////////////////////////////////////
static void SysInit(void)
{
    xSysCtlClockSet(72000000,  xSYSCTL_OSC_MAIN | xSYSCTL_XTAL_25MHZ);
    SysCtlDelay(10000);
}

static void UartInit(void)
{
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_GPIOD);
    xSysCtlPeripheralEnable(SYSCTL_PERIPH_AFIO);

    //xSPinTypeUART(UART2RX,PD6);
    xSPinTypeUART(UART2TX,PD5);

    xSysCtlPeripheralReset(xSYSCTL_PERIPH_UART2);
    xSysCtlPeripheralEnable(xSYSCTL_PERIPH_UART2);

    xUARTConfigSet(USART2_BASE, 115200, (UART_CONFIG_WLEN_8 |
                                         UART_CONFIG_STOP_ONE |
                                         UART_CONFIG_PAR_NONE));

    xUARTEnable(USART2_BASE, (UART_BLOCK_UART | UART_BLOCK_TX | UART_BLOCK_RX));
}

