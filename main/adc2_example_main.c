/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "esp_err.h"
#include "esp_log.h"
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

#define tag "adc_main"

void init_ADC(){
  ESP_LOGI(tag, "Initialize Analag to Digital Converter 1");

  esp_err_t r;
  gpio_num_t adc_gpio_num;

  r = adc1_pad_get_io_num( ADC1_EXAMPLE_CHANNEL, &adc_gpio_num );
  assert( r == ESP_OK );
  ESP_LOGI(tag, "ADC1 channel %d @ GPIO %d \n", ADC1_EXAMPLE_CHANNEL, adc_gpio_num);


  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten( ADC1_EXAMPLE_CHANNEL, ADC_ATTEN_DB_11 ); //0 to 3.9VDC
}

void init_DAC(){

  ESP_LOGI(tag, "Initialize Digital to Analog Converter");

  esp_err_t r;
  gpio_num_t dac_gpio_num;

  r = dac_pad_get_io_num( DAC_EXAMPLE_CHANNEL, &dac_gpio_num );
  assert( r == ESP_OK );

  ESP_LOGI(tag, "DAC channel %d @ GPIO %d.\n", DAC_EXAMPLE_CHANNEL + 1, dac_gpio_num );
  dac_output_enable( DAC_EXAMPLE_CHANNEL );
}

float dac_set_voltage(int dac_set_point){
    float dac_output_vdc; //DAC output volts DC (0-3.3 VDC)
    dac_output_vdc = dac_set_point * 0.01289063;
    return dac_output_vdc;
}

float adc_measured_voltage1(int adc_measured_counts){
    float adc_input_vdc;
    adc_input_vdc = adc_measured_counts * 0.000952; // 3.9 divided by 4096
    return adc_input_vdc;
}

int span_pct_integer(int adc_measured_counts){
  int span_percent;
  span_percent = (int) (adc_measured_voltage1(adc_measured_counts)*30.3);
  if (span_percent < 0){
    span_percent = 0;
  }
  else {if (span_percent > 100){
    span_percent = 100;
    }
  }
  return span_percent;
}

void span_pct_string(int adc_measured_counts, char *span){
    sprintf(span,"%3d", span_pct_integer(adc_measured_counts));
    return;
}

void app_main(void)
{
    uint8_t dac_set_point=0;
    int adc_measured_counts=0;

    char span[4] = "";

    init_ADC();
    init_DAC();

    vTaskDelay(2 * portTICK_PERIOD_MS);

    ESP_LOGI(tag, "start conversion.\n");
    printf("  DAC Count    DAC VDC  ADC Count    ADC VDC   Span PCT"
    "   Digit[0]   Digit[1]   Digit[2]\n");

    while(1) {

        dac_output_voltage( DAC_EXAMPLE_CHANNEL, dac_set_point );
        adc_measured_counts = adc1_get_raw( ADC1_EXAMPLE_CHANNEL);

        span_pct_string(adc_measured_counts,span);
        printf("%11d %10.2f %10d %10.2f %10d %10c %10c %10c\n",
        dac_set_point, // 0 to 256 is the input range of the DAC
        dac_set_voltage(dac_set_point), //Calculated output voltage from the DAC
        adc_measured_counts, // input counts measured by ADC range is 0 to 4096
        adc_measured_voltage1(adc_measured_counts), //Calculated ADC voltage
        span_pct_integer(adc_measured_counts), //Span (0-100) recorded by ADC integer
        span[0], span[1], span[2]); //Span (0-100) recorded by ADC as string

        dac_set_point++;

        vTaskDelay( 2 * portTICK_PERIOD_MS );
    }
}
