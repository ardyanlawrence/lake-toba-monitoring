/*
 *  adc_app.c file
 *
 * 	author	: Dadan
 * 	email	: dadanugm07@gmail.com
 * 	date	: 8/2/2021
 * 	revison	:
 *
*/

#include "adc_app.h"

/*
 *  Info: Only using ADC unit 1, with 13 bit width and no attenuation,
 *  see datasheet for ADC UNIT 1 channels
 *  @ adc_ch = ADC unit 1 channels (not GPIO Num)
 */

static esp_adc_cal_characteristics_t *adc_chars;

void check_efuse(void)
{
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    }
	else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }

}

void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}


void adc_init(adc_channel_t adc_ch)
{
	check_efuse();
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(adc_ch, ADC_ATTEN_DB_11);

	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_0, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
	print_char_val_type(val_type);
}


float adc_read_val(adc_channel_t adc_ch)
{
	uint32_t adc_reading = 0;
	uint32_t voltage;
	float Volt;
	// 0.1304 -> result of voltage divider calculation with R1 = 1M, R2=150K

	for (int i = 0; i < NO_OF_SAMPLES; i++)
	{
		adc_reading += adc1_get_raw(adc_ch);
	}

	adc_reading /= NO_OF_SAMPLES;

	//Convert adc_reading to voltage in mV
	voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
	Volt = ((float)voltage/1000);

	//printf("Raw: %d Voltage: %dmV, %fV\r\n", adc_reading, voltage, Volt);
	return Volt;
}

uint32_t adc_read_raw(adc_channel_t adc_ch)
{
	uint32_t adc_reading = 0;
	uint32_t voltage;

	for (int i = 0; i < NO_OF_SAMPLES; i++)
	{
		adc_reading += adc1_get_raw(adc_ch);
	}

	adc_reading /= NO_OF_SAMPLES;
	return adc_reading;
}
//EOF
