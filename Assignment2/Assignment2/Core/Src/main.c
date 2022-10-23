/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.h"
#include "../../Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.h"
#include "stdio.h"
#include "stdlib.h"  // use stdlib.h header file to use abs() function.
#include "stdbool.h"
#include "string.h"
#include "math.h"
#include "ctype.h"

/* Declare Constants ------------------------------------------------------------------*/
#define GYRO_THRESHOLD 20.0 // based on testing
#define TEMP_THRESHOLD_MIN -20.0 // -20 degrees Celcius
#define TEMP_THRESHOLD_MAX 70.0 // 70 degrees Celcius

#define MAG_THRESHOLD 2.0 // Max is 4

#define HUM_THRESHOLD 30.0
#define PRES_THRESHOLD_MIN 90000.0
#define PRES_THRESHOLD_MAX 105000.0

#define WARNING 1
#define SAFE 0

#define MESSAGE_SIZE 300

/* Function Declarations ------------------------------------------------------------------*/
static void UART1_Init(void);
static void mode_selection(void);
static void MX_GPIO_Init(void);
static void exploration_warning(void);
static void battle_warning(void);
void SystemClock_Config(void);
static void exploration(void);
static void battle(void);
static void charge_fluxer_battery(void);
void reset_sensor_warning_flags(void);

/* Global Variables ------------------------------------------------------------------*/
uint32_t T1, T2; // counting single and double press
UART_HandleTypeDef huart1; // huart1 variable of type UART_HANDLER
uint8_t flag = 0, press = 0, EXPLORATION = 1, EXPLORATION_WARNING_STATE = 0,
		BATTLE = 0, BATTLE_WARNING_STATE = 0, count_warnings = 0,
		fluxer_battery = 10;

char message_print[MESSAGE_SIZE]; // array buffer used for UART1 transmission

uint32_t time_EXPLORATION_SENSOR = 0;
uint32_t time_EXPLORATION_WARNING_LED = 0;
uint32_t time_EXPLORATION_WARNING_MESSAGE = 0;

uint32_t time_BATTLE_SENSOR = 0;
uint32_t time_BATTLE_WARNING_LED = 0;
uint32_t time_BATTLE_WARNING_MESSAGE = 0;
uint32_t time_BATTLE_LED = 0;
uint32_t time_fluxer = 0;

volatile uint8_t GYROSCOPE_Flag = SAFE, MAGNETOMETER_Flag = SAFE,
		PRESSURE_Flag = SAFE, HUMIDITY_Flag = SAFE;

typedef struct sensor_data_t {
	float temperature_data;
	float humidity_data;
	float pressure_data;
	float altitude;

	int16_t magnetometer_raw_data[3];
	float magnetometer_data[3];

	int16_t accelerometer_raw_data[3];
	float accelerometer_data[3];

	float gyroscope_raw_data[3];
	float gyroscope_data[3];

} sensor_data_t;
// Initialize 2 structs for Exploration and Battle modes
sensor_data_t sensor_data_t_exploration;
sensor_data_t sensor_data_t_battle;

/**
 * @brief  External Interrupt to detect 1st button press and 2nd button press
 * @note
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
	// accelero_interrupt_config();
	/*enable NVIC EXTI interrupt*/
	// HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	/* Exploration Mode: Print only once*/
	//memset(message_print, 0, strlen(message_print));
	snprintf(message_print, MESSAGE_SIZE, "Entering EXPLORATION Mode \r\n");
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
 * @brief  	Selects different modes such as Exploration and Battle and selects
 *			different states such as Normal and Warning in the respective modes.
 * @note
 * @retval	None
 */
static void mode_selection() {
	/* Exploration Mode */
	if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 0 && BATTLE == 0
			&& press <= 1) {
		// Normal state
		exploration();
		press = 0;
	} else if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 1) {
		// Come to the Warning State through interrupts or polling
		exploration_warning();

		if (press == 1) {
			// Clear the warning and go back to Exploration mode
			EXPLORATION_WARNING_STATE = 0;
			EXPLORATION = 1;
			press = 0;
		} else if (press == 2) {
			// Ignore it
			press = 0;
		}
	} else if (EXPLORATION == 1 && EXPLORATION_WARNING_STATE == 0
			&& press == 2) {
		// Change to Battle Mode
		/* A message "Entering BATTLE mode" is sent once to Cyrix's Lab once
		 * immediately upon entering the BATTLE mode.
		 * The press flag is cleared later in mode_selection()
		 */
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Entering BATTLE Mode \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);

		EXPLORATION = 0;
		BATTLE = 1;
		press = 0;
	}

	/* Battle Mode */
	if (EXPLORATION == 0 && BATTLE_WARNING_STATE == 0 && BATTLE == 1
			&& press == 0) {
		// Battle state
		battle();
		press = 0;
	} else if (EXPLORATION == 0 && BATTLE_WARNING_STATE == 0 && BATTLE == 1
			&& press == 1) {
		/*	In BATTLE_MODE, without WARNING:
		 * 	i.e., when Pixie is not sending 'SOS' message to Cyrix,
		 * 	single press triggers BATTERY_CHARGING,
		 * 	i.e., after single press, Fluxer is charged with 1/10 energy
		 * 	of its capacity.*/
		charge_fluxer_battery();
		BATTLE = 1;
		press = 0; // reset the press flag
	} else if (BATTLE == 1 && BATTLE_WARNING_STATE == 1) {
		battle_warning();

		if (press == 1) {
			// Clear the warning and go back to Battle mode
			BATTLE_WARNING_STATE = 0;
			BATTLE = 1;
			press = 0;
		} else if (press == 2) {
			// Ignore it
			press = 0;
		}
	} else if (BATTLE == 1 && BATTLE_WARNING_STATE == 0 && press == 2) {
		// Change to EXPLORATION Mode
		BATTLE = 0;
		EXPLORATION = 1;
		press = 0;
	}
}

/**
 * @brief	In Exploration Mode,
 * 			Gyroscope, Magnetometer, Pressure, Humidity sensor readings
 * 			are send to Cyrix lab periodically.
 * @note	The sensors are grouped in a struct for easier data manipulation.
 * @retval	None
 */

static void exploration(void) {
	// In EXPLORATION MODE, only those sensors mounted on Pixie are read periodically every ONE second
	if (HAL_GetTick() - time_EXPLORATION_SENSOR > 1000) {

		// Reset variables
		sensor_data_t_exploration.humidity_data = 0;
		sensor_data_t_exploration.pressure_data = 0;
		sensor_data_t_exploration.magnetometer_raw_data[3] = 0;
		sensor_data_t_exploration.magnetometer_data[3] = 0;
		sensor_data_t_exploration.gyroscope_raw_data[3] = 0;
		sensor_data_t_exploration.gyroscope_data[3] = 0;

		// Read Humidity readings
		sensor_data_t_exploration.humidity_data = BSP_HSENSOR_ReadHumidity();
		// Read the pressure in units (Pascal)
		// One hectopascal(hPa) is equal to exactly 100 Pascals.
		sensor_data_t_exploration.pressure_data = BSP_PSENSOR_ReadPressure()
				* 100.0f;

		// Pass in the memory address to pDataXYZ Pointer to get XYZ magnetometer values.
		BSP_MAGNETO_GetXYZ(sensor_data_t_exploration.magnetometer_raw_data);

		sensor_data_t_exploration.magnetometer_data[0] =
				(float) sensor_data_t_exploration.magnetometer_raw_data[0]
						/ 1000.0f;
		sensor_data_t_exploration.magnetometer_data[1] =
				(float) sensor_data_t_exploration.magnetometer_raw_data[1]
						/ 1000.0f;
		sensor_data_t_exploration.magnetometer_data[2] =
				(float) sensor_data_t_exploration.magnetometer_raw_data[2]
						/ 1000.0f;

		// Pass in the memory address to pDataXYZ Pointer to get XYZ gyroscope values.
		BSP_GYRO_GetXYZ(sensor_data_t_exploration.gyroscope_raw_data);
		sensor_data_t_exploration.gyroscope_data[0] =
				sensor_data_t_exploration.gyroscope_raw_data[0] / 1000.0f;
		sensor_data_t_exploration.gyroscope_data[1] =
				sensor_data_t_exploration.gyroscope_raw_data[1] / 1000.0f;
		sensor_data_t_exploration.gyroscope_data[2] =
				sensor_data_t_exploration.gyroscope_raw_data[2] / 1000.0f;

		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE,
				"G:%0.2f:%0.2f (dps), M:%0.3f:%0.3f:%0.3f (Gauss), P:%0.2f (Pa), H:%0.2f (%%RH) \r\n",
				sensor_data_t_exploration.gyroscope_data[0],
				sensor_data_t_exploration.gyroscope_data[1],
				// sensor_data_t_exploration.gyroscope_data[2],
				sensor_data_t_exploration.magnetometer_data[0],
				sensor_data_t_exploration.magnetometer_data[1],
				sensor_data_t_exploration.magnetometer_data[2],
				sensor_data_t_exploration.pressure_data,
				sensor_data_t_exploration.humidity_data);
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);

		time_EXPLORATION_SENSOR = HAL_GetTick();
	}

	// EXPLORATION LED will always be ON
	HAL_GPIO_WritePin(GPIOB, LED2_Pin, GPIO_PIN_SET);

	/**
	 * @brief	Check if the sensors have reached their threshold, then if ANY 2
	 * 			of the sensors have exceeded their maximum/minimum threshold,
	 * 			go to the WARNING state.
	 * @steps	1. Raise flags if threshold is reached.
	 * 			2. Type-cast variables explicitly to (int) to use abs()
	 * 			3. Reset flags to SAFE before leaving exploration mode
	 * 			and reaching to warning state.
	 * 			4. Reset count_warnings counter to 0.
	 * 			5. Set the EXPLORATION_WARNING_STATE flag to 1.
	 */
	if ((abs((int) sensor_data_t_exploration.magnetometer_data[0])
			>= MAG_THRESHOLD
			|| abs((int) sensor_data_t_exploration.magnetometer_data[1])
					>= MAG_THRESHOLD
			|| abs((int) sensor_data_t_exploration.magnetometer_data[2])
					>= MAG_THRESHOLD) && MAGNETOMETER_Flag != WARNING) {

		// Set MAGNETOMETER_Flag to WARNING
		MAGNETOMETER_Flag = WARNING;
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Magnetometer Flag enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		count_warnings += 1;
	}

	if ((abs((int) sensor_data_t_exploration.gyroscope_data[0])
			>= GYRO_THRESHOLD
			|| abs((int) sensor_data_t_exploration.gyroscope_data[1])
					>= GYRO_THRESHOLD) && GYROSCOPE_Flag != WARNING) {

		// Set GYROSCOPE_Flag to WARNING
		GYROSCOPE_Flag = WARNING;
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Gyroscope Flag enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		count_warnings += 1;
	}

	if ((sensor_data_t_exploration.pressure_data <= PRES_THRESHOLD_MIN)
			|| (sensor_data_t_exploration.pressure_data >= PRES_THRESHOLD_MAX)) {

		// Set PRESSURE_Flag to WARNING
		PRESSURE_Flag = WARNING;
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Pressure Flag enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		count_warnings += 1;
	}

	if (sensor_data_t_exploration.humidity_data <= HUM_THRESHOLD) {

		// Set HUMIDITY_Flag to WARNING
		HUMIDITY_Flag = WARNING;
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Humidity Flag enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		count_warnings += 1;
	}

	if (count_warnings == 2) {
		reset_sensor_warning_flags();
		count_warnings = 0;
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE,
				"EXPLORATION_WARNING_STATE enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		// Set the EXPLORATION_WARNING_STATE flag to 1
		EXPLORATION_WARNING_STATE = 1;
	}

	// Used for testing EXPLORATION_WARNING_STATE
	int stateOfPushButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
	if (stateOfPushButton == 1) {
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE,
				"EXPLORATION_WARNING_STATE enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		// Set the EXPLORATION_WARNING_STATE flag to 1
		EXPLORATION_WARNING_STATE = 1;
	}
}

static void exploration_warning(void) {
	// Toggle WARNING LED every 3 seconds.
	if ((HAL_GetTick() - time_EXPLORATION_WARNING_LED) > 3000) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		time_EXPLORATION_WARNING_LED = HAL_GetTick(); // reset time_EXPLORATION_WARNING_LED
	}

	// Send WARNING mode: SOS once every 1 second.
	if ((HAL_GetTick() - time_EXPLORATION_WARNING_MESSAGE) > 1000) {
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "WARNING mode: SOS \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		time_EXPLORATION_WARNING_MESSAGE = HAL_GetTick(); // reset time_EXPLORATION_WARNING_LED
	}
}

static void battle(void) {

	// In BATTLE MODE, only those sensors mounted on Pixie are read periodically every ONE second.
	if (HAL_GetTick() - time_BATTLE_SENSOR > 1000) {

		// Reset variables
		sensor_data_t_battle.temperature_data = 0;
		sensor_data_t_battle.humidity_data = 0;
		sensor_data_t_battle.pressure_data = 0;
		sensor_data_t_battle.magnetometer_raw_data[3] = 0;
		sensor_data_t_battle.magnetometer_data[3] = 0;
		sensor_data_t_battle.gyroscope_raw_data[3] = 0;
		sensor_data_t_battle.gyroscope_data[3] = 0;
		sensor_data_t_battle.accelerometer_raw_data[3] = 0;
		sensor_data_t_battle.accelerometer_data[3] = 0;

		// Read Humidity readings
		sensor_data_t_battle.humidity_data = BSP_HSENSOR_ReadHumidity();

		/*	Read the pressure in units (Pascal)
		 * 	One hectopascal(hPa) is equal to exactly 100 Pascals. */
		sensor_data_t_battle.pressure_data = BSP_PSENSOR_ReadPressure()
				* 100.0f;

		// Read Temperature Readings
		sensor_data_t_battle.temperature_data = BSP_TSENSOR_ReadTemp();

		// Pass in the memory address to pDataXYZ Pointer to get XYZ magnetometer values.
		BSP_MAGNETO_GetXYZ(sensor_data_t_battle.magnetometer_raw_data);
		sensor_data_t_battle.magnetometer_data[0] =
				(float) sensor_data_t_battle.magnetometer_raw_data[0] / 1000.0f;
		sensor_data_t_battle.magnetometer_data[1] =
				(float) sensor_data_t_battle.magnetometer_raw_data[1] / 1000.0f;
		sensor_data_t_battle.magnetometer_data[2] =
				(float) sensor_data_t_battle.magnetometer_raw_data[2] / 1000.0f;

		// Pass in the memory address to pDataXYZ Pointer to get XYZ gyroscope values.
		BSP_GYRO_GetXYZ(sensor_data_t_battle.gyroscope_raw_data);
		sensor_data_t_battle.gyroscope_data[0] =
				sensor_data_t_battle.gyroscope_raw_data[0] / 1000.0f;
		sensor_data_t_battle.gyroscope_data[1] =
				sensor_data_t_battle.gyroscope_raw_data[1] / 1000.0f;
		sensor_data_t_battle.gyroscope_data[2] =
				sensor_data_t_battle.gyroscope_raw_data[2] / 1000.0f;

		/* Pass in the memory address to pDataXYZ Pointer to get XYZ accelerometer values.
		 * The function below returns 16 bit integers which are 100 * acceleration(m/s^2).
		 * Convert to float to print the actual acceleration*/
		BSP_ACCELERO_AccGetXYZ(sensor_data_t_battle.accelerometer_raw_data);
		sensor_data_t_battle.accelerometer_data[0] =
				sensor_data_t_battle.accelerometer_raw_data[0] / 100.0f;
		sensor_data_t_battle.accelerometer_data[1] =
				sensor_data_t_battle.accelerometer_raw_data[1] / 100.0f;
		sensor_data_t_battle.accelerometer_data[2] =
				sensor_data_t_battle.accelerometer_raw_data[2] / 100.0f;

		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE,
				"T:%0.2f (deg C), P:%0.2f (Pa), H:%0.2f (%%RH), A:%0.2f (g), G:%0.2f:%0.2f (dps), M:%0.3f:%0.3f:%0.3f (Gauss) \r\n",
				sensor_data_t_battle.temperature_data,
				sensor_data_t_battle.pressure_data,
				sensor_data_t_battle.humidity_data,
				// sensor_data_t_battle.accelerometer_data[0],
				// sensor_data_t_battle.accelerometer_data[1],
				sensor_data_t_battle.accelerometer_data[2],
				sensor_data_t_battle.gyroscope_data[0],
				sensor_data_t_battle.gyroscope_data[1],
				// sensor_data_t_battle.gyroscope_data[2],
				sensor_data_t_battle.magnetometer_data[0],
				sensor_data_t_battle.magnetometer_data[1],
				sensor_data_t_battle.magnetometer_data[2]);
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);

		time_BATTLE_SENSOR = HAL_GetTick();
	}

	// Toggle WARNING LED every 1 second.
	if ((HAL_GetTick() - time_BATTLE_LED) > 1000) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		time_BATTLE_LED = HAL_GetTick(); // reset time_BATTLE_LED
	}

	// Self firing Fluxer every 5s.
	if (HAL_GetTick() - time_fluxer > 5000 && fluxer_battery >= 2) {

		fluxer_battery -= 2;

		time_fluxer = HAL_GetTick(); // reset time_fluxer

		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "Battery: %d/10 \r\n",
				fluxer_battery);
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
	}

	/**
	 * @brief	Check if the sensors have reached their threshold, then if ANY 2
	 * 			of the sensors have exceeded their maximum/minimum threshold,
	 * 			go to the WARNING state.
	 * @steps	1. Raise flags if threshold is reached.
	 * 			2. Type-cast variables explicitly to (int) to use abs()
	 * 			3. Reset flags to SAFE before leaving exploration mode
	 * 			and reaching to warning state.
	 * 			4. Reset count_warnings counter to 0.
	 * 			5. Set the EXPLORATION_WARNING_STATE flag to 1.
	 */
	if ((sensor_data_t_battle.temperature_data >= 70
			|| sensor_data_t_battle.temperature_data < -20)
			|| sensor_data_t_battle.humidity_data <= HUM_THRESHOLD
			|| (sensor_data_t_battle.pressure_data >= PRES_THRESHOLD_MAX
					|| sensor_data_t_battle.pressure_data <= PRES_THRESHOLD_MIN)
			|| sensor_data_t_battle.accelerometer_data[0] >= 10
			// || sensor_data_t_battle.accelerometer_data[1] >= 1000
			// || sensor_data_t_battle.accelerometer_data[2] >= 1000
			|| abs(
					(int) sensor_data_t_battle.gyroscope_data[0]
							>= GYRO_THRESHOLD)
			|| abs(
					(int) sensor_data_t_battle.gyroscope_data[1]
							>= GYRO_THRESHOLD)
			// || sensor_data_t_battle.gyroscope_data[2] >= 2000000
			|| abs(
					(int) sensor_data_t_battle.magnetometer_data[0]
							>= MAG_THRESHOLD)
			|| abs(
					(int) sensor_data_t_battle.magnetometer_data[1]
							>= MAG_THRESHOLD)
			|| abs(
					(int) sensor_data_t_battle.magnetometer_data[2]
							>= MAG_THRESHOLD)) {
		BATTLE_WARNING_STATE = 1;
	}

	// Used for testing BATTLE_WARNING_STATE
	int stateOfPushButton = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
	if (stateOfPushButton == 1) {
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE,
				"BATTLE_WARNING_STATE enabled \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		BATTLE_WARNING_STATE = 1;
	}
}

static void charge_fluxer_battery(void) {
	// Fluxer recharging using PB.
	if (fluxer_battery <= 10 && BATTLE_WARNING_STATE == 0) {
		fluxer_battery += 1;
		press = 0;
	} else {
		press = 0;
	}
}

static void battle_warning(void) {
	// Toggle WARNING LED every 3 seconds.
	if ((HAL_GetTick() - time_BATTLE_WARNING_LED) > 3000) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		time_BATTLE_WARNING_LED = HAL_GetTick(); // reset time_EXPLORATION_WARNING_LED
	}

	// Warning Message sent every once a second
	if ((HAL_GetTick() - time_BATTLE_WARNING_MESSAGE) > 1000) {
		// send BATTLE mode: SOS
		//memset(message_print, 0, strlen(message_print));
		snprintf(message_print, MESSAGE_SIZE, "BATTLE mode: SOS \r\n");
		HAL_UART_Transmit(&huart1, (uint8_t*) message_print,
				strlen(message_print), 0xFFFF);
		time_BATTLE_WARNING_MESSAGE = HAL_GetTick(); // reset time_EXPLORATION_WARNING_LED
	}
}

/**
 * @brief  	Set the flags of various sensors to the default state
 * @note	For example, accflag = SAFE; gyroflag = SAFE; and so on ...
 * @retval	None
 */
void reset_sensor_warning_flags(void) {
	GYROSCOPE_Flag = SAFE;
	MAGNETOMETER_Flag = SAFE;
	PRESSURE_Flag = SAFE;
	HUMIDITY_Flag = SAFE;
}

static void MX_GPIO_Init(void) //For LED and PB
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	//GPIO Ports Clock Enable
	__HAL_RCC_GPIOA_CLK_ENABLE();// D8 Arduino Pinout
	__HAL_RCC_GPIOB_CLK_ENABLE(); // For LED
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

	// Testing single press and double press
	// Configuration of D7 as input
	GPIO_InitStruct.Pin = ARD_D7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL; // Pull-down activation
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // PA4

	// Configuration of D8 as input
	GPIO_InitStruct.Pin = ARD_D8_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL; // Pull-down activation
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // PB2

	// Enable NVIC EXTI line 13
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
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
