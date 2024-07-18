#include "LoRaWanMinimal_APP.h"
#include "Arduino.h"

//Set these OTAA parameters to match your app/node in TTN
uint8_t devEui[] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x07, 0xB2 };
uint8_t appEui[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85 };
uint8_t appKey[] = { 0x3F, 0x0D, 0x57, 0x75, 0x3C, 0x8E, 0xC7, 0x0E, 0x3E, 0xEA, 0x5E, 0xFC, 0x01, 0xFA, 0x5B, 0x3A };
int temps = 100; // Indiquez dans cette ligne la fréquence d'envoi de données, en secondes. (Ne pas aller plus bas que 3minutes, soit 180sec)

uint16_t userChannelsMask[6]={ 0x00FF,0x0000,0x0000,0x0000,0x0000,0x0000 };
static uint8_t counter=0;
uint8_t lora_data[4];
uint8_t downlink ;

///////////////////////////////////////////////////
//Some utilities for going into low power mode
TimerEvent_t sleepTimer;
//Records whether our sleep/low power timer expired
bool sleepTimerExpired;

static void wakeUp()
{
  sleepTimerExpired=true;
}

static void lowPowerSleep(uint32_t sleeptime)
{
  sleepTimerExpired=false;
  TimerInit( &sleepTimer, &wakeUp );
  TimerSetValue( &sleepTimer, sleeptime );
  TimerStart( &sleepTimer );
  //Low power handler also gets interrupted by other timers
  //So wait until our timer had expired
  while (!sleepTimerExpired) lowPowerHandler();
  TimerStop( &sleepTimer );
}

///////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  pinMode(GPIO7,OUTPUT);
  digitalWrite(GPIO7,LOW);

  LoRaWAN.begin(LORAWAN_CLASS, ACTIVE_REGION);
  
  //Enable ADR
  LoRaWAN.setAdaptiveDR(true);

  while (1) {
    Serial.print("Joining... ");
    LoRaWAN.joinOTAA(appEui, appKey, devEui);
    if (!LoRaWAN.isJoined()) {
      //In this example we just loop until we're joined, but you could
      //also go and start doing other things and try again later
      Serial.println("JOIN FAILED! Sleeping for 30 seconds");
      lowPowerSleep(30000);
    } else {
      Serial.println("JOINED");
      break;
    }
  }
}


///////////////////////////////////////////////////
void loop()
{
  counter++; 
  
  uint8_t voltage = getBatteryVoltage()/50; //Voltage in %
  
  int sensorValue2 = analogRead(ADC2);
  int sensorValue3 = analogRead(ADC3)/64;
  
  float tension2 = (sensorValue2)/4094.00*2.40;
  float tempfloat2 = (tension2 -0.5) * 100 ;
  int temp2 = tempfloat2 *100 + 5000;
  float tension3 = (sensorValue3);
  float tempfloat3 = (255-tension3)*100.00/255.00;
  int temp3 = (tempfloat3);

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
  lora_data[3] = temp3;
  //Now send the data. The parameters are "data size, data pointer, port, request ack"
  Serial.printf("\nSending packet with counter=%d\n", counter);
  Serial.printf("\nValue 1 to send : %d\n", lora_data[1]);

  //Here we send confirmed packed (ACK requested) only for the first two (remember there is a fair use policy)
  bool requestack=counter<2?true:false;
  if (LoRaWAN.send(sizeof(lora_data), lora_data, 1, requestack)) {
    Serial.println("Send OK");
  } else {
    Serial.println("Send FAILED");
  }

  lowPowerSleep(temps*1000);  
  delay(10);
}
