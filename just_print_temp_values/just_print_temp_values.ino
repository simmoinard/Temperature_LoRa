#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"

//Set these OTAA parameters to match your app/node in TTN
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x31, 0x5D };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[] = { 0x77, 0xA4, 0x46, 0x4A, 0x0A, 0x99, 0xBF, 0x62, 0x44, 0xAF, 0x05, 0x69, 0x5A, 0x80, 0x35, 0x56 };
int temps = 120; // Indiquez dans cette ligne la fréquence d'envoi de données, en secondes. (Ne pas aller plus bas que 3minutes, soit 180sec)

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
static uint8_t counter=0;
uint8_t lora_data[5];
uint8_t downlink ;

///////////////////////////////////////////////////
void setup() {
	Serial.begin(115200);
}


///////////////////////////////////////////////////
void loop()
{
  counter++; 
  delay(10);
  uint8_t voltage = getBatteryVoltage()/50; //Tension en %

  Serial.print("Batterie = ");
  Serial.println(voltage);

  int sensorValue2 = analogRead(ADC2);
  int sensorValue3 = analogRead(ADC3);
  
  float tension2 = (sensorValue2)/4094.00*2.40;
  float tempfloat2 = (tension2 -0.5) * 100 ;
  int temp2 = tempfloat2 *100 + 5000;
  float tension3 = (sensorValue3);
  float tempfloat3 = (tension3);
  int temp3 = 4094-tempfloat3;

  Serial.print("Volt2 = ");
  Serial.print(tension2);
  Serial.print(" // ");
  Serial.print("Tempfloat2 = ");
  Serial.println(tempfloat2);
  
  Serial.print("Volt3 = ");
  Serial.print(tension3);
  Serial.print(" // ");
  Serial.print("Tempfloat3 = ");
  Serial.println(tempfloat3);

  lora_data[0] = voltage;
  lora_data[1] = highByte(temp2);
  lora_data[2] = lowByte(temp2);
  lora_data[3] = highByte(temp3);
  lora_data[4] = lowByte(temp3);
  
  delay(1000);
}
