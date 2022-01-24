// **NODE 1**

#include <LoRa.h>
#include <DHT.h>
#include "RTClib.h"
#include <SPI.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

#define SS 53
#define RST 43
#define DIO0 45

//dht22
#define DHTPIN 3          // what pin we're connected to
#define DHTTYPE DHT22     // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
float Y;
float hum;  //Stores humidity value
float temp; //Stores temperature value

//RTC
RTC_DS3231 rtc;
int hari, bulan, tahun, jam, menit;

//kecepatan_angin
#define windPin 2            // Receive the data from sensor
const float pi = 3.14159265; // pi number
int period = 10000;          // Measurement period (miliseconds)
int delaytime = 0;           // Time between samples (miliseconds)
int radio = 90;              // Distance from center windmill to outer cup (mm)
int jml_celah = 18;          // jumlah celah sensor
unsigned int Sample = 0;     // Sample number
unsigned int counter = 0;    // B/W counter for sensor
unsigned int RPM = 0;        // Revolutions per minute
float speedwind = 0;         // Wind speed (m/s)

//arah_angin
#define utara 4
#define tl 5
#define timur 6
#define tenggara 7
#define selatan 8
#define bd 9
#define barat 10
#define bl 11
String arahAngin = "";

//PENTING UNTUK PENGIRIMAN
int sender = 1;
int count;
StaticJsonDocument<200> doc;
String json_lora;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  // Set the pins windspeed
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  //set arah_angin pins
  pinMode(utara, INPUT_PULLUP);
  pinMode(tl, INPUT_PULLUP);
  pinMode(timur, INPUT_PULLUP);
  pinMode(tenggara, INPUT_PULLUP);
  pinMode(selatan, INPUT_PULLUP);
  pinMode(bd, INPUT_PULLUP);
  pinMode(barat, INPUT_PULLUP);
  pinMode(bl, INPUT_PULLUP);

  while (!Serial)
    ;
  Serial.println("Sender Host");

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }
  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(DATE), F(TIME)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2022, 1, 22, 20, 28, 0));
  }

  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6))
  {
    Serial.println("Starting LoRa Failed!");
    delay(100);
    while (1)
      ;
  }
  lcd.init(); // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();

  //  delay(100);
}
void loop()
{

  json_lora = "";
  arahAngin = "";

  //Ambil semua data sensor
  DateTime now = rtc.now();
  hari = now.day(), DEC;
  bulan = now.month(), DEC;
  tahun = now.year(), DEC;
  jam = now.hour(), DEC;
  menit = now.minute(), DEC;
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  Y = 0.50 * hum + 24.715;
  angin();
  if (digitalRead(utara) == LOW)
  {
    arahAngin = "utara";
  }
  else if (digitalRead(tl) == LOW)
  {
    arahAngin = "timur laut";
  }
  else if (digitalRead(timur) == LOW)
  {
    arahAngin = "timur";
  }
  else if (digitalRead(tenggara) == LOW)
  {
    arahAngin = "tenggara";
  }
  else if (digitalRead(selatan) == LOW)
  {
    arahAngin = "selatan";
  }
  else if (digitalRead(bd) == LOW)
  {
    arahAngin = "barat daya";
  }
  else if (digitalRead(barat) == LOW)
  {
    arahAngin = "barat";
  }
  else if (digitalRead(bl) == LOW)
  {
    arahAngin = "barat laut";
  }
  // Selesai ambil data

  Serial.println("Kirim dari NODE 2 - Take Off");

  doc["suhu"] = temp;
  doc["kelem"] = Y;
  doc["hari"] = hari;
  doc["bulan"] = bulan;
  doc["tahun"] = tahun;
  doc["kec"] = speedwind;
  doc["arah"] = arahAngin;
  doc["jam"] = jam;
  doc["menit"] = menit;
  serializeJson(doc, json_lora);
  //    Serial.println(json_lora);

  Serial.print(hari);
  Serial.print(" / ");
  Serial.print(bulan);
  Serial.print(" / ");
  Serial.print(tahun);
  Serial.print(" ---- ");
  Serial.print(jam);
  Serial.print(":");
  Serial.println(menit);

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C ");
  Serial.print("Humidity: ");
  Serial.print(Y);
  Serial.println(" % ");
  Serial.print("Kecepatan angin: ");
  Serial.print(speedwind);
  Serial.println(" [m/s] ");
  Serial.print("Arah angin: ");
  Serial.println(arahAngin);

  LoRa.beginPacket();
  LoRa.write(sender);
  LoRa.print(json_lora);
  LoRa.endPacket();

  lcd.setCursor(0, 0);
  lcd.print(hari);
  lcd.print('/');
  lcd.print(bulan);
  lcd.print('/');
  lcd.print(tahun);
  // lcd.print(" (");
  lcd.setCursor(1, 1);
  lcd.print(temp);
  lcd.setCursor(6, 1);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(8, 1);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print(Y);
  lcd.setCursor(15, 1);
  lcd.print(" %RH");
  lcd.setCursor(1, 2);
  lcd.print(speedwind);
  lcd.setCursor(6, 2);
  lcd.print("m/s");
  lcd.setCursor(11, 0);
  lcd.print(now.hour(), DEC);
  lcd.print(':');
  lcd.println(now.minute(), DEC);
  //    lcd.println();

  if (speedwind > 5)
  {
    lcd.setCursor(6, 3);
    lcd.print("WASPADA!");
    digitalWrite(13, HIGH);
    Serial.println("Angin lebih dari 5");
  }
  else
  {
    lcd.setCursor(1, 3);
    lcd.print("AMAN");
    digitalWrite(13, LOW);
    Serial.println("AMAN");
  }

  //  delay(3000);
}

void angin()
{
  Sample++;
  //  Serial.print(Sample);
  Serial.print(": Start measurement…");
  windvelocity();
  Serial.println(" finished.");
  //  Serial.print("Counter: ");
  //  Serial.print(counter);
  //  Serial.print("; RPM: ");
  RPMcalc();
  //  Serial.print(RPM);
  //  Serial.print("; Wind speed: ");
  WindSpeed();
  //  Serial.print(speedwind);
  //  Serial.print(" [m/s]");
  Serial.println();
}

// Measure wind speed
void windvelocity()
{
  speedwind = 0;
  counter = 0;
  attachInterrupt(0, addcount, CHANGE);
  unsigned long millis();
  long startTime = millis();
  while (millis() < startTime + period)
  {
  }

  detachInterrupt(1);
}

void RPMcalc()
{
  RPM = ((counter / jml_celah) * 60) / (period / 1000); // Calculate revolutions per minute (RPM)
}

void WindSpeed()
{
  speedwind = ((2 * pi * radio * RPM) / 60) / 1000; // Calculate wind speed on m/s
}

void addcount()
{
  counter++;
}
