/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 */
/* USER CODE END Header */

#include "main.h"
#include "fonts.h"
#include "ssd1306.h"
#include "stdio.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN 0 */
uint8_t DHT11_Read_Float(float *humidity, float *temperature)
{
    GPIO_TypeDef *PORT = GPIOB;
    uint16_t PIN = GPIO_PIN_0;
    TIM_HandleTypeDef *TIMER = &htim2;

    uint8_t data[5] = {0};
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);

    if (TIMER->State == HAL_TIM_STATE_RESET || TIMER->State == HAL_TIM_STATE_READY)
        HAL_TIM_Base_Start(TIMER);
    __HAL_TIM_SET_COUNTER(TIMER, 0);
    while (__HAL_TIM_GET_COUNTER(TIMER) < 30)
        ;

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(PORT, &GPIO_InitStruct);

    uint16_t timeout;

    timeout = 200;
    while (HAL_GPIO_ReadPin(PORT, PIN) != GPIO_PIN_RESET)
    {
        if (--timeout == 0)
            return 3;
        __HAL_TIM_SET_COUNTER(TIMER, 0);
        while (__HAL_TIM_GET_COUNTER(TIMER) < 1)
            ;
    }

    timeout = 200;
    while (HAL_GPIO_ReadPin(PORT, PIN) != GPIO_PIN_SET)
    {
        if (--timeout == 0)
            return 4;
        __HAL_TIM_SET_COUNTER(TIMER, 0);
        while (__HAL_TIM_GET_COUNTER(TIMER) < 1)
            ;
    }

    timeout = 200;
    while (HAL_GPIO_ReadPin(PORT, PIN) != GPIO_PIN_RESET)
    {
        if (--timeout == 0)
            return 5;
        __HAL_TIM_SET_COUNTER(TIMER, 0);
        while (__HAL_TIM_GET_COUNTER(TIMER) < 1)
            ;
    }

    for (int i = 0; i < 40; i++)
    {
        timeout = 200;
        while (HAL_GPIO_ReadPin(PORT, PIN) != GPIO_PIN_SET)
        {
            if (--timeout == 0)
                return 6;
            __HAL_TIM_SET_COUNTER(TIMER, 0);
            while (__HAL_TIM_GET_COUNTER(TIMER) < 1)
                ;
        }

        __HAL_TIM_SET_COUNTER(TIMER, 0);
        while (__HAL_TIM_GET_COUNTER(TIMER) < 40)
            ;

        data[i / 8] <<= 1;
        if (HAL_GPIO_ReadPin(PORT, PIN) == GPIO_PIN_SET)
            data[i / 8] |= 1;

        timeout = 200;
        while (HAL_GPIO_ReadPin(PORT, PIN) != GPIO_PIN_RESET)
        {
            if (--timeout == 0)
                return 7;
            __HAL_TIM_SET_COUNTER(TIMER, 0);
            while (__HAL_TIM_GET_COUNTER(TIMER) < 1)
                ;
        }
    }

    if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
        return 2;

    *humidity = (float)data[0] + ((float)data[1] / 10.0f);
    *temperature = (float)data[2] + ((float)data[3] / 10.0f);

    return 0;
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM2_Init();

    /* USER CODE BEGIN 2 */
    SSD1306_Init();
    int hasError;
    float temperature;
    float humidity;
    char temp[50], hum[50];

    // PC13 LED ?????? OFF (Blue Pill-? LOW = ON, HIGH = OFF)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    /* USER CODE END 2 */

    while (1)
    {
        hasError = DHT11_Read_Float(&humidity, &temperature);

        if (!hasError)
        {
            // OLED display
            SSD1306_Fill(0);
            SSD1306_GotoXY(0, 0);
            sprintf(temp, "Temp: %.2f", temperature);
            SSD1306_Puts(temp, &Font_7x10, 1);
            SSD1306_GotoXY(0, 20);
            sprintf(hum, "Humidity: %.2f", humidity);
            SSD1306_Puts(hum, &Font_7x10, 1);
            SSD1306_UpdateScreen();

            if (temperature <= 28.0f)
            {
                for (int i = 0; i < 3; i++)
                {
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // ON
                    HAL_Delay(200);
                    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // OFF
                    HAL_Delay(200);
                }
            }
            else
            {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); // ON
            }
        }

        HAL_Delay(1000);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        Error_Handler();

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        Error_Handler();
}

static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
        Error_Handler();
}

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 65535;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
        Error_Handler();

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
        Error_Handler();

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
        Error_Handler();
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE(); // PC13 ?? ???? GPIOC clock enable

    // PB0 ? DHT11
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // PC13 ? Built-in LED
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); // ?????? OFF
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif