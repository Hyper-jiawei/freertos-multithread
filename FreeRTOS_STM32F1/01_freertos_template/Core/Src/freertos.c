/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_oled.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

static StackType_t g_pucStackOfLightTask[128];
static StaticTask_t g_TCBofLightTask;
static TaskHandle_t xLightTaskHandle;

static StackType_t g_pucStackOfColorTask[128];
static StaticTask_t g_TCBofColorTask;
static TaskHandle_t xColorTaskHandle;
//任务创建环节,为"声光色影"模块设置的句柄


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;

const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/*这段程序用于测试多个task共用一个函数的例子*/
struct PrintTask
{
  uint32_t x;
  uint32_t y;
  char name[16];
};
static struct PrintTask task1 = {0,0,"Task1"};
static struct PrintTask task2 = {0,2,"Task2"};
static struct PrintTask task3 = {0,4,"Task3"};
static struct PrintTask task4 = {0,6,"Task4"};
int OLEDeable = 1;



/*这段程序用于测试多个task共用一个函数的例子/两种Delay函数延时时间的例子*/
void PrintTask(void *parameters)
{
  struct PrintTask* taskinfo = parameters;
	uint32_t cnt = 0;
  uint16_t len;
  uint32_t t1 , t2;//记录两个时间节点
  BaseType_t preTime; //记录上一个时间节点
  preTime = xTaskGetTickCount(); //获取当前的时间节点
  while(1)
  {
          t1 = system_get_ns(); //记录第一个时间节点
    if(OLEDeable)
    {
      OLEDeable = 0;
      char buf[16];
      len = LCD_PrintString(taskinfo->x, taskinfo->y, taskinfo->name);
      len += LCD_PrintString(len, taskinfo->y, ":");                
      LCD_PrintSignedVal(len, taskinfo->y, cnt++);

      mdelay(cnt & 0x3); //模拟任务运行时间

      /*主要看以下这两个函数的延时区别*/

		 //vTaskDelay(500);                   //这个函数导致的结果使t2-t1大约是500ms，不确定
		 vTaskDelayUntil(&preTime, 500);  //这个函数使t2-t1应该是确定的
		t2 = system_get_ns(); //记录第二个时间节点
		
		LCD_ClearLine(taskinfo->x, taskinfo->y+2);
		LCD_PrintSignedVal(taskinfo->x, taskinfo->y+2, t2-t1); //计算任务运行时间
    //在下一行打印任务运行时间
      OLEDeable = 1;

    }

  }
}


/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	
	TaskHandle_t xSoundTaskHandle;
	BaseType_t ret;
	
  LCD_Init();
	LCD_Clear();

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */ 

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* task for MUSIC*/
  //extern void PlayMusic(void *params);
  //ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &xSoundTaskHandle);

  /* task for LED*/
  xLightTaskHandle = xTaskCreateStatic(Led_Test, "LightTask", 128, NULL, osPriorityNormal, g_pucStackOfLightTask, &g_TCBofLightTask);

  /* task for ColorLED_Test*/
  //xColorTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorTask", 128, NULL, osPriorityNormal, g_pucStackOfColorTask, &g_TCBofColorTask);
 
  

  //xTaskCreate(PrintTask, "PrintTask1", 128, &task1, osPriorityNormal, NULL);
  //xTaskCreate(PrintTask, "PrintTask2", 128, &task2, osPriorityNormal, NULL);
  //xTaskCreate(PrintTask, "PrintTask3", 128, &task3, osPriorityNormal, NULL);
  //TaskCreate(PrintTask, "PrintTask4", 128, &task4, osPriorityNormal, NULL);

 



  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    uint8_t dev, data;
    int len;
  
    TaskHandle_t xSoundTaskHandle = NULL;
	  BaseType_t ret;

    int beRunning;


    LCD_Init();
	  LCD_Clear();

    IRReceiver_Init();
	  LCD_PrintString(0, 0, "Waiting control");



    /*以下是关于任务管理（创建/删除/挂起/阻塞）的实验程序 */
    // while (1)
    // {
    //     /* 接收遥控数据 */
		// if (0 == IRReceiver_Read(&dev, &data))
		// {		
		// 	if (data == 0xa8) /* play */
		// 	{
		// 		/* play按键，创建任务 */
		// 	  extern void PlayMusic(void *params);
		// 	  if (xSoundTaskHandle == NULL) // 还未创建任务
		// 	  {
		// 			LCD_ClearLine(0, 0);
		// 			LCD_PrintString(0, 0, "Create Task");
		// 			ret = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal+1, &xSoundTaskHandle);
    //       //优先级比平时提升了一级

    //       beRunning = 1;
		// 	  }
    //     else
    //     {
    //       if (beRunning)
    //       {
    //         vTaskSuspend(xSoundTaskHandle);
    //         //挂起任务
    //         PassiveBuzzer_Control(0); /* 主动停止蜂鸣器 */
    //         LCD_ClearLine(0, 0);
		// 			 LCD_PrintString(0, 0, "suspend Task");
    //         beRunning = 0;
    //       }
    //       else
    //       {
    //         vTaskResume(xSoundTaskHandle);
    //         //恢复任务
    //         LCD_ClearLine(0, 0);
		// 			 LCD_PrintString(0, 0, "Resume Task");
    //         beRunning = 1;
    //       }
    //     }
		// 	}
			
		// 	else if (data == 0xa2) /* power key */
		// 	{
		// 		/* 已经创建任务 */
		// 		if (xSoundTaskHandle != NULL)
		// 		{
		// 			LCD_ClearLine(0, 0);
		// 			LCD_PrintString(0, 0, "Delete Task");
		// 			vTaskDelete(xSoundTaskHandle);
		// 			PassiveBuzzer_Control(0); /* 主动停止蜂鸣器 */
		// 			xSoundTaskHandle = NULL; // 任务状态句柄归零，等待创建
		// 		}
		// 	}
		// }
    // }


    for(;;)
  {
   //Led_Test();
  //LCD_Test();
	//MPU6050_Test(); 
	//DS18B20_Test();
	//DHT11_Test();
	//ActiveBuzzer_Test();
	//PassiveBuzzer_Test();
	//ColorLED_Test();
	IRReceiver_Test();  
	//IRSender_Test();
	//LightSensor_Test();
	//IRObstacle_Test();
	//SR04_Test();
	//W25Q64_Test();
	//RotaryEncoder_Test();
	//Motor_Test();
	//Key_Test();
	//UART_Test();
  }

  
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

