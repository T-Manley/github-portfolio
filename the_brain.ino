#include <Wire.h>
#include "SPL07-003.h"
#include "rgb_lcd.h"

//Sensors
#define SPL07_ADDR SPL07_ADDR_DEF

SPL07_003 spl;
rgb_lcd lcd;

//Variables
int qualityValue = 0;

const unsigned long displayInterval = 30000; //30 secs, so the user can read the data

bool stormApproaching = false;

float smoothedPressure = 0;
float pressureNow = 0;
float pressure3hrAgo = 0;
float pressure12hrAgo = 0;
float pressureShortAgo = 0;

unsigned long last3hrUpdate = 0;
unsigned long last12hrUpdate = 0;
unsigned long lastShortUpdate = 0;

//Rolla elevation correction, change elevationMeters if not in Rolla lol
float elevationMeters = 350.52;

String deployCanopy = "No";


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Wire.begin();

  pinMode(A0, INPUT); //light sensor
  
  pinMode(A1, INPUT); //air quality

  //LCD setup
  lcd.begin(16, 2); //dimensions
  lcd.setRGB(0, 128, 255);
  lcd.clear();

  //Setup SPA06-003 situation
  if (!spl.begin(SPL07_ADDR, &Wire)) { 
    Serial.println("Could not find SPL07 sensor!");
    lcd.print("SPL07 Error!");
    while (1); //terminates
  }

  spl.setPressureConfig(SPL07_4HZ, SPL07_32SAMPLES);
  spl.setTemperatureConfig(SPL07_4HZ, SPL07_1SAMPLE);

  spl.setMode(SPL07_CONT_PRES_TEMP);
  
  lcd.setCursor(0,0);
  lcd.print("Sensors Ready!");
  Serial.println("Sensors Ready!");

  //Pressure tracking with chatgpt, my goat
  delay(2000);
  float pressurePa = spl.readPressure();
  float pressureHPa = pressurePa / 100.0; //hPa to detect mbar/hPa

  float seaLevelPressure = pressureHPa / pow(1 - (elevationMeters / 44330.0), 5.255);


  pressureNow = seaLevelPressure;
  //smoothedPressure = pressureNow;
  

  pressure3hrAgo = pressureNow;
  pressure12hrAgo = pressureNow;
  pressureShortAgo = pressureNow;

  last3hrUpdate = millis();
  last12hrUpdate = millis();
  lastShortUpdate = millis();

  stormApproaching = false;

}

void loop() {
  
  stormApproaching = false;

  delay(1000);

  //Some variables for the barometer
  float pressurePa = spl.readPressure();
  float pressureHPa = pressurePa / 100.0; //hPa to detect mbar/hPa

  float seaLevelPressure = pressureHPa / pow(1 - (elevationMeters / 44330.0), 5.255);


  pressureNow = seaLevelPressure;
  //smoothedPressure = (smoothedPressure * 0.9) + (seaLevelPressure * 0.1); //clear noise of flucuation
  

  float pressureInHg = pressureNow * 0.02953; //hPa --> inHg
  float tempC = spl.readTemperature();
  float tempF = tempC * 1.8 + 32; //C to F

  unsigned long currentMillis = millis();

  //3 hour pressure
  if ((currentMillis - last3hrUpdate) >= 10800000UL) {
    pressure3hrAgo = pressureNow;
    last3hrUpdate = currentMillis;
  }
  

  //12 hour pressure
  if ((currentMillis - last12hrUpdate) >= 43200000UL) {
    pressure12hrAgo = pressureNow;
    last12hrUpdate = currentMillis;
  }


  //short-term pressure (~3 minutes)
  if ((currentMillis - lastShortUpdate) >= 180000UL) {
    pressureShortAgo = pressureNow;
    lastShortUpdate = currentMillis;
  }


  //Simple storm detection
  if (millis() > 1) {

    //storm check for 3, 12, and fast drop
    if ((pressure3hrAgo != 0) && ((pressureNow - pressure3hrAgo) <= -4.1)) {
      stormApproaching = true;
    }

    if ((pressure12hrAgo != 0) && ((pressureNow - pressure12hrAgo) <= -8.1)) {
      stormApproaching = true;
    }

    if ((pressureShortAgo != 0) && (pressureInHg <= 29.4) && ((pressureNow - pressureShortAgo) <= -0.55)) {
      stormApproaching = true;
    }
  }
  
  if (!stormApproaching)
    lcd.setRGB(0, 128, 255);

  /*ping sensors 0 through 3
  qualityValue = analogRead(A1);
  lightValue = analogRead(A0);
  pressureTempValue = analogRead(A3); */

  /* output data to LCD
  Serial.println(lightValue);
  Serial.println(pressureTempValue); */
  

  //Readings
  //qualityValue = airqualitysensor.slope();
  int lightValue = analogRead(A0);
  qualityValue = analogRead(A1);

  //print inital values

  Serial.print("Air Raw: ");
  Serial.println(qualityValue);

  Serial.print("Light: ");
  Serial.println(lightValue);

  
  //Air shit

  if (qualityValue > 700) {
    Serial.println("Air Quality: High Pollution");
  } else if (qualityValue > 400) {
    Serial.println("Air Quality: Moderate");
  } else {
    Serial.println("Air Quality: Fresh");
  }
  //End Air shit, minus isr timer2 situation

  //Begin lightsaber
  if (lightValue > 700) {
    Serial.println("Light status: VERY BRIGHT");
  } else if (lightValue >= 400) {
    Serial.println("Light status: OVERCAST");
  } else {
    Serial.println("Light status: DARK");
  }

  //barometer drop of 4mb in 3 hours or 8mb in 12 hours;
  // below 29.8, dropping quickly
  Serial.print("Pressure: "); Serial.print(seaLevelPressure); Serial.println(" hPa");
  Serial.print("Pressure: "); Serial.print(pressureInHg); Serial.println(" inHg");
  
  //tempature
  Serial.print("Temperature: "); Serial.print(tempC); Serial.println(" °C");
  Serial.print("Temperature: "); Serial.print(tempF); Serial.println(" °F");

  //storm?
  if (stormApproaching) {
    Serial.println("Storm Approaching!");
  } else {
    Serial.println("Weather Stable");
  }

  Serial.println("------------------------");


  //Print to LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("P: ");
  lcd.print(seaLevelPressure,1);
  lcd.print(" hPa");

  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(tempF, 0);
  lcd.print("F");


  if (stormApproaching) {

    lcd.setCursor(10, 1);
    lcd.print("Storm!");

    //flashes yellow and stays yellow if there's a storm
    lcd.setRGB(255, 255, 0);
    delay(150);
    lcd.setRGB(255, 255, 255);
    delay(150);
    lcd.setRGB(255, 255, 0);
    delay(150);
    lcd.setRGB(255, 255, 255);
    delay(150);
    lcd.setRGB(200, 200, 0);

  }

  //tests for canopy deployment
  //tempF = 40;
  //tempF = 86;


  
  //If statements for deployable canopy/coverage/aunning
  if ((tempF <= 50) || (tempF >= 85)) {
    deployCanopy = "Yes, too temperature.";
  } else if (stormApproaching == true) {
    deployCanopy = "Yes, Storm.";
  } 

  Serial.println("Canopy deployment?");
  Serial.println(deployCanopy);

  Serial.println("------------------------");
    
  delay(displayInterval);
    
  
}
