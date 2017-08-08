/*
 * IRremote: IRsendRawDemo - demonstrates sending IR codes with sendRaw
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 *
 * IRsendRawDemo - added by AnalysIR (via www.AnalysIR.com), 24 August 2015
 *
 * This example shows how to send a RAW signal using the IRremote library.
 * The example signal is actually a 32 bit NEC signal.
 * Remote Control button: LGTV Power On/Off. 
 * Hex Value: 0x20DF10EF, 32 bits
 * 
 * It is more efficient to use the sendNEC function to send NEC signals. 
 * Use of sendRaw here, serves only as an example of using the function.
 * 
 */

#include <IRremote.h>

IRsend irsend;

void setup()
{
  Serial1.begin(57600);
  Serial.begin(9600);
  //delay(2000); while(!Serial);
  Serial.print("PIN to send IR: ");
  Serial.println(TIMER_PWM_PIN);
}

void loop() {
  int khz = 38; // 38kHz carrier frequency for the NEC protocol

  /*
  // XiaoMi
  unsigned int xiaomi[143] = {1050,600, 550,600, 600,1400, 600,1400, 600,550, 650,1350, 650,550, 600,1400, 650,500, 650,1400, 600,1400, 600,10700, 1050,550, 600,550, 600,1400, 650,1400, 600,550, 600,1400, 650,550, 600,1400, 600,550, 650,1350, 650,1400, 600,10700, 1050,550, 600,550, 600,1400, 650,1350, 650,550, 600,1400, 650,500, 650,1400, 600,550, 600,1400, 650,1400, 600,10700, 1050,550, 600,550, 600,1400, 650,1400, 600,550, 600,1400, 650,550, 600,1400, 600,600, 550,1400, 650,1400, 600,10700, 1050,550, 600,600, 550,1400, 650,1350, 650,550, 600,1400, 600,600, 600,1400, 600,550, 600,1400, 650,1400, 600,10700, 1000,600, 600,550, 600,1400, 600,1400, 650,550, 600,1400, 600,550, 650,1400, 600,550, 600,1400, 650,1350, 650};  // XiaoMi
  int i = Serial1.read();
  if (i > -1) {
    irsend.sendRaw(xiaomi, sizeof(xiaomi) / sizeof(xiaomi[0]), khz);
    Serial.println("Sent to xiaomi");
  }
  */

  // Gree
  int i = Serial1.read();
  if (i > -1) {
    switch(i) {
    case 'o':
      irsend.sendGREE(0x9050040A, 0x4000F);
      Serial.println("Sent open");
      break;
    case '5':
      irsend.sendGREE(0x9090040A, 0x40007);
      Serial.println("Sent 25");
      break;
    case '6':
      irsend.sendGREE(0x9050040A, 0x4000F);
      Serial.println("Sent 26");
      break;
    case '7':
      irsend.sendGREE(0x90D0040A, 0x40000);
      Serial.println("Sent 27");
      break;
    case 'c':
      irsend.sendGREE(0x8050040A, 0x4000E);
      Serial.println("Send close");
      break;
    default:
      Serial.print(i);
      Serial.println(" - invalid");
      break;
    }
  }

  /*
  // LED
  unsigned int  openCmd[67] = {9050,4450, 550,1700, 550,550, 550,1700, 550,550, 550,550, 550,550, 550,1700, 500,1700, 550,550, 550,550, 600,550, 550,550, 550,550, 550,1700, 500,1700, 550,550, 550,550, 550,600, 550,550, 550,1650, 550,1700, 550,1650, 550,1700, 550,1650, 550,1700, 550,1650, 550,1700, 550,550, 550,550, 550,600, 500,600, 550,550, 550};  // NEC A3061FE0
  unsigned int  closeCmd[67] = {9000,4500, 550,1700, 500,600, 550,1650, 550,600, 500,600, 550,550, 550,1650, 550,1700, 550,550, 550,550, 550,550, 550,550, 550,600, 550,1650, 550,1700, 550,550, 550,550, 550,1700, 550,550, 550,550, 550,1700, 550,1650, 550,1700, 550,1650, 550,1700, 500,600, 550,1650, 550,1700, 550,550, 550,550, 550,550, 550,600, 500};  // NEC A3064FB0 
  int i = Serial1.read();
  if (i > -1) {
    switch(i) {
      case 'o':
      irsend.sendRaw(openCmd, sizeof(openCmd) / sizeof(openCmd[0]), khz);
      Serial.println("Sent open");
      break;
    default:
      irsend.sendRaw(closeCmd, sizeof(closeCmd) / sizeof(closeCmd[0]), khz);
      Serial.println("Sent close");
      break;
    }
  }
  */
}
