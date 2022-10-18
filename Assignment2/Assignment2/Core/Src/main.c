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

/* Declare Constants ------------------------------------------------------------------*/
#define GYRO_THRESHOLD 20.0
#define TEMP_THRESHOLD 38
#define MAG_THRESHOLD 0.4
#define HUM_THRESHOLD 30.0
#define PRES_THRESHOLD 110000.0
#define WARNING 1
#define SAFE 0

/* Function Declarations ------------------------------------------------------------------*/
extern void initialise_monitor_handles(void); // for semi-hosting support (printf)
static void UART1_Init(void);
static void mode_selection(void);
static void MX_GPIO_Init(void);
static void read_accelerometer(void);
static void read_temperature(void);
void SystemClock_Config(void);
static void exploration(void);
static void reset(void);

/* Global Variables ------------------------------------------------------------------*/
uint32_t T1, T2; // counting single and double press
UART_HandleTypeDef huart1; // huart1 variable of type UART_HANDLER
uint8_t flag = 0, press = 0, EXPLORATION = 1, EXPLORATION_WARNING_STATE = 0,
		BATTLE = 0, BATTLE_WARNING_STATE = 0;
char message_print[64]; // array buffer used for UART1 transmission
uint time_one_second = 0;

typedef struct exploration_t {
	float humidity_data;
	float pressure_data;
	int16_t magnetometer_raw_data[3];
	float magnetometer_data[3];
} exploration_t;

exploration_t exploration_t_1;

/**
 * @brief  External Interrupt to detect 1st button press and 2nd button press
 *
 * @note
 *
 * @retval	None
 */
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

	// initialise_monitor_handles();

	/* Reset of all peripherals */
	HAL_Init();
	MX_GPIO_Init(); // initialize PB14, pin connected to LED2
	UART1_Init(); // initialize UART1 for UART communication

	/* Peripheral initializations using BSP functions for all sensors*/
	BSP_ACCELERO_Init(); // initialize accelerometer
	BSP_TSENSOR_Init();	 // initialize temperature sensor
	BSP_HSENSOR_Init();	 // initialize humidity sensor
	BSP_GYRO_Init();	 // initialize gyroscope
	BSP_PSENSOR_Init();	 // initialize pressure sensor
	BSP_MAGNETO_Init();	 // initialize magnetometer

	/* sensor interrupt configuration*/
	// 		accelero_interrupt_config();
	/*enable NVIC EXTI interrupt*/
	// 		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	/* Exploration Mode: Print only once*/
	memset(message_print, 0, strlen(message_print));
	sprintf(message_print, "Entering EXPLORATION Mode \r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*) message_print, strlen(message_print),
			0xFFFF);

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

/**
 * @brief  Selects different modes such as Exploration and Battle and selects
 *			different states such as Normal and Warning in the respective modes.
 * @note
 *
 * @retval
 */
static void mode_selection() {
	/* Exploration Mode */
	if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 0 && BATTLE == 0
			&& press == 0) {
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
	if (EXPLORATION == 0 && BATTLE_WARNING_STATE == 0 && BATTLE == 1
			&& press == 0) {
		// Battle state
		// battle();
		memset(message_print, 0, strlen(message_print));
		sprintf(message_print, "Battle Mode \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
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

/**
 * @brief  Selects different modes such as Exploration and Battle and selects
 *			different states such as Normal and Warning in the respective modes.
 * @note
 *
 * @retval
 */
static void MX_GPIO_Init(void) //For LED and PB
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	//GPIO Ports Clock Enable
	__HAL_RCC_GPIOB_CLK_ENABLE();// For LED
	__HAL_RCC_GPIOC_CLK_ENABLE(); // For Push Button

	//Configure GPIO pin Output Level // Pin Initialization
	HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_RESET);

	//Configure GPIO pin LED2_Pin  // Pin Configuration
	GPIO_InitStruct.Pin = LED2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// Configuration of BUTTON_EXTI13_Pin (GPIO-C Pin-13) as AF
	GPIO_InitStruct.Pin = BUTTON_EXTI13_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Enable NVIC EXTI line 13
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

static void exploration(void) {
	if (HAL_GetTick() - time_one_second > 1000) {

		// Reset variables
		exploration_t_1.humidity_data = 0;
		exploration_t_1.pressure_data = 0;
		exploration_t_1.magnetometer_raw_data[3] = 0;
		exploration_t_1.magnetometer_data[3] = 0;

		exploration_t_1.humidity_data = BSP_HSENSOR_ReadHumidity();
		exploration_t_1.pressure_data = BSP_PSENSOR_ReadPressure();

		// Pass in the memory address to pDataXYZ Pointer to get XYZ magnetometer values.
		BSP_MAGNETO_GetXYZ(exploration_t_1.magnetometer_raw_data);

		exploration_t_1.magnetometer_data[0] =
				(float) exploration_t_1.magnetometer_raw_data[0] / 1000.0f;
		exploration_t_1.magnetometer_data[1] =
				(float) exploration_t_1.magnetometer_raw_data[1] / 1000.0f;
		exploration_t_1.magnetometer_data[2] =
				(float) exploration_t_1.magnetometer_raw_data[2] / 1000.0f;

		memset(message_print, 0, strlen(message_print));
		sprintf(message_print, "M:%f:%f:%f, P:%f, H:%f \r\n",
				exploration_t_1.magnetometer_data[0],
				exploration_t_1.magnetometer_data[1],
				exploration_t_1.magnetometer_data[2],
				exploration_t_1.pressure_data,
				exploration_t_1.humidity_data);
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);

		time_one_second = HAL_GetTick();
	}
}

static void read_accelerometer(void) {
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

/**
 * @brief  Initialize UART and Configure UART communication protocol
 *
 * @note
 *
 * @retval	None
 */
static void UART1_Init(void) {
	/* Pin configuration for UART. BSP_COM_Init() can do this automatically */
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* Configuring UART1 */
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		while (1)
			;
	}
}

/**
 * @brief  Set the flags of various sensors to the default state
 *
 * @note	For example, accflag = SAFE; gyroflag = SAFE; and so on ...
 *
 * @retval	None
 */
static void reset(void) {

}
