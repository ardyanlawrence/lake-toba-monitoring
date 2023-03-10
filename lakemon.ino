#include "time.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <Adafruit_MCP3008.h>

#define DAC1 25
#define DAC2 26

const char* ntpServer = "pool.ntp.org"; //ntp server yang dapat digunakan hanya yang internasional (gmt), tidak bisa local seperti server indonesia (gmt+7)
const long gmtOffset = 25200; //gmt+7 in seconds
const int daylightOffset = 0; //indonesia tidak punya daylight offset
 
OneWire onewire(23);
DallasTemperature sensors(&onewire);
 
Adafruit_TCS34725 TCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
uint16_t color[4];
const int phSample[7][4] =
{
  {382,  356,  210,  4},
  {345,  335,  202,  5},
  {314,  359,  258,  6},
  {241,  280,  193,  7},
  {208,  254,  189,  8},
  {187,  234,  190,  9},
  {184,  232,  199,  10}
}; //data sampel RGB dari setiap warna di strip pH, diambil dari color training
int phSize = sizeof(phSample)/sizeof(phSample[0]);
 
Adafruit_MCP3008 ADC;
 
int ph = 0;
float temp = 0.0;
int turbid = 0;
float light = 0.0;
float volt = 0.0;
float battPercent = 0.0;
 
void setup() {
  TCS.begin();
  ADC.begin(19, 17, 18, 16); //MCP3008 menggunakan SPI Software karena pin SPI Hardware sudah digunakan untuk sensor lain
  Serial.begin(115200);
 
  ph = identifyPH(); //mengambil nilai pH dari function identifyPH
  temp = getTemp(); //mengambil nilai suhu dari function getTemp
  turbid = getTurbidity(); //mengambil nilai turbiditas dari function getTurbidity
  light = getLux(); //mengambil nilai intensitas cahaya dari function getLux
  volt = getVolt(); //mengambil nilai voltase baterai dari function getVolt
  battPercent = getBatPercent(); //mengambil nilai persentase baterai dari function getBatPercent
 
  //tampilkan semua data yang sudah diperoleh ESP32
  

}
 
void loop() {
  float phToVolt = ((float)identifyPH())/10*255;
  float tempToVolt = (getTemp()/125)*255;
  
  Serial.print("Raw PH: ");
  Serial.println(phToVolt);

  Serial.print("Raw Temp: ");
  Serial.println(tempToVolt);


  
  dacWrite(DAC1,tempToVolt);
  dacWrite(DAC2,phToVolt);
  
  // put your main code here, to run repeatedly:
  Serial.print("pH: ");
  Serial.println(identifyPH());
  Serial.print("Temperature: ");
  Serial.print(getTemp());
  Serial.println(" °C");
  Serial.print("Kejernihan: ");
  Serial.println(getTurbidity());
  
  

  delay(1000);
}
 
int identifyPH() {
  TCS.getRawData(&color[0], &color[1], &color[2], &color[3]); //mengambil nilai RGB dalam bentuk unsigned integer 16 bit, jika tidak raw menggunakan int 8 bit (0-255), raw dipilih karena range 16 bit lebih luas sehingga akuarsi bisa lebih tinggi
  int falsecount = 0, minDis = 0, listProbability[7][2] = {{}}, listDistance[7] = {}, size = 0, colorDistance = 0;
  int maxDist = listDistance[0];
  for(int i=0; i<phSize; i++)
  {
    colorDistance = getDistance(color[0], color[1], color[2], phSample[i][0], phSample[i][1], phSample[i][2]);
    if(colorDistance < 60) {
      listProbability[size][0] = colorDistance;
      listProbability[size][1] = phSample[i][3];
      listDistance[size] = colorDistance;
      size++;
    }
  }
  Serial.print("R: ");
  Serial.println(color[0]);
  Serial.print("G: ");
  Serial.println(color[1]);
  Serial.print("B: ");
  Serial.println(color[2]);
  Serial.print("C: ");
  Serial.println(color[3]);
  if(color[3] > 2000){
    return 0;
  }else{
    if(size != 0) {
      for(int j=0; j<size; j++) {
        maxDist = max(listDistance[j], maxDist);
      }
      for(int k=0; k<size; k++)
      {
        if(listProbability[k][0] == maxDist) {
          return listProbability[k][1];
        }
      }
    }
    else {
      return 7;
    }
  }
  
}
 
//function mencari jarak dari satu warna ke warna lain
int getDistance(int rSens, int gSens, int bSens, int rSample, int gSample, int bSample) {
  return sqrt(pow(rSens - rSample, 2) + pow(gSens - gSample, 2) + pow(bSens - bSample, 2));
}
 
float getTemp() {
  float temp = 0;
  sensors.requestTemperatures();
  temp = sensors.getTempCByIndex(0);
  return temp;
}
 
int getTurbidity() {
  int analog = 0, mapped = 0, cons = 0;
  for(int i=0; i<1000; i++)
  {
    analog += ADC.readADC(1);
  }
  analog = analog / 1000;
  mapped = map(analog, 0, 966, 0, 100);
  cons = constrain(mapped, 0, 100);
  return cons;
}
 
float getLux() {
  float voltage = 0, current = 0, microCurr = 0, lux = 0;
  for(int i=0; i<1000; i++) {
    voltage += ADC.readADC(2) * (5.083 / 1023.0);
  }
  voltage = voltage / 1000;
  current = voltage / 10000;
  microCurr = current * 1000000;
  lux = microCurr * 2;
  return lux;
}
 
float getVolt() {
  float voltage = 0;
  for(int i=0; i<1000; i++) {
    voltage +=  ADC.readADC(3) * (5.083 / 1023.0);
  }
  voltage = voltage / 1000;
  return voltage;
}
 
float getBatPercent() {
  int percent = batMap(volt, 3.00, 4.19, 0, 100);
  percent = constrain(percent, 0, 100);
  return percent;
}
 
//custom map function dikarenakan built-in map menggunakan tipe data long, sedangkan data yang digunakan adalah float
long batMap(float x, float in_min, float in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
