// Compare PM 2.5 detectors: A4 vs. G3
// Compare temperature sensors: DHT-22 vs. SHT-31
// Runs on the Arduino part of LinkIt Smart 7688 Duo

// A4 pin 1   -> +5V
// A4 pin 2   -> GND
// A4 pin 5   -> 7688 Duo pin D11
// G3 pin 5   -> 7688 Duo pin D9
// SHT-31 SDA -> 7688 Duo pin D2
// SHT-31 SCL -> 7688 Duo pin D3
// DHT-22 DAT -> 7688 Duo pin D4

#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <IRremote.h>

# define MODE_ZERO 0
# define MODE_UP   1
# define MODE_DOWN 2

int mode = MODE_ZERO;

# define HUM_MODE_ZERO 0
# define HUM_MODE_ON   1
# define HUM_MODE_OFF  2

int hum_mode = HUM_MODE_ZERO;

IRsend irsend;

// #define SERIAL_DEBUG  true  // Comment to disable it
Adafruit_SHT31 sht31 = Adafruit_SHT31();

# define MYPRINT(...)    ({ \
  Serial.print(__VA_ARGS__);  \
  Serial1.print(__VA_ARGS__); \
})
# define MYPRINTLN(...)  ({ \
  Serial.println(__VA_ARGS__);  \
  Serial1.println(__VA_ARGS__); \
  delay(1000); \
})

const unsigned long GREE_OPEN[2] = {0x9050040A, 0x4000F};
const unsigned long GREE_CLOSE[2] = {0x8050040A, 0x4000E};
const unsigned long GREE_25[2] = {0x9090040A, 0x40007};
const unsigned long GREE_26[2] = {0x9050040A, 0x4000F};
const unsigned long GREE_27[2] = {0x90D0040A, 0x40000};

void setup() {
  Serial1.begin(57600);
  Serial.begin(9600);

  delay(60000);
  if (!sht31.begin(0x44)) {
    MYPRINTLN("LOG: Couldn't find SHT31");
    while(1) {
      MYPRINTLN("LOG: Fail forever");
      delay(1000);
    }
  }
  MYPRINTLN("LOG: Found SHT31");
}

void loop() {
  unsigned long start = millis();
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  if(!isnan(t) && !isnan(h)) {
    MYPRINT("REC: ");
    MYPRINT(t);
    MYPRINT(",");
    MYPRINTLN(h);

    MYPRINTLN("LOG: Check temperature");
    if (t < 26.0) {
      if (mode != MODE_UP) {
        irsend.sendGREE(GREE_27[0], GREE_27[1]);
        MYPRINTLN("LOG: Temperature < 26.00, turn up to 27");
        mode = MODE_UP;
      }
    } else if (t > 26.5) {
      if (mode != MODE_DOWN) {
        irsend.sendGREE(GREE_25[0], GREE_25[1]);
        MYPRINTLN("LOG: Temperature > 26.50, turn down to 25");
        mode = MODE_DOWN;
      }
    } else {
      if (mode != MODE_ZERO) {
        mode = MODE_ZERO;
        irsend.sendGREE(GREE_26[0], GREE_26[1]);
        MYPRINTLN("LOG: Temperature is good, change to 26");
      }
    }

    MYPRINTLN("LOG: Check humidity");
    if (h < 60.0) {
      if (hum_mode != HUM_MODE_ON) {
        MYPRINTLN("CMD: TURN_ON_SWITCH");
        hum_mode = HUM_MODE_ON;
      }
    } else if (h > 65.0) {
      if (hum_mode != HUM_MODE_OFF) {
        MYPRINTLN("CMD: TURN_OFF_SWITCH");
        hum_mode = HUM_MODE_OFF;
      }
    } else {
      MYPRINTLN("LOG: Do not operate switch");
    }
  } else {
    MYPRINTLN("LOG: SHT31 - Cannot get temperature...");
  }

  unsigned long duration = 0;
  if ((start + 60000) < start) { // Check if overflow
    unsigned long MAX = -1;
    duration = MAX - start;
    delay(duration);
  }
  delay(60000 - duration);
}

