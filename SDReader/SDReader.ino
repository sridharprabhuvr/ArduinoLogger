/*
 * Created by Sridhar Prabhu 
 * March 24 2018
 */
 
#include <Wire.h>
#include <SD.h>

//Temp and Pressure Sensors (I2C Communication)
int temp_address = 0; //INSERT THE ADDRESS OF THE TEMP SENSOR TC74A0
int pressure_address = 0; //INSERT THE ADDRESS OF THE PRESSURE SENSOR BMP180

//UltraSonic Sensor HC SR04
int trigPin = 0; //INSERT THE VALUE OF TRIGGERING PIN
int echoPin = 0; //INSERT THE VALUE OF THE ECHO PIN
const int sound_speed = 0.034; // cm/microsec

//Colour Sensor
//Define the pin number for s1, s2, s3, sensorOut
int S0, S1, S2, S3, sensorOut;

//Data Logging
long id = 1; //serial id for data
float refresh_rate = 100; //refresh rate in ms 
int powerPin = 0; //INSERT THE VALUE OF PIN FOR POWER
int csPin = 0; //INSERT THE VALUE OF CS PIN

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  //Setting Pin modes for Colour Detection
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  //Setting pin modes for distance
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Initialize Card
  if (!SD.begin(csPin)){
    Serial.println("Card Failure");
    return; //exit
  }
  Serial.println("Card OK");

  //Set Up Data file
  File dataFile = SD.open("Data.csv", FILE_WRITE); //NOTE FILE NAME SHOULD EXIST
  if (dataFile){
    String header = "ID, Temp, Pressure, Distance, R, G, B";

    dataFile.println(header);
    dataFile.close();

    Serial.println("Successful file access");
  } else{
    Serial.println("Couldnt open file.");
  }  
}

void loop() {
  int temp_val = get_value_I2C(temp_address);
  int pressure_val = get_value_I2C(pressure_address);
  int distance_val = get_distance();
  int red_val = get_red();
  int blue_val = get_blue();
  int green_val = get_green();

  //Data string
  String dataString = String(id) + ", " + String(temp_val) + ", " + String(pressure_val) + ", " + String(distance_val) + ", " + String(red_val) + ", " + String(green_val) + ", " + String(blue_val);

  //Write to file
  File dataFile = SD.open("Data.csv", FILE_WRITE);
  if (dataFile){
    dataFile.println(dataString);
    dataFile.close();
  }else{
    Serial.println("Couldnt write to file");
  }

  id++;
  delay(refresh_rate);
}

int get_value_I2C(int address){
  //request for the first byte from the sensor and return it
  
  Wire.beginTransmission(address);
  Wire.write(0);
  Wire.endTransmission();

  Wire.requestFrom(address, 1);

  while(Wire.available() == 0);
    return Wire.read();
}

int get_distance(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  //Sending pulso for 10micros
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long time = pulseIn(echoPin, HIGH);

  return time*sound_speed/2; // /2 as twice the distance
}


//The format used from https://howtomechatronics.com/tutorials/arduino/arduino-color-sensing-tutorial-tcs230-tcs3200-color-sensor/
int get_red(){
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

int get_green(){
  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

int get_blue(){
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  return pulseIn(sensorOut, LOW);
}

