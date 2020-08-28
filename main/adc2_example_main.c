/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/dac.h"
#include "esp_system.h"

#define DAC_EXAMPLE_CHANNEL     CONFIG_EXAMPLE_DAC_CHANNEL
#define ADC1_EXAMPLE_CHANNEL    CONFIG_EXAMPLE_ADC1_CHANNEL

// DAC on Pin 25 -- IO25
// ADC on Pin 27 -- IO27


void app_main(void)
{
    uint8_t output_data=0;
    int current_value=0;
    esp_err_t r;
    float dac_output_vdc=0; //DAC output volts DC (0-3.3 VDC)
    float adc_input_vdc=0; // ADC input volts DC (0 - ESP32 supply voltage)
    int span_percent=0; // full span of 3.3VC = 100
    char span[4] = "";

    gpio_num_t adc_gpio_num, dac_gpio_num;

    r = adc1_pad_get_io_num( ADC1_EXAMPLE_CHANNEL, &adc_gpio_num );
    assert( r == ESP_OK );
    r = dac_pad_get_io_num( DAC_EXAMPLE_CHANNEL, &dac_gpio_num );
    assert( r == ESP_OK );

    printf("ADC1 channel %d @ GPIO %d, DAC channel %d @ GPIO %d.\n", ADC1_EXAMPLE_CHANNEL, adc_gpio_num,
                DAC_EXAMPLE_CHANNEL + 1, dac_gpio_num );

    dac_output_enable( DAC_EXAMPLE_CHANNEL );

    //be sure to do the init before using ADC1.
    printf("ADC1_init...\n");
    adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_config_channel_atten( ADC1_EXAMPLE_CHANNEL, ADC_ATTEN_0db );
    adc1_config_channel_atten( ADC1_EXAMPLE_CHANNEL, ADC_ATTEN_DB_11 );

    vTaskDelay(2 * portTICK_PERIOD_MS);

    printf("start conversion.\n");
    printf("  DAC Count    DAC VDC  ADC Count    ADC VDC   Span PCT\n");

    while(1) {

        dac_output_voltage( DAC_EXAMPLE_CHANNEL, output_data );
        current_value = adc1_get_raw( ADC1_EXAMPLE_CHANNEL);

        dac_output_vdc = output_data * 0.01289063;  // 3.3 divided by 256
        adc_input_vdc = current_value * 0.000952; // 3.9 divided by 4096
        span_percent = (int) (adc_input_vdc*30.3);
        sprintf(span,"%3d", span_percent);

        printf("%11d %10.2f %10d %10.2f %10d %10c %10c %10c %10c\n", output_data, dac_output_vdc,
        current_value, adc_input_vdc, span_percent, span[0], span[1], span[2], span[3]);

        output_data++;

        vTaskDelay( 2 * portTICK_PERIOD_MS );
    }
}
