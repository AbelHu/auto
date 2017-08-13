// Compare PM 2.5 detectors: A4 vs. G3
// Compare temperature sensors: DHT-22 vs. SHT-31
// Runs on the Arduino part of LinkIt Smart 7688 Duo

// IR LED     -> 7688 Duo pin D13
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

#define GREE_MODE_INIT 0
#define GREE_MODE_COOL 1
#define GREE_MODE_WARM 2

#define DEVICE_BEDROOM   0
#define DEVICE_LIVING_ROOM 1

#define CMD_COOL_OPEN  0
#define CMD_COOL_CLOSE 1
#define CMD_COOL_24    2
#define CMD_COOL_25    3
#define CMD_COOL_26    4
#define CMD_COOL_27    5
#define CMD_COOL_28    6

#define CMD_WARM_OPEN  0
#define CMD_WARM_CLOSE 1
#define CMD_WARM_23    2
#define CMD_WARM_24    3
#define CMD_WARM_25    4
#define CMD_WARM_26    5
#define CMD_WARM_27    6

const unsigned long GREE_COOL_CMD_BEDROOM[7][2] = {
  {0x9050040A, 0x4000F},  // OPEN
  {0x8050040A, 0x4000E},  // CLOSE
  {0x9010040A, 0x84000B}, // 24
  {0x9090040A, 0x40007},  // 25
  {0x9050040A, 0x4000F},  // 26
  {0x90D0040A, 0x40000},  // 27
  {0x9030040A, 0x840008}  // 28
};

const unsigned long GREE_WARM_CMD_BEDROOM[7][2] = {
  {0x3090040A, 0x40008}, // OPEN
  {0x2090040A, 0x40009}, // CLOSE
  {0x30E0040A, 0x4000F}, // 23
  {0x3010040A, 0x40000}, // 24
  {0x3090040A, 0x40008}, // 25
  {0x3050040A, 0x40004}, // 26
  {0x30D0040A, 0x4000C}  // 27
};

const unsigned long GREE_WARM_CMD_BEDROOM_WITHOUT_HEAT[7][2] = {
  {0x3090050A, 0x40008}, // OPEN
  {0x2090050A, 0x40009}, // CLOSE
  {0x30E0050A, 0x4000F}, // 23
  {0x3010050A, 0x40000}, // 24
  {0x3090050A, 0x40008}, // 25
  {0x3050050A, 0x40004}, // 26
  {0x30D0050A, 0x4000C}  // 27
};

const unsigned long GREE_COOL_CMD_LIVING_ROOM[7][2] = {
  {0x9050040A, 0x4000F}, // OPEN
  {0x8010040A, 0x4000A}, // CLOSE
  {0x9010040A, 0x4000B}, // 24
  {0x9090040A, 0x40007}, // 25
  {0x9050040A, 0x4000F}, // 26
  {0x90D0040A, 0x40000}, // 27
  {0x9030040A, 0x40008}  // 28
};

void cool(int cmd) {
  irsend.sendGREE(GREE_COOL_CMD_BEDROOM[cmd][0], GREE_COOL_CMD_BEDROOM[cmd][1]);
}

void warm(int cmd) {
  irsend.sendGREE(GREE_WARM_CMD_BEDROOM[cmd][0], GREE_WARM_CMD_BEDROOM[cmd][1]);
}

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

int count = 0;
int greeMode = GREE_MODE_INIT;

void loop() {
  unsigned long start = millis();
  float t = sht31.readTemperature();
  float h = sht31.readHumidity();
  if(!isnan(t) && !isnan(h)) {
    MYPRINT("REC: ");
    MYPRINT(t);
    MYPRINT(",");
    MYPRINTLN(h);

    handle_temperature(t);
    handle_humidity(h);
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

void handle_humidity(float h) {
    MYPRINTLN("LOG: Check humidity");
    if (h < 60.0 && hum_mode != HUM_MODE_ON) {
      MYPRINTLN("CMD: TURN_ON_SWITCH");
      hum_mode = HUM_MODE_ON;
    } else if (h > 65.0 && hum_mode != HUM_MODE_OFF) {
      MYPRINTLN("CMD: TURN_OFF_SWITCH");
      hum_mode = HUM_MODE_OFF;
    }
}

void handle_temperature(float t) {
  MYPRINTLN("LOG: Check temperature");
  if (greeMode == GREE_MODE_INIT) {
    if (t < 20.0) {
      greeMode = GREE_MODE_WARM;
      MYPRINTLN("LOG: Set mode to warm");
    } else if (t > 26.0) {
      greeMode = GREE_MODE_COOL;
      MYPRINTLN("LOG: Set mode to cool");
    }
  }

  switch(greeMode) {
    case GREE_MODE_WARM:
      change_temperature_for_warm(t);
      break;
    case GREE_MODE_COOL:
      change_temperature_for_cool(t);
      break;
    default:
      break;
  }
}

void change_temperature_for_cool(float t) {
  if (t < 26.0) {
    if (mode != MODE_UP) {
      cool(CMD_COOL_27);
      mode = MODE_UP;
      count = 0;
      MYPRINTLN("LOG: Temperature < 26.00, turn up to 27");
    } else if (mode == MODE_UP) {
      if (count < 5) {
        count++;
      } else if (count == 5) {
        cool(CMD_COOL_28);
        count = 6;
        MYPRINTLN("LOG: Temperature < 26.00 for > 5 minutes, turn up to 28");
      }
    }
  } else if (t > 26.5) {
    if (mode != MODE_DOWN) {
      cool(CMD_COOL_25);
      mode = MODE_DOWN;
      count = 0;
      MYPRINTLN("LOG: Temperature > 26.50, turn down to 25");
    } else if (mode == MODE_DOWN) {
      if (count < 5) {
        count++;
      } else if (count == 5) {
        cool(CMD_COOL_24);
        count = 6;
        MYPRINTLN("LOG: Temperature > 26.50 for > 5 minutes, turn down to 24");
      }
    }
  } else {
    switch(mode) {
      case MODE_UP:
        if (t > 26.2) {
          cool(CMD_COOL_26);
          mode = MODE_ZERO;
          count = 0;
          MYPRINTLN("LOG: Temperature is good, change to 26");
        }
        break;
      case MODE_DOWN:
        if (t < 26.3) {
          cool(CMD_COOL_26);
          mode = MODE_ZERO;
          count = 0;
          MYPRINTLN("LOG: Temperature is good, change to 26");
        }
        break;
      default:
        break;
    }
  }
}

void change_temperature_for_warm(float t) {
  if (t < 25.0) {
    if (mode != MODE_UP) {
      warm(CMD_WARM_26);
      mode = MODE_UP;
      count = 0;
      MYPRINTLN("LOG: Temperature < 25.00, turn up to 26");
    } else if (mode == MODE_UP) {
      if (count < 5) {
        count++;
      } else if (count == 5) {
        warm(CMD_WARM_27);
        count = 6;
        MYPRINTLN("LOG: Temperature < 25.00 for > 5 minutes, turn up to 27");
      }
    }
  } else if (t > 25.5) {
    if (mode != MODE_DOWN) {
      warm(CMD_WARM_24);
      mode = MODE_DOWN;
      count = 0;
      MYPRINTLN("LOG: Temperature > 25.50, turn down to 24");
    } else if (mode == MODE_DOWN) {
      if (count < 5) {
        count++;
      } else if (count == 5) {
        warm(CMD_WARM_23);
        count = 6;
        MYPRINTLN("LOG: Temperature > 25.50 for > 5 minutes, turn down to 23");
      }
    }
  } else {
    switch(mode) {
      case MODE_UP:
        if (t > 25.2) {
          warm(CMD_WARM_25);
          mode = MODE_ZERO;
          count = 0;
          MYPRINTLN("LOG: Temperature is good, change to 25");
        }
        break;
      case MODE_DOWN:
        if (t < 25.3) {
          warm(CMD_WARM_25);
          mode = MODE_ZERO;
          count = 0;
          MYPRINTLN("LOG: Temperature is good, change to 25");
        }
        break;
      default:
        break;
    }
  }
}

