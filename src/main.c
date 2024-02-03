#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "ssd1306.h"

#define DHT_PIN 15
#define WATER_PIN 26
#define SOIL_PIN 28
#define BUTTON_PIN 14

void setup_gpios_ssd1306(void);
void setup_gpios_gy302(void);
void draw_temp(int temp);
void draw_humidity(int humidity);
void draw_lumen(float lumen);
void draw_water(int water);
void draw_soil(int soil);
void read_lumen();
bool read_dht11();
void read_water();
void read_soil();
void displayValue();

int temp = 0;
int humidity = 0;
float lumen = 0;
int water = 0;
int soil = 0;

char displayString[50];

enum Sensor{
    TEMPERATURE,
    HUMIDITY,
    LUMEN,
    WATER,
    SOIL
};

const char* sensorStrings[] = {
    "Temperature: ",
    "Humidity: ",
    "Lumen: ",
    "Water: ",
    "Soil Moisture: "
};

enum Sensor currentSensor = TEMPERATURE;

ssd1306_t disp;


int main() {
    stdio_init_all();

    //Setup Button
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    // Setup ADC
    adc_init();
    adc_gpio_init(WATER_PIN);
    adc_gpio_init(SOIL_PIN);

    // Setup I2C
    setup_gpios_ssd1306();
    setup_gpios_gy302();

    // Setup SSD1306
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 32, 0x3C, i2c1);

    while(1){
        
        currentSensor = TEMPERATURE;
        displayValue();
        sleep_ms(1000);
        currentSensor = HUMIDITY;
        displayValue();
        sleep_ms(1000);
        currentSensor = LUMEN;
        displayValue();
        sleep_ms(1000);
        currentSensor = WATER;
        displayValue();
        sleep_ms(1000);
        currentSensor = SOIL;
        displayValue();
        sleep_ms(1000);
        

    }

    return 0;
}

void setup_gpios_ssd1306(void) {
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
}

void setup_gpios_gy302(void){
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    uint8_t cmd =  0x10;
    i2c_write_blocking(i2c0, 0x23, &cmd, 1, false);
}

bool read_dht11() {
    uint8_t data[5] = {0};
    uint8_t byte_index = 0, bit_index = 7;
    uint32_t start_time;

    gpio_init(DHT_PIN);
    gpio_set_dir(DHT_PIN, GPIO_OUT);
    gpio_put(DHT_PIN, 0);
    sleep_ms(18); 
    gpio_set_dir(DHT_PIN, GPIO_IN);
    sleep_us(40); 

    while (!gpio_get(DHT_PIN)); 
    while (gpio_get(DHT_PIN));

    for (int i = 0; i < 40; i++) {
        while (!gpio_get(DHT_PIN));

        start_time = time_us_32();
        while (gpio_get(DHT_PIN)); 
        if ((time_us_32() - start_time) > 50) { 
            data[byte_index] |= (1 << bit_index);
        }
        if (bit_index == 0) {
            bit_index = 7;
            byte_index++;
        } else {
            bit_index--;
        }
    }

    if (((data[0] + data[1] + data[2] + data[3]) & 0xFF) == data[4]) {
        humidity = data[0];
        temp = data[2];
        return true;
    }

    return false;
}

void displayValue(){
    switch (currentSensor)
    {
    case TEMPERATURE:
        read_dht11();
        ssd1306_clear(&disp);
        sprintf(displayString,"%s%d", sensorStrings[TEMPERATURE], temp);
        ssd1306_draw_string(&disp, 8, 2, 1, displayString);
        ssd1306_show(&disp);
        break;
    case HUMIDITY:
        //read_dht11(); TODO
        ssd1306_clear(&disp);
        sprintf(displayString,"%s%d", sensorStrings[HUMIDITY], humidity);
        ssd1306_draw_string(&disp, 8, 2, 1, displayString);
        ssd1306_show(&disp);
        break;
    case LUMEN:
        read_lumen();
        ssd1306_clear(&disp);
        sprintf(displayString,"%s%f", sensorStrings[LUMEN], lumen);
        ssd1306_draw_string(&disp, 8, 2, 1, displayString);
        ssd1306_show(&disp);
        break;
    case WATER:
        read_water();
        ssd1306_clear(&disp);
        sprintf(displayString,"%s%d", sensorStrings[WATER], water);
        ssd1306_draw_string(&disp, 8, 2, 1, displayString);
        ssd1306_show(&disp);
        break;
    case SOIL:
        read_soil();
        ssd1306_clear(&disp);
        sprintf(displayString,"%s%d", sensorStrings[SOIL], soil);
        ssd1306_draw_string(&disp, 8, 2, 1, displayString);
        ssd1306_show(&disp);
        break;
    
    default:
        break;
    }
}

void draw_temp(int temp) {
    ssd1306_clear(&disp);
    char tempString[50];
    sprintf(tempString, "Temperature: %d", temp);
    ssd1306_draw_string(&disp, 8, 2, 1, tempString);
    ssd1306_show(&disp);
}

void draw_humidity(int humidity) {
    ssd1306_clear(&disp);
    char humidityString[50];
    sprintf(humidityString, "Humidity: %d", humidity);
    ssd1306_draw_string(&disp, 8, 2, 1, humidityString);
    ssd1306_show(&disp);
}

void draw_lumen(float lumen) {
    ssd1306_clear(&disp);
    char lumenString[50];
    sprintf(lumenString, "Lumen: %f", lumen);
    ssd1306_draw_string(&disp, 8, 2, 1, lumenString);
    ssd1306_show(&disp);
}

void draw_water(int water){
    ssd1306_clear(&disp);
    char waterString[50];
    sprintf(waterString, "Water: %d", water);
    ssd1306_draw_string(&disp, 8, 2, 1, waterString);
    ssd1306_show(&disp);
}

void draw_soil(int soil){
    ssd1306_clear(&disp);
    char soilString[50];
    sprintf(soilString, "Soil Moisture: %d", soil);
    ssd1306_draw_string(&disp, 8, 2, 1, soilString);
    ssd1306_show(&disp);
}

void read_water(){
    adc_select_input(0);
    water = adc_read() * 100 / 4095;
}

void read_soil(){
    adc_select_input(2);
    soil = 100 - (adc_read() * 100 / 4095);
}

void read_lumen(){
    uint8_t data[2];
    i2c_read_blocking(i2c0, 0x23, data, 2, false);
    lumen = ((data[0] << 8) | data[1]) / 1.2;
}
