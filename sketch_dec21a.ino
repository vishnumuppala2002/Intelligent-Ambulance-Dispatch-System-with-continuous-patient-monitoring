#include <LiquidCrystal.h>
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


#include <Wire.h>
#include "MAX30105.h"
MAX30105 particleSensor;

#define Serial Serial //Uncomment this line if you're using an Uno or ESP
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS A1
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


#include "HX711.h"
const int dout = 6;
const int sck = 7;
HX711 scale;

int h, pressure;
const int PulseWire = A3;
int Threshold = 550;
PulseSensorPlayground pulseSensor;
float tempval, changetemp, t, tf;
int p, oxygenval;
int bpm, myBPM, humidity, temp, tempsend;
int irValue;

void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("PATIENT MONITORING");
  scale.begin(dout, sck);
  sensors.begin();
  sensors.requestTemperatures();
  dht.begin();
  t = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  tf = DallasTemperature::toFahrenheit(t);

  changetemp = settemp - tf;
  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  pulseSensor.begin();


  pinMode(PulseWire, INPUT);


  delay(1000);
  scale.read();
  scale.read_average(20);
  scale.get_value(5);
  (scale.get_units(5), 1);
  scale.set_scale(2280.f);
  scale.tare();
  scale.read();
  scale.read_average(20);
  scale.get_value(5);
  (scale.get_units(5), 1);

  // Initialize sensor
  if (particleSensor.begin() == false)
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");

  }

  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED dri


  delay(300);
  lcd.clear();
}


void loop()
{

  h = dht.readHumidity();
  h = h / 2;
  lcd.setCursor(9, 0);
  lcd.print("R:");
  if (h <= 9) {
    lcd.print("0");
    lcd.print(h);
  }
  else if (h <= 99) {
    lcd.print("");
    lcd.print(h);
  }
  lcd.print(" ");


  //OXYGEN
  oxygenval = particleSensor.getIR();

  Serial.print("ir value :");
  Serial.println(irValue);

  if (oxygenval <= 95) {
    oxygenval = 0;
  }
  if (oxygenval >= 100) {
    oxygenval = 100;
  }

  Serial.print("OXYGEN:");
  Serial.println(oxygenval);

  sensors.requestTemperatures();
  // Fetch the temperature in degrees Celsius for device index:
  float t = sensors.getTempCByIndex(0); // the index 0 refers to the first device
  tf = DallasTemperature::toFahrenheit(t);
  tf = tf + changetemp;
  if (tf <= 97) {
    tf = 0;
  }
  else if (tf >= 103) {
    tf = 103;
  }
 
  tempsend = tf * 100;


  //bpm....................................................................................

  myBPM = pulseSensor.getBeatsPerMinute();
  myBPM = analogRead(PulseWire);

  //to read pressure...................................................

  (scale.get_units(), 1);

  p = scale.get_units(10);
  (p, 1);
  Serial.print("PRESSURE:");
  Serial.println(p);
  p = map(p, 0, 2500, 0, 130);
  scale.power_down();              // put the ADC in sleep mode
  delay(500);
  scale.power_up();

  Serial.print("P:");
  Serial.println(p);

  if (p < 0) {
    p = 0;
  }
  lcd.setCursor(7, 1); lcd.print("P:");
  if (p <= 9) {
    lcd.print("00");
    lcd.print(p);
  }
  else if (p <= 99) {
    lcd.print("0");
    lcd.print(p);
  }
  else if (p <= 999) {
    lcd.print(p);
  }

  if (oxygenval < 0) {
    oxygenval = 0;
  }
  lcd.setCursor(11, 1); lcd.print("OX:");
  if (oxygenval <= 9) {
    lcd.print("0");
    lcd.print(oxygenval);
  }
  else if (oxygenval <= 99) {
    lcd.print("");
    lcd.print(oxygenval);
  }

  if (tf < 0) {
    tf = 0;
  }
  lcd.setCursor(0, 0); lcd.print("T:");
  if (tf <= 9) {
    lcd.print("00");
    lcd.print(tf);
  }
  else if (tf <= 99) {
    lcd.print("0");
    lcd.print(tf);
  }
  else if (tf <= 999) {
    lcd.print(tf);
  }

  if (bpm < 0) {
    bpm = 0;
  }
  lcd.setCursor(0, 1); lcd.print("PU:");
  if (bpm <= 9) {
    lcd.print("00");
    lcd.print(bpm);
  }
  else if (bpm <= 99) {
    lcd.print("0");
    lcd.print(bpm);
  }
  else if (bpm <= 999) {
    lcd.print(bpm);
  }

  delay(200);

  senddata();
}

void senddata()
{
  Serial.print('*');


  if (bpm <= 9) {
    Serial.print("00");
    Serial.print(bpm);
  }
  else if (bpm <= 99) {
    Serial.print("0");
    Serial.print(bpm);
  }
  else if (bpm <= 999) {
    Serial.print(bpm);
  }

  if (h <= 9) {
    Serial.print("00");
    Serial.print(h);
  }
  else if (h <= 99) {
    Serial.print("0");
    Serial.print(h);
  }
  else if (h <= 999) {
    Serial.print(h);
  }


  if (oxygenval <= 9) {
    Serial.print("00");
    Serial.print(oxygenval);
  }
  else if (oxygenval <= 99) {
    Serial.print("0");
    Serial.print(oxygenval);
  }
  else if (oxygenval <= 999) {
    Serial.print(oxygenval);
  }

  if (p <= 9) {
    Serial.print("00");
    Serial.print(p);
  }
  else if (p <= 99) {
    Serial.print("0");
    Serial.print(p);
  }
  else if (p <= 999) {
    Serial.print(p);
  }

  if (tempsend <= 9) {
    Serial.print("0000");
    Serial.print(tempsend);
  }
  else if (tempsend <= 99) {
    Serial.print("000");
    Serial.print(tempsend);
  }
  else if (tempsend <= 999) {
    Serial.print("00");
    Serial.print(tempsend);
  }
  else if (tempsend <= 9999) {
    Serial.print("0");
    Serial.print(tempsend);
  }
  else if (tempsend <= 99999) {
    Serial.print(tempsend);
  }
}
