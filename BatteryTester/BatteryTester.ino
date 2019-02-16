#include <SPI.h>
#include <SD.h>
#include <RTClib.h>

int chargeCheck_1 = 0;
bool cell_1_full = false;
int chargeCheck_2 = 0;
bool cell_2_full = false;
int chargeCheck_3 = 0;
bool cell_3_full = false;
int chargeCheck_4 = 0;
bool cell_4_full = false;



bool cell_1_empty = false;
bool cell_2_empty = false;
bool cell_3_empty = false;
bool cell_4_empty = false;

int cycle = 0;
int mainCycle = 1;

int row1 = 8;
int row2 = 20;
int row3 = 32;
int row4 = 48;

int col1 = 0;
int col2 = 35;
int col3 = 70;
int col4 = 105;

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
static uint32_t timer2;

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

float curr_1_offset = -1.8;
float curr_2_offset = 1.5;
float curr_3_offset = 0.8;
float curr_4_offset = 0.5;

const float VCC   = 5.0;// supply voltage is from 4.5 to 5.5V. Normally 5V.
const int model = 0;   // enter the model number (see below)
const float QOV =   0.5 * VCC;// set quiescent Output voltage of 0.5V

//float C_voltage_1;// internal variable for voltage
//float current_1;
//float prev_1_Current_1;
//float prev_2_Current_1;
//float prev_3_Current_1;
//float prev_4_Current_1;
//float current_1_print;
//float current_1_avg;

float current_1_print;

//float C_voltage_2;// internal variable for voltage
//float voltage_raw_2;
//float current_2_avg;
//float current_2;
//float prev_1_Current_2;
//float prev_2_Current_2;
//float prev_3_Current_2;
//float prev_4_Current_2;
//float lastCurrent_2;

float current_2_print;

//float C_voltage_3;// internal variable for voltage
//float current_3;
//float prev_1_Current_3;
//float prev_2_Current_3;
//float prev_3_Current_3;
//float lastCurrent_3;

float current_3_print;

//float C_voltage_4;// internal variable for voltage
//float current_4;
//float prev_1_Current_4;
//float prev_2_Current_4;
//float prev_3_Current_4;
//float lastCurrent_4;

float current_4_print;

float sensitivity[] = {
  0.185,// for ACS712ELCTR-05B-T
  0.100,// for ACS712ELCTR-20A-T
  0.066// for ACS712ELCTR-30A-T
};
RTC_Millis rtc;

/////

float voltage_1 ;
float voltage_2 ;
float voltage_3 ;
float voltage_4 ;



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


float array_1[4];
float array_2[4];
float array_3[4];
float array_4[4];

float array_V_1[4];
float array_V_2[4];
float array_V_3[4];
float array_V_4[4];

float getCurrent(int curr_pin, float curr_offset, float mArray[]) {

  float voltage_raw =   (5.0 / 1023.0) * (analogRead(curr_pin) + curr_offset); // Read the voltage from sensor
  float C_voltage =  voltage_raw - QOV + 0.012 ;// 0.000 is a value to make voltage zero when there is no current
  float current_ = C_voltage / sensitivity[model];

  mArray[3] = mArray[2];
  mArray[2] = mArray[1];
  mArray[1] = mArray[0];
  mArray[0] = current_;

  float current_avg = ((mArray[0] + mArray[1] + mArray[2] + mArray[3]) / 4);
  float current_print;
  if (abs(current_avg) < 0.4) {
    current_print = 0;
  } else {
    current_print = current_avg;
  }
  return current_print;

}
float getVoltage(int vol_pin, int vol_offset, float mArray[]) {
  int volt_ = analogRead(vol_pin);// read the input
  double voltage_ = map(volt_, 0, 1023, 0, 2500) + vol_offset; // map 0-1023 to 0-2500 and add correction offset
  voltage_ /= 100; // divide by 100 to get the decimal values

  mArray[3] = mArray[2];
  mArray[2] = mArray[1];
  mArray[1] = mArray[0];
  mArray[0] = voltage_;

  float voltage_avg = ((mArray[0] + mArray[1] + mArray[2] + mArray[3]) / 4);

  float voltage_print;
  if (abs(voltage_avg) < 0.1) {
    voltage_print = 0;
  } else {
    voltage_print = voltage_avg;
  }
  return voltage_print;
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

  voltage_1 = getVoltage(vol_1_pin, vol_1_offset, array_V_1);
  voltage_2 = getVoltage(vol_2_pin, vol_2_offset, array_V_2);
  voltage_3 = getVoltage(vol_3_pin, vol_3_offset, array_V_3);
  voltage_4 = getVoltage(vol_4_pin, vol_4_offset, array_V_4);

  ///////////////////////////////-----CURRENT----///////////////////////////////////////

  current_1_print = getCurrent(curr_1_pin, curr_1_offset, array_1);
  current_2_print = getCurrent(curr_2_pin, curr_2_offset, array_2);
  current_3_print = getCurrent(curr_3_pin, curr_3_offset, array_3);
  current_4_print = getCurrent(curr_4_pin, curr_4_offset, array_4);

  if (cycle == 0) {
    cycle ++;
  }

  if (cycle == 1) {
    digitalWrite(8, LOW);//set charger ON

    digitalWrite(12, HIGH);//set bulb 1 OFF
    digitalWrite(11, HIGH);//set bulb 2 OFF
    digitalWrite(10, HIGH);//set bulb 3 OFF
    digitalWrite(9, HIGH);//set bulb 4 OFF
  }

  if (cell_1_full == true && cell_2_full == true && cell_3_full == true && cell_4_full == true) {
    cycle ++;
  }

  if (cycle == 2) {
    digitalWrite(8, HIGH);//set charger OFF

    digitalWrite(12, LOW);//set bulb 1 ON
    digitalWrite(11, LOW);//set bulb 2 ON
    digitalWrite(10, LOW);//set bulb 3 ON
    digitalWrite(9, LOW);//set bulb 4 ON
  }

  if (cell_1_empty == true && cell_2_empty == true && cell_3_empty == true && cell_4_empty == true) {
    cycle = 0;
    mainCycle++;
  }
  
  if (now.secondstime() > timer) {
    timer = now.secondstime();
    Serial.print("    OPLA---");
    Serial.println(mSecond);

    //////////////////////////////// -- FULL CHAGRE CHECK
    if (cycle == 1 && cell_1_full == false && current_1_print == 0) {
      chargeCheck_1++;
    } else {
      chargeCheck_1 = 0;
    }
    if (chargeCheck_1 > 20) {
      cell_1_full = true;
    }
    //////////////////////////////// --
    if (cycle == 1 && cell_2_full == false && current_2_print == 0) {
      chargeCheck_2++;
    } else {
      chargeCheck_2 = 0;
    }
    if (chargeCheck_2 > 20) {
      cell_2_full = true;
    }
    //////////////////////////////// --
    if (cycle == 1 && cell_3_full == false && current_3_print == 0) {
      chargeCheck_3++;
    } else {
      chargeCheck_3 = 0;
    }
    if (chargeCheck_3 > 20) {
      cell_3_full = true;
    }
    //////////////////////////////// --
    if (cycle == 1 && cell_4_full == false && current_4_print == 0) {
      chargeCheck_4++;
    } else {
      chargeCheck_4 = 0;
    }
    if (chargeCheck_4 > 20) {
      cell_4_full = true;
    }



    //////////////////////////////// -- EMPTY CHECK
    if (cycle == 2 && cell_1_empty == false && voltage_1 < 2.5) {
      cell_1_empty = true;
    }
    if (cycle == 2 && cell_2_empty == false && voltage_2 < 2.5) {
      cell_2_empty = true;
    }
    if (cycle == 2 && cell_3_empty == false && voltage_3 < 2.5) {
      cell_3_empty = true;
    }
    if (cycle == 2 && cell_4_empty == false && voltage_4 < 2.5) {
      cell_4_empty = true;
    }



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
      myFile.print(current_1_print);
      myFile.print(",");
      myFile.print(current_2_print);
      myFile.print(",");
      myFile.print(current_3_print);
      myFile.print(",");
      myFile.print(current_4_print);
      myFile.print(",");
      myFile.print(voltage_1);
      myFile.print(",");
      myFile.print(voltage_2);
      myFile.print(",");
      myFile.print(voltage_3);
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
    u8g2.setFont(u8g2_font_profont10_mr );
    u8g2.setCursor(col1, row1);
    //Serial.println(current_1_print);
    u8g2.print(current_1_print);

    u8g2.setCursor(col1, row2);
    u8g2.print(voltage_1);

    u8g2.setCursor(col2, row1);
    u8g2.print(current_2_print);
    u8g2.setCursor(col2, row2);
    u8g2.print(voltage_2);

    u8g2.setCursor(col3, row1);
    u8g2.print(current_3_print);
    u8g2.setCursor(col3, row2);
    u8g2.print(voltage_3);

    u8g2.setCursor(col4, row1);
    u8g2.print(current_4_print);
    u8g2.setCursor(col4, row2);
    u8g2.print(voltage_4);



    u8g2.setCursor(col1, row3);
    u8g2.print(cell_1_full);

    u8g2.setCursor(col2, row3);
    u8g2.print(cell_2_full);

    u8g2.setCursor(col3, row3);
    u8g2.print(cell_3_full);

    u8g2.setCursor(col4, row3);
    u8g2.print(cell_4_full);


    u8g2.setCursor(col1 + 15, row3);
    u8g2.print(cell_1_empty);

    u8g2.setCursor(col2 + 15, row3);
    u8g2.print(cell_2_empty);

    u8g2.setCursor(col3 + 15, row3);
    u8g2.print(cell_3_empty);

    u8g2.setCursor(col4 + 15, row3);
    u8g2.print(cell_4_empty);


    u8g2.setCursor(0, row4);
    u8g2.print("c:");
    u8g2.print(cycle);

    u8g2.print(" mc:");
    u8g2.print(mainCycle);


    u8g2.setFont(u8g2_font_profont10_mn);
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
