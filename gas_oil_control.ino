#include <String.h>

//DHT Declearation
#include <DHT.h>
#define DHTPIN 12
DHT dht(DHTPIN, DHT11);

//UART Declearation
#include <SoftwareSerial.h>
SoftwareSerial GPRSserial(3, 4);

//Flow Meter Declearation
#include <FlowMeter.h>
FlowMeter Meter = FlowMeter(2);

void MeterISR() {
  Meter.count();
}

//Setup function
void setup() {

  MeterISR();
  attachInterrupt(INT0, MeterISR, RISING);
  Meter.reset();

  Serial.begin(9600); //This is in case you use tx and rx with the default baud rate
  GPRSserial.begin(9600);//Software serial begin with the default baud rate
  dht.begin(); //DHT begin
  Serial.println("AT"); //Activate serial monitor
  GPRSserial.println("AT"); //Activate serial monitor for GPRS
  delay(1000);

  Serial.println("AT+CPIN?");
  GPRSserial.println("AT+CPIN?");
  delay(1000);

  Serial.println("AT+CREG?");
  GPRSserial.println("AT+CREG?");
  delay(1000);

  Serial.println("AT+CGATT?");
  GPRSserial.println("AT+CGATT?");
  delay(1000);

  Serial.println("AT+CIPSHUT");
  GPRSserial.println("AT+CIPSHUT");
  delay(1000);

  Serial.println("AT+CIPSTATUS");
  GPRSserial.println("AT+CIPSTATUS");
  delay(2000);

  Serial.println("AT+CIPMUX=0");
  GPRSserial.println("AT+CIPMUX=0");
  delay(2000);
  ShowSerialData();//Sub function

  Serial.println("AT+CSTT=\"mtn.com.ng\"");//start task and setting the APN
  GPRSserial.println("AT+CSTT=\"mtn.com.ng\"");//start task and setting the APN Software serial
  delay(1000);
  ShowSerialData();

  Serial.println("AT+CIICR");//bring up wireless connection
  GPRSserial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
  ShowSerialData();

  Serial.println("AT+CIFSR");//get local IP adress
  GPRSserial.println("AT+CIFSR");//get local IP adress software serial
  delay(2000);
  ShowSerialData();

  Serial.println("AT+CIPSPRT=0");
  GPRSserial.println("AT+CIPSPRT=0");
  delay(3000);
  ShowSerialData();

  Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  GPRSserial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  delay(6000);
  ShowSerialData();

  delay(1000);
  Serial.println("AT+CIPSEND");//begin send data to remote server
  GPRSserial.println("AT+CIPSEND");//begin send data to remote server software serial
  delay(4000);
  ShowSerialData();
}

void loop() {
  //Assigning the float value h to the command
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float VT = Meter.getTotalVolume();
  float FR = Meter.getCurrentFlowrate();
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.println(" °C");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println(" %");
  Serial.print("Flow rate = ");
  Serial.print(FR);
  Serial.println(" l/min");
  Serial.print("Volume = ");
  Serial.print(VT);
  Serial.println(" L");

  GPRSserial.print("Temperature = ");
  GPRSserial.print(t);
  GPRSserial.println(" °C");
  GPRSserial.print("Humidity = ");
  GPRSserial.print(h);
  GPRSserial.println(" %");
  GPRSserial.print("Flow rate = ");
  GPRSserial.print(FR);
  GPRSserial.println(" l/min");
  GPRSserial.print("Volume = ");
  GPRSserial.print(VT);
  GPRSserial.println(" L");

  if (GPRSserial.available())
    Serial.write(GPRSserial.read());
  String str = "GET https://api.thingspeak.com/update?api_key=5GAUUB5IY7LCOZBC&field1=" + String(t) + "&field2=" + String(h) + "&field3=" + String(FR) + "&field4=" + String(VT);
  Serial.println(str);
  GPRSserial.println(str);//Begin send data to remote server
  Serial.write(GPRSserial.read());
  delay(1000);
}
void ShowSerialData()
{
  while (GPRSserial.available() != 0)
    Serial.write(GPRSserial.read());
  delay(5000);
}
