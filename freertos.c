/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "semphr.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern I2C_HandleTypeDef hi2c1;
SemaphoreHandle_t oledMutex = NULL;
volatile uint32_t g_counter = 0;
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartDisplayTask(void const * argument);
void StartCounterTask(void const * argument);
/* USER CODE END FunctionPrototypes */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* ================================================
 * DISPLAY TASK — runs every 500ms
 * Takes Mutex -> writes OLED -> gives Mutex -> sleeps
 * ================================================ */
void StartDisplayTask(void const * argument)
{
    if (oledMutex == NULL) {
        printf("[DISPLAY] ERROR: Mutex NULL. Exiting.\r\n");
        vTaskDelete(NULL);
        return;
    }

    printf("[DISPLAY] Calling ssd1306_Init...\r\n");
    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    printf("[DISPLAY] OLED initialized. Entering loop.\r\n");
    char line1[20], line2[20];

    for (;;)
    {
        if (xSemaphoreTake(oledMutex, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            /* === CRITICAL SECTION === */
            ssd1306_Fill(Black);

            snprintf(line1, sizeof(line1), "Counter: %lu",
                     (unsigned long)g_counter);
            ssd1306_SetCursor(0, 0);
            ssd1306_WriteString(line1, Font_7x10, White);

            uint32_t uptime = xTaskGetTickCount() / 1000;
            snprintf(line2, sizeof(line2), "Up: %lus",
                     (unsigned long)uptime);
            ssd1306_SetCursor(0, 20);
            ssd1306_WriteString(line2, Font_7x10, White);

            ssd1306_SetCursor(0, 40);
            ssd1306_WriteString("Project-3 FreeRTOS", Font_6x8, White);

            ssd1306_UpdateScreen();

            printf("[DISPLAY] Counter: %lu | Uptime: %lus\r\n",
                   (unsigned long)g_counter, (unsigned long)uptime);

            xSemaphoreGive(oledMutex); /* give BEFORE delay */
            /* === END CRITICAL SECTION === */
        }
        else
        {
            printf("[DISPLAY] WARNING: Mutex timeout.\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* ================================================
 * COUNTER TASK — runs every 1000ms
 * Takes Mutex -> increments counter -> gives Mutex -> sleeps
 * ================================================ */
void StartCounterTask(void const * argument)
{
    printf("[COUNTER] Started. Incrementing every 1000ms.\r\n");

    for (;;)
    {
        if (oledMutex != NULL)
        {
            if (xSemaphoreTake(oledMutex, pdMS_TO_TICKS(200)) == pdTRUE)
            {
                g_counter++;
                printf("[COUNTER] Count: %lu\r\n",
                       (unsigned long)g_counter);
                xSemaphoreGive(oledMutex);
            }
            else
            {
                printf("[COUNTER] WARNING: Mutex timeout.\r\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/* USER CODE END Application */

