#include <SPI.h>
#include <SD.h>
#include <RTClib.h>
//#include "U8glib.h"
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI
//#include <Arduino.h>
//#include <U8x8lib.h>
//U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

int row1 = 20;
int row2 = 40;
int col1 = 0;
int col2 = 30;
int col3 = 60;
int col4 = 90;

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

int vol_1_offset = 5;
int vol_2_offset = 5;
int vol_3_offset = 5;
int vol_4_offset = 5;

int offset = 5;

//int voltagePin = A2;

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

void setup() {
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));


  u8g2.begin();

  //  if (u8g.getMode() == U8G_MODE_R3G3B2) {
  //    u8g.setColorIndex(255);     // white
  //  }
  //  else if (u8g.getMode() == U8G_MODE_GRAY2BIT) {
  //    u8g.setColorIndex(3);         // max intensity
  //  }
  //  else if (u8g.getMode() == U8G_MODE_BW) {
  //    u8g.setColorIndex(1);         // pixel on
  //  }
  //  else if (u8g.getMode() == U8G_MODE_HICOLOR) {
  //    u8g.setHiColorByRGB(255, 255, 255);
  //  }

  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }

  //  // re-open the file for reading:
  //  myFile = SD.open("test.txt");
  //  if (myFile) {
  //    Serial.println("test.txt:");
  //
  //    // read from the file until there's nothing else in it:
  //    while (myFile.available()) {
  //      Serial.write(myFile.read());
  //    }
  //    // close the file:
  //    myFile.close();
  //  } else {
  //    // if the file didn't open, print an error:
  //    Serial.println("error opening test.txt");
  //  }







  // initialize digital pin LED_BUILTIN as an output.
  pinMode(13, OUTPUT);

  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);

  pinMode(8, OUTPUT);

  digitalWrite(13, HIGH);
}

//String ts;
//void draw(void) {
//  DateTime now = RTC.now();
//  // graphic commands to redraw the complete screen should be placed here
//  u8g.setFont(u8g_font_unifont);
//  //u8g.setFont(u8g_font_osb21);
//  u8g.print(now.minute());
//}



//void desk1(void) {
//  DateTime now = rtc.now();
//
//  /*///////////////////////////////--Dispaly Time--//////////////////////////////////////*/
//  //  u8g.setFont(u8g_font_fur14);
//  //  u8g.setPrintPos(0, 35);
//  //  u8g.print(now.hour(), DEC);
//  //  u8g.print(":");
//  //  if ((now.minute()) <= 9) {
//  //    u8g.print("0");
//  //    u8g.print(now.minute(), DEC);
//  //  }
//  //  else {
//  //    u8g.print(now.minute(), DEC);
//  //  }
//  //  u8g.setFont(u8g_font_7x13B);
//  //  u8g.setPrintPos(87, 30);
//  //  u8g.print("rmp3");
//
//  /*///////////////////////////////--Dispaly Date--//////////////////////////////////////*/
//
//  u8g.setFont(u8g_font_7x13B);
//  u8g.setPrintPos(0, 60);
//  u8g.print(now.day(), DEC);
//  u8g.print("/");
//  u8g.print(now.month(), DEC);
//  u8g.print("/");
//  u8g.print(now.year(), DEC);
//  u8g.print(" ");
//  u8g.print(now.hour(), DEC);
//  u8g.print(":");
//  u8g.print(now.minute(), DEC);
//  u8g.print(":");
//  u8g.print(now.second(), DEC);
//}
//
//void draw(void) {
//
//  desk1();
//
//  //  switch (draw_state >> 0) {
//  //    case 0: desk1(); break;
//  //      //case 1: desk2(); break;
//  //      //case 2: desk3(); break;
//  //  }
//}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.day(), DEC);
  Serial.print("/");
  Serial.print(now.month(), DEC);
  Serial.print("/");
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(":");
  Serial.print(now.minute(), DEC);
  Serial.print(":");
  Serial.print(now.second(), DEC);
  Serial.println();

  ///////////////////////////////-----VOLTAGE----///////////////////////////////////////

  //  int volt_1 = analogRead(vol_1_pin);// read the input
  //  double voltage_1 = map(volt_1, 0, 1023, 0, 2500) + vol_1_offset; // map 0-1023 to 0-2500 and add correction offset
  //  voltage_1 /= 100; // divide by 100 to get the decimal values
  //


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

  float voltage_raw_1 =   (5.0 / 1023.0) * analogRead(curr_1_pin); // Read the voltage from sensor
  C_voltage_1 =  voltage_raw_1 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_1 = C_voltage_1 / sensitivity[model];
  current_1 = (current_1 + lastCurrent_1) / 2;
  lastCurrent_1 = current_1;

  float voltage_raw_2 =   (5.0 / 1023.0) * analogRead(curr_2_pin); // Read the voltage from sensor
  C_voltage_2 =  voltage_raw_2 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_2 = C_voltage_2 / sensitivity[model];
  current_2 = (current_2 + lastCurrent_2) / 2;
  lastCurrent_2 = current_2;

  float voltage_raw_3 =   (5.0 / 1023.0) * analogRead(curr_3_pin); // Read the voltage from sensor
  C_voltage_3 =  voltage_raw_3 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_3 = C_voltage_3 / sensitivity[model];
  current_3 = (current_3 + lastCurrent_3) / 2;
  lastCurrent_3 = current_3;

  float voltage_raw_4 =   (5.0 / 1023.0) * analogRead(curr_4_pin); // Read the voltage from sensor
  C_voltage_4 =  voltage_raw_4 - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_4 = C_voltage_4 / sensitivity[model];
  current_4 = (current_4 + lastCurrent_4) / 2;
  lastCurrent_4 = current_4;


  // switch on Lights
  // switch off charging
  //    myFile = SD.open("dis22.txt", FILE_WRITE);
  //    digitalWrite(12, LOW);
  //    digitalWrite(11, LOW);
  //    digitalWrite(10, LOW);
  //    digitalWrite(9, LOW);
  //digitalWrite(8, HIGH);//charger HIGH=off
  //


  // switch on charging
  // switch off Lights
  myFile = SD.open("charge_5.txt", FILE_WRITE);
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(8, LOW);//charger LOW=on



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
    //  int volt = analogRead(A5);// read the input
    //  double voltage = map(volt, 0, 1023, 0, 2500) + offset; // map 0-1023 to 0-2500 and add correction offset
    //
    //  voltage /= 100; // divide by 100 to get the decimal values
    //  Serial.print("Voltage: ");
    //  Serial.print(voltage);//print the voltge
    //  Serial.println("V");







    if (myFile) {



      if ((now.day()) <= 9) {
        myFile.print("0");
      }
      myFile.print(now.day(), DEC);
      myFile.print(".");
      if ((now.month()) <= 9) {
        myFile.print("0");
      }
      myFile.print(now.month(), DEC);
      myFile.print(".");
      myFile.print(now.year(), DEC);
      myFile.print(" ");
      if ((now.hour()) <= 9) {
        myFile.print("0");
      }
      myFile.print(now.hour(), DEC);
      myFile.print(":");
      if ((now.minute()) <= 9) {
        myFile.print("0");
      }
      myFile.print(now.minute(), DEC);
      myFile.print(":");
      if ((now.second()) <= 9) {
        u8g2.print("0");
      }
      myFile.print(now.second(), DEC);




      myFile.print(",");
      myFile.print(current_1);
      Serial.print(",");
      Serial.print(current_1);
      myFile.print(",");
      myFile.print(current_2);
      Serial.print(",");
      Serial.print(current_2);
      myFile.print(",");
      myFile.print(current_3);
      Serial.print(",");
      Serial.print(current_3);
      myFile.print(",");
      myFile.print(current_4);
      Serial.print(",");
      Serial.print(current_4);
      myFile.print(",");
      myFile.print(voltage_1);
      Serial.print(",");
      Serial.print(voltage_1);
      myFile.print(",");
      myFile.print(voltage_2);
      Serial.print(",");
      Serial.print(voltage_2);
      myFile.print(",");
      myFile.print(voltage_3);
      Serial.print(",");
      Serial.print(voltage_3);
      myFile.print(",");
      myFile.print(voltage_4);
      Serial.print(",");
      Serial.print(voltage_4);
      Serial.println();
      myFile.println();
      myFile.close();



    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening loop txt file");
    }

  }



  u8g2.setFont(u8g2_font_tenthinnerguys_tu );
  u8g2.firstPage();
  do {
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

    u8g2.setFont(u8g2_font_crox1hb_tf );
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
