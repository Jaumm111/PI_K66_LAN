/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

//#include "FreeRTOS_sockets.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hello_task(void *pvParameters);
static void hello_taskest(void *pvParameters);
static void vUDPSendUsingStandardInterface( void *pvParameters );

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    if (xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY-1, NULL) !=
        pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }
    if (xTaskCreate(hello_taskest, "Hello_taskest", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL) !=
            pdPASS)
        {
            PRINTF("Task creation failed!.\r\n");
            while (1)
                ;
        }
    vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters)
{
    for (;;)
    {
        PRINTF("Tarefa1.\r\n");
        vTaskDelay(100);
    }
}
static void hello_taskest(void *pvParameters)
{
    for (;;)
    {
        PRINTF("Tarefa2.\r\n");
        vTaskDelay(150);
    }
}


static void vUDPSendUsingStandardInterface( void *pvParameters )
{
Socket_t xSocket;
struct freertos_sockaddr xDestinationAddress;
uint8_t cString[ 50 ];
uint32_t ulCount = 0UL;
const TickType_t x1000ms = 1000UL / portTICK_PERIOD_MS;

   /* Send strings to port 10000 on IP address 192.168.0.50. */
   memset( &xDestinationAddress, 0, sizeof(xDestinationAddress) );
   xDestinationAddress.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr( "192.168.0.50" );
   xDestinationAddress.sin_family = FREERTOS_AF_INET4;
   xDestinationAddress.sin_port = FreeRTOS_htons( 10000 );

   /* Create the socket. */
   xSocket = FreeRTOS_socket( FREERTOS_AF_INET, /* Used for IPv4 UDP socket. */
                                 /* FREERTOS_AF_INET6 can be used for IPv6 UDP socket. */
                              FREERTOS_SOCK_DGRAM,/*FREERTOS_SOCK_DGRAM for UDP.*/
                              FREERTOS_IPPROTO_UDP );

   /* Check the socket was created. */
   configASSERT( xSocket != FREERTOS_INVALID_SOCKET );

   /* NOTE: FreeRTOS_bind() is not called. This will only work if
      ipconfigALLOW_SOCKET_SEND_WITHOUT_BIND is set to 1 in FreeRTOSIPConfig.h. */

   for( ;; )
   {
       /* Create the string that is sent. */
       sprintf( cString,
                "Standard send message number %lurn",
                ulCount );

       /* Send the string to the UDP socket. ulFlags is set to 0, so the standard
          semantics are used. That means the data from cString[] is copied
          into a network buffer inside FreeRTOS_sendto(), and cString[] can be
          reused as soon as FreeRTOS_sendto() has returned. */
       FreeRTOS_sendto( xSocket,
                        cString,
                        strlen( cString ),
                        0,
                        &xDestinationAddress,
                        sizeof( xDestinationAddress ) );

       ulCount++;

       /* Wait until it is time to send again. */
       vTaskDelay( x1000ms );
   }
}


