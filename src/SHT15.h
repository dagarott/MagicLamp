#ifndef SHT15_H
#define SHT15_H

//Define data pin and clk pin
#define _SHT_DATA_PIN_ 12
#define _SHT_CLK_PIN_ 13

float sht15_get_temperature();
float sht15_get_humidity();
void sht_send_Command(int command, int data_pin, int clock_pin);
void sht_wait_for_result(int data_pin);
int sht_get_data(int data_pin, int clock_pin);
void sht_skip_crc(int data_pin, int clock_pin);

#endif
