/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

#define GYRO_THRESHOLD 20.0
#define TEMP_THRESHOLD 38
#define MAG_THRESHOLD 0.4
#define HUM_THRESHOLD 30.0
#define PRES_THRESHOLD 110000.0
#define WARNING 1
#define SAFE 0

extern void initialise_monitor_handles(void); // for semi-hosting support (printf)
static void mode_selection(void);
static void MX_GPIO_Init(void);
static void showAcc(void);
static void showTemp(void);
void SystemClock_Config(void);
static void exploration(void);

/*Global Variables*/

// counting single and double press
uint32_t T1, T2;
uint8_t flag = 0, press = 0, EXPLORATION = 1, EXPLORATION_WARNING_STATE = 0,
		BATTLE = 0, BATTLE_WARNING_STATE = 0;

HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if ((GPIO_Pin == BUTTON_EXTI13_Pin) && (flag == 0)) {
		flag++;
		T1 = uwTick;
	} else if ((GPIO_Pin == BUTTON_EXTI13_Pin) && (flag > 0)) {
		flag++;
		T2 = uwTick;
	}
}

int main(void) {

	initialise_monitor_handles();
	HAL_Init();
	MX_GPIO_Init();
	BSP_ACCELERO_Init();
	BSP_TSENSOR_Init();
	BSP_HSENSOR_Init(); //Humidity Sensor Test

	while (1) {
		if ((flag == 1) && (uwTick - T1 > 1000)) {
			// Detect a single press
			flag = 0;
			press = 1;
		}

		if ((flag == 2) && (T2 - T1 < 1000)) {
			// Detect double press
			press = 2;
			flag = 0;
		}
		mode_selection();
	}
}

static void mode_selection() {
	/* Exploration Mode */
	if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 0 && BATTLE == 0 && press == 0) {
		// Normal state
		exploration();
	} else if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 1) {
		// Come to the Warning State through interrupts or polling
		// exploration_warning();
		if (press == 1) {
			// Clear the warning and go back to Exploration mode
			EXPLORATION_WARNING_STATE = 0;
			EXPLORATION = 1;
			press = 0;
		} else if (press == 2) {
			// Ignore it
		}
	} else if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 0
			&& press == 2) {
		// Change to Battle Mode
		EXPLORATION = 0;
		BATTLE = 1;
		press = 0;
	}

	/* Battle Mode */
	if (EXPLORATION == 0 && BATTLE_WARNING_STATE == 0 && BATTLE == 1 && press == 0) {
		// Battle state
		// battle();
		printf("Battle mode \n");
	} else if (BATTLE == 1 && BATTLE_WARNING_STATE == 1) {
		if (press == 1) {
			// Clear the warning and go back to Exploration mode
			BATTLE_WARNING_STATE = 0;
			BATTLE = 1;
			press = 0;
		} else if (press == 2) {
			// Ignore it
		}
	} else if (BATTLE == 1 && BATTLE_WARNING_STATE == 0 && press == 2) {
		// Change to EXPLORATION Mode
		BATTLE = 0;
		EXPLORATION = 1;
		press = 0;
	}
}

static void MX_GPIO_Init(void) //For LED and PB
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	//GPIO Ports Clock Enable
	__HAL_RCC_GPIOB_CLK_ENABLE();// For LED
	__HAL_RCC_GPIOC_CLK_ENABLE(); // For PB

	//Configure GPIO pin Output Level // Pin Initialization
	HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);

	//Configure GPIO pin LED2_Pin  // Pin Configuration
	GPIO_InitStruct.Pin = LED2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //Have to implement as part of GPIO initialization
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Configuration of BUTTON_EXTI13_Pin (G{IO-C Pin-13)as AF
	GPIO_InitStruct.Pin = BUTTON_EXTI13_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Enable NVIC EXTI line 13
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void exploration(void) {
	uint32_t Acc, Acc1, Temp, Temp1, LED, LED1;
	Acc = uwTick; //Initialize tick for Acc
	Temp = uwTick; //Initialize tick for Temp
	LED = uwTick; //Initialize tick for LED

	do {
		Acc1 = uwTick;
		Temp1 = uwTick;
		LED1 = uwTick;

		if ((Acc1 - Acc) > 1000) {
			showAcc();
			Acc = uwTick;
		}

		if ((Temp1 - Temp) > 1500) {
			showTemp();
			Temp = uwTick;
		}

		if ((LED1 - LED) > 1000) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); //Pin Read Write (Toggle 0 to 1)
			LED = uwTick;
		}
	} while (flag == 0);
}

static void showAcc(void) {
	float accel_data[3];
	int16_t accel_data_i16[3] = { 0 }; // array to store the x, y and z readings.
	BSP_ACCELERO_AccGetXYZ(accel_data_i16);  // read accelerometer
	// the function above returns 16 bit integers which are 100 * acceleration_in_m/s2. Converting to float to print the actual acceleration.
	accel_data[0] = (float) accel_data_i16[0] / 100.0f;
	accel_data[1] = (float) accel_data_i16[1] / 100.0f;
	accel_data[2] = (float) accel_data_i16[2] / 100.0f;
	printf("\nAccel:\nX: %f; Y: %f; Z: %f", accel_data[0], accel_data[1],
			accel_data[2]);
}

static void showTemp(void) {
	float temp_data;
	temp_data = BSP_TSENSOR_ReadTemp();   // read temperature sensor
	printf("\nTemperature : %f\n", temp_data);
}
