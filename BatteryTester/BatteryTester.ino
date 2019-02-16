#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

int row1 = 20;
int row2 = 40;
int col1 = 0;
int col2 = 30;
int col3 = 60;
int col4 = 90;

int c = 0;

String mDay;
String mMonth;
String mYear;
String mHour;
String mMinute;
String mSecond;

String filename;

#include <Arduino.h>
#include <U8g2lib.h>
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
int btn = 2; // btn display
int draw_state = 0;
File myFile;

static uint32_t timer;
int curr_1_pin = A4;
int curr_2_pin = A3;
int curr_3_pin = A2;
int curr_4_pin = A0;

int vol_1_pin = A5;
int vol_2_pin = A6;
int vol_3_pin = A7;
int vol_4_pin = A8;

int vol_1_offset = 0;
int vol_2_offset = 5;
int vol_3_offset = 5;
int vol_4_offset = 5;

float curr_1_offset = -1.8;
float curr_2_offset = 1.5;
float curr_3_offset = 0.8;
float curr_4_offset = 0.5;




const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
const int model = 0;   // enter the model number (see below)

const float QOV =   0.5 * VCC;// set quiescent Output voltage of 0.5V

float C_voltage_1;// internal variable for voltage
float current_1;
float lastCurrent_1;

float C_voltage_2;// internal variable for voltage
float current_2;
float lastCurrent_2;

float C_voltage_3;// internal variable for voltage
float current_3;
float lastCurrent_3;

float C_voltage_4;// internal variable for voltage
float current_4;
float lastCurrent_4;

float sensitivity[] = {
  0.185,// for ACS712ELCTR-05B-T
  0.100,// for ACS712ELCTR-20A-T
  0.066// for ACS712ELCTR-30A-T
};
RTC_Millis rtc;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////                  SETUP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  DateTime now = rtc.now();
  //////////////////////////////////////////////////////////////DATE STRING
  if ((now.day()) <= 9) {
    mDay = "0" + String(now.day());
  } else {
    mDay = String(now.day());
  }
  if ((now.month()) <= 9) {
    mMonth = "0" + String(now.month());
  } else {
    mMonth = String(now.month());
  }
  mYear = now.year();

  if ((now.hour()) <= 9) {
    mHour = "0" + String(now.hour());
  } else {
    mHour = String(now.hour());
  }
  if ((now.minute()) <= 9) {
    mMinute = "0" + String(now.minute());
  } else {
    mMinute = String(now.minute());
  }
  if ((now.second()) <= 9) {
    mSecond = "0" + String(now.second());
  } else {
    mSecond = String(now.second());
  }
  filename = mDay + mMonth + mHour + mMinute + ".CSV";

  Serial.begin(57600);
  u8g2.begin();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // relays
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(13, HIGH);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////                  LOOP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  DateTime now = rtc.now();

  //////////////////////////////////////////////////////////////DATE STRING
  if ((now.day()) <= 9) {
    mDay = "0" + String(now.day());
  } else {
    mDay = String(now.day());
  }
  if ((now.month()) <= 9) {
    mMonth = "0" + String(now.month());
  } else {
    mMonth = String(now.month());
  }
  mYear = now.year();

  if ((now.hour()) <= 9) {
    mHour = "0" + String(now.hour());
  } else {
    mHour = String(now.hour());
  }
  if ((now.minute()) <= 9) {
    mMinute = "0" + String(now.minute());
  } else {
    mMinute = String(now.minute());
  }
  if ((now.second()) <= 9) {
    mSecond = "0" + String(now.second());
  } else {
    mSecond = String(now.second());
  }
  Serial.println(mSecond);

  ///////////////////////////////-----VOLTAGE----///////////////////////////////////////

  int volt_1 = analogRead(vol_1_pin);// read the input
  double voltage_1 = map(volt_1, 0, 1023, 0, 2500) + vol_2_offset; // map 0-1023 to 0-2500 and add correction offset
  voltage_1 /= 100; // divide by 100 to get the decimal values

  int volt_2 = analogRead(vol_2_pin);// read the input
  double voltage_2 = map(volt_2, 0, 1023, 0, 2500) + vol_2_offset; // map 0-1023 to 0-2500 and add correction offset
  voltage_2 /= 100; // divide by 100 to get the decimal values

  int volt_3 = analogRead(vol_3_pin);// read the input
  double voltage_3 = map(volt_3, 0, 1023, 0, 2500) + vol_3_offset; // map 0-1023 to 0-2500 and add correction offset
  voltage_3 /= 100; // divide by 100 to get the decimal values

  int volt_4 = analogRead(vol_4_pin);// read the input
  double voltage_4 = map(volt_4, 0, 1023, 0, 2500) + vol_4_offset; // map 0-1023 to 0-2500 and add correction offset
  voltage_4 /= 100; // divide by 100 to get the decimal values

  ///////////////////////////////-----CURRENT----///////////////////////////////////////

  float voltage_raw_1 =   (5.0 / 1023.0) * (analogRead(curr_1_pin) + curr_1_offset); // Read the voltage from sensor
  C_voltage_1 =  voltage_raw_1 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_1 = C_voltage_1 / sensitivity[model];
  current_1 = (current_1 + lastCurrent_1) / 2;
  lastCurrent_1 = current_1;

  float voltage_raw_2 =   (5.0 / 1023.0) * (analogRead(curr_2_pin) + curr_2_offset); // Read the voltage from sensor
  C_voltage_2 =  voltage_raw_2 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_2 = C_voltage_2 / sensitivity[model];
  current_2 = (current_2 + lastCurrent_2) / 2;
  lastCurrent_2 = current_2;

  float voltage_raw_3 =   (5.0 / 1023.0) * (analogRead(curr_3_pin) + curr_3_offset); // Read the voltage from sensor
  C_voltage_3 =  voltage_raw_3 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_3 = C_voltage_3 / sensitivity[model];
  current_3 = (current_3 + lastCurrent_3) / 2;
  lastCurrent_3 = current_3;

  float voltage_raw_4 =   (5.0 / 1023.0) * (analogRead(curr_4_pin) + curr_4_offset); // Read the voltage from sensor
  C_voltage_4 =  voltage_raw_4 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_4 = C_voltage_4 / sensitivity[model];
  current_4 = (current_4 + lastCurrent_4) / 2;
  lastCurrent_4 = current_4;

  //  Serial.print("---ffr  ");
  //  Serial.println(analogRead(curr_4_pin));
  //  Serial.print("---------222222  ");
  //  Serial.println(analogRead(voltage_raw_4));


  ///////////////////// switch on Lights
  //////////////////// switch off charging
  digitalWrite(12, LOW);
  digitalWrite(11, LOW);
  digitalWrite(10, LOW);
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);//charger HIGH=off

  /////////////////// switch on charging
  /////////////////// switch off Lights
  //  digitalWrite(12, HIGH);
  //  digitalWrite(11, HIGH);
  //  digitalWrite(10, HIGH);
  //  digitalWrite(9, HIGH);
  //  digitalWrite(8, LOW);//charger LOW=on

  /*/////////////////////////////////--picture loop--////////////////////////////////////////////*/
  //  u8g.firstPage();
  //  do {
  //    draw();
  //  } while ( u8g.nextPage() );
  //  if (digitalRead(btn) == LOW) {
  //    draw_state++;
  //    delay(300);
  //    if ( draw_state >= 3 ) {
  //      draw_state = 0;
  //    }
  //  }

  if (now.secondstime() > timer) {
    timer = now.secondstime();
    Serial.print("    OPLA---");
    Serial.println(mSecond);
    myFile = SD.open(filename, FILE_WRITE);
    if (myFile) {
      myFile.print(mDay);
      myFile.print(".");
      myFile.print(mMonth);
      myFile.print(".");
      myFile.print(mYear);
      myFile.print(" ");
      myFile.print(mHour);
      myFile.print(":");
      myFile.print(mMinute);
      myFile.print(":");
      myFile.print(mSecond);

      myFile.print(",");
      myFile.print(current_1);
      myFile.print(",");
      myFile.print(voltage_1);
      myFile.print(",");
      myFile.print(current_2);
      myFile.print(",");
      myFile.print(voltage_2);
      myFile.print(",");
      myFile.print(current_3);
      myFile.print(",");
      myFile.print(voltage_3);
      myFile.print(",");
      myFile.print(current_4);
      myFile.print(",");
      myFile.print(voltage_4);
      myFile.println();
      myFile.close();
      c = 1;

    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening loop txt file");
      c = 0;
    }
  }


  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_profont10_mn);
    u8g2.setCursor(col1, row1);
    u8g2.print(current_1);
    
    u8g2.setCursor(col1, row2);
    u8g2.print(voltage_1);

    u8g2.setCursor(col2, row1);
    u8g2.print(current_2);
    u8g2.setCursor(col2, row2);
    u8g2.print(voltage_2);

    u8g2.setCursor(col3, row1);
    u8g2.print(current_3);
    u8g2.setCursor(col3, row2);
    u8g2.print(voltage_3);

    u8g2.setCursor(col4, row1);
    u8g2.print(current_4);
    u8g2.setCursor(col4, row2);
    u8g2.print(voltage_4);


    u8g2.setCursor(0, 50);
    u8g2.print(c);

    u8g2.setFont(u8g2_font_6x10_tr);
    //--------------------------------------------- TIME
    u8g2.setCursor(0, 60);

    if ((now.day()) <= 9) {
      u8g2.print("0");
    }
    u8g2.print(now.day(), DEC);
    u8g2.print("/");
    if ((now.month()) <= 9) {
      u8g2.print("0");
    }
    u8g2.print(now.month(), DEC);
    u8g2.print("/");
    u8g2.print(now.year(), DEC);
    u8g2.print(" ");
    u8g2.print(now.hour(), DEC);
    u8g2.print(":");
    if ((now.minute()) <= 9) {
      u8g2.print("0");
    }
    u8g2.print(now.minute(), DEC);
    u8g2.print(":");
    if ((now.second()) <= 9) {
      u8g2.print("0");
    }
    u8g2.print(now.second(), DEC);
    u8g2.println();

  } while ( u8g2.nextPage() );

}
