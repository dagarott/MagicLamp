
#include "SHT15.h" 
#include "Arduino.h" 

//BEGIN TEMPERATURE-HUMIDITY SENSOR
float sht15_get_temperature()
{
  //Return Temperature in Celsius
  sht_send_Command(0x03, _SHT_DATA_PIN_, _SHT_CLK_PIN_);
  sht_wait_for_result(_SHT_DATA_PIN_);

  int val = sht_get_data(_SHT_DATA_PIN_, _SHT_CLK_PIN_);
  sht_skip_crc(_SHT_DATA_PIN_, _SHT_CLK_PIN_);
  return (float)val * 0.01 - 40; //convert to celsius
}

float sht15_get_humidity()
{
  //Return  Relative Humidity
  sht_send_Command(0x05, _SHT_DATA_PIN_, _SHT_CLK_PIN_);
  sht_wait_for_result(_SHT_DATA_PIN_);

  int val = sht_get_data(_SHT_DATA_PIN_, _SHT_CLK_PIN_);
  sht_skip_crc(_SHT_DATA_PIN_, _SHT_CLK_PIN_);

  return -4.0 + 0.0405 * val + -0.0000028 * val * val;
}

void sht_send_Command(int command, int data_pin, int clock_pin)
{
  // send a command to the SHTx sensor
  // transmission start
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  digitalWrite(data_pin, HIGH);
  digitalWrite(clock_pin, HIGH);
  digitalWrite(data_pin, LOW);
  digitalWrite(clock_pin, LOW);
  digitalWrite(clock_pin, HIGH);
  digitalWrite(data_pin, HIGH);
  digitalWrite(clock_pin, LOW);

  // shift out the command (the 3 MSB are address and must be 000, the last 5 bits are the command)
  shiftOut(data_pin, clock_pin, MSBFIRST, command);
  delay(10);

  // verify we get the right ACK
  digitalWrite(clock_pin, HIGH);
  delay(10);

  pinMode(data_pin, INPUT);
  delay(10);

  if (digitalRead(data_pin))
  {
    Serial.println("ACK error 0");
  }
  delay(10);
  digitalWrite(clock_pin, LOW);
  delay(10);

  if (!digitalRead(data_pin))
  {
    Serial.println("ACK error 1");
  }
}

void sht_wait_for_result(int data_pin)
{
  // wait for the SHTx answer
  pinMode(data_pin, INPUT);

  uint8_t ack; //acknowledgement

  //need to wait up to 2 seconds for the value
  for (int i = 0; i < 1000; ++i)
  {
    delay(20);
    ack = digitalRead(data_pin);
    if (ack == LOW)
      break;
  }

  if (ack == HIGH)
  {
    Serial.println("ACK error 2");
  }
}

int sht_get_data(int data_pin, int clock_pin)
{
  // get data from the SHTx sensor

  // get the MSB (most significant bits)
  pinMode(data_pin, INPUT);
  pinMode(clock_pin, OUTPUT);
  byte MSB = shiftIn(data_pin, clock_pin, MSBFIRST);

  // send the required ACK
  pinMode(data_pin, OUTPUT);
  digitalWrite(data_pin, HIGH);
  digitalWrite(data_pin, LOW);
  digitalWrite(clock_pin, HIGH);
  digitalWrite(clock_pin, LOW);

  // get the LSB (less significant bits)
  pinMode(data_pin, INPUT);
  byte LSB = shiftIn(data_pin, clock_pin, MSBFIRST);
  return ((MSB << 8) | LSB); //combine bits
}

void sht_skip_crc(int data_pin, int clock_pin)
{
  // skip CRC data from the SHTx sensor
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  digitalWrite(data_pin, HIGH);
  digitalWrite(clock_pin, HIGH);
  digitalWrite(clock_pin, LOW);
}
//END TEMPERATURE-HUMIDITY SENSOR