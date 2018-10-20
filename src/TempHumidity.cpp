
#include "SHT15.h"
#include "TempHumidity.h"
#include <WS2812FX.h>

extern WS2812FX ws2812fx;
float temperature = 0;
float humidity = 0;

void TempHumiditySensing(float *ptr_Temp, float *ptr_Hum)
{
    temperature = sht15_get_temperature();
    humidity = sht15_get_humidity();
    *ptr_Temp=temperature;
    *ptr_Hum=humidity;

    if ((temperature <= 25.00) && (temperature >= 20.00))
    {
        ws2812fx.setColor(0xFF, 0, 0); //GRB
        ws2812fx.setMode(FX_MODE_STATIC);
    }
    if ((temperature > 25.00) && (temperature < 30.00))
    {
        ws2812fx.setColor(0x8C, 0xFF, 0); //GRB
        ws2812fx.setMode(FX_MODE_STATIC);
    }
    if (temperature >= 30.00)
    {
        ws2812fx.setColor(0, 0xFF, 0); //GRB
        ws2812fx.setMode(FX_MODE_STATIC);
    }
    ws2812fx.service();
}
