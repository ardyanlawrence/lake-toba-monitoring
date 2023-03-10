/*
 *  board.h file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 2/8/2021
 * 	revison	:
 *
*/


#ifndef __BOARD_H
#define __BOARD_H

#define BACKUPS_FOLDER                  "b"
#define SETTINGS_FOLDER                 "settings"
#define SENSOR_SETTINGS_FOLDER          "ssetting"
#define TOPICS_FOLDER                   "topics"
#define PROVISIONING_FOLDER             "provis"
#define ANALOG1_BACKUP_FOLDER           "b/an1"
#define ANALOG2_BACKUP_FOLDER           "b/an2"
#define ANALOG3_BACKUP_FOLDER           "b/an3"
#define ANALOG1_INSTANCE_FOLDER         "b/anist1"
#define ANALOG2_INSTANCE_FOLDER         "b/anist2"
#define ANALOG3_INSTANCE_FOLDER         "b/anist3"

#define DIGITAL1_BACKUP_FOLDER          "b/di1"
#define DIGITAL2_BACKUP_FOLDER          "b/di2"
#define DIGITAL3_BACKUP_FOLDER          "b/di3"
#define COUNTER1_BACKUP_FOLDER          "b/cnt1"
#define COUNTER2_BACKUP_FOLDER          "b/cnt2"
#define COUNTER3_BACKUP_FOLDER          "b/cnt3"
#define FREQ1_BACKUP_FOLDER             "b/fr1"
#define FREQ2_BACKUP_FOLDER             "b/fr2"
#define FREQ3_BACKUP_FOLDER             "b/fr3"
#define MODBUS_BACKUP_FOLDER            "b/modbus"
#define UART_BACKUP_FOLDER              "b/uart"
#define DEFAULT_SENSOR_STACK_SIZE       4095
#define MAIN_INIT_STACK_SIZE            8191
#define MODBUS_WAIT_RESPONSE            500
#define UART_RX_TASK_PRIORITY           2
#define GPIO_HANDLER_PRIORITY           2

#define UART1_BAUD                      9600
#define UART0_BAUD                      115200

#define MQTT_PREFIX_LEN                 61
#define MQTT_UID_PREFIX_LEN             30

// #define ESP32U
#define ESP32
// #define ESP32S3

/////////// ESP32U Board pins////////////// 
#ifdef ESP32U
// UART
#define UART0_TX 		1
#define UART0_RX		3
#define UART1_TX		10
#define UART1_RX		9
#define UART2_TX		17
#define UART2_RX		16
// SPI
#define SPI_CS			15
#define SPI_MOSI		13
#define SPI_MISO	    33
#define SPI_CLK			14
//I2C
#define I2C_SDA			34
#define I2C_SCL			35
// GPIO CHANNELS
#define DIGITAL_CH1		19
#define DIGITAL_CH2		18
#define DIGITAL_CH3		5
// ADC CHANNELS
#define ANALOG_CH1              ADC1_CHANNEL_7
#define ANALOG_CH2              ADC1_CHANNEL_6
#define ANALOG_CH3              ADC1_CHANNEL_0
#define ANALOG_SAMPLING_DELAY   100 // ms   
// EEPROM
#define EEPROM_ADDR 			0x50
// BUTTON and LED
#define LED_INDICATOR   2
#define BUTTON_MODE     4

#endif // ESP32U

/////////// ESP32 Board pins////////////// 
#ifdef ESP32
// UART
#define UART0_TX 		1
#define UART0_RX		3
#define UART1_TX		10
#define UART1_RX		9
#define UART2_TX		17
#define UART2_RX		16
// SPI
#define SPI_CS			15
#define SPI_MOSI		13
#define SPI_MISO	    33
#define SPI_CLK			14
//I2C
#define I2C_SDA			34
#define I2C_SCL			35
// GPIO CHANNELS
#define DIGITAL_CH1		19
#define DIGITAL_CH2		18
#define DIGITAL_CH3		5
// ADC CHANNELS
#define ANALOG_CH1              ADC1_CHANNEL_7
#define ANALOG_CH2              ADC1_CHANNEL_6
#define ANALOG_CH3              ADC1_CHANNEL_0
#define ANALOG_SAMPLING_DELAY   100 // ms   
// EEPROM
#define EEPROM_ADDR 			0x50
// BUTTON and LED
#define LED_INDICATOR   2
#define BUTTON_MODE     4

#endif // ESP32

/////////// ESP32 S3 Board pins////////////// 
#ifdef ESP32S3
// UART
#define UART0_TX 		43
#define UART0_RX		44
#define MODBUS_TX		17
#define MODBUS_RX		18
// SPI
#define SPI_CS			5
#define SPI_MOSI		15
#define SPI_MISO		2
#define SPI_CLK			14
//I2C
#define I2C_SDA			34
#define I2C_SCL			35
// GPIO CHANNELS
#define DIGITAL_CH1		4
#define DIGITAL_CH2		12
#define DIGITAL_CH3		13
// ADC CHANNELS
#define ANALOG_CH1              ADC_CHANNEL_0
#define ANALOG_CH2              ADC_CHANNEL_3
#define ANALOG_CH3              ADC_CHANNEL_5
#define ANALOG_SAMPLING_DELAY   100 // ms
// EEPROM
#define EEPROM_ADDR 			0x50

#endif // ESP32 S3

#endif
