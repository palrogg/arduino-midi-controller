#define DEBUG false

#include <MIDI.h>

// fix for CC -- see here: https://github.com/projectgus/hairless-midiserial/issues/16
struct HairlessMidiSettings : public midi::DefaultSettings
{
  static const bool UseRunningStatus = false;
  //static const long BaudRate = 31250;
  static const long BaudRate = 57600; // midi = 31250
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessMidiSettings);

bool isSendingNote = false;
const int AnalogIn  = A0;
const int AnalogIn2 = A1;
int readingIn = 0;
int readingIn2 = 0;
int lastPos;
int lastPos2;
int buttonCounter = 0;

// 6 booleans indicating if each button is currently pressed/touched
bool buttonsPressed[6];

// 4 ints containing the slider values
int analogValues[4];
const int analogInList[] = {A0, A1, A2, A3};
const int controlChanges[] = {92, 93, 94, 95};

inline void debugPrintln(char *debugMessage) {
  if (DEBUG) {
    Serial.println(debugMessage);
  }
}
inline void debugPrint(char *debugMessage) {
  if (DEBUG) {
    Serial.print(debugMessage);
  }
}
void setup()
{
  // touch
  pinMode(3, INPUT); // A, B, C, D = upper left, lower left, etc.
  pinMode(5, INPUT);
  pinMode(8, INPUT);
  pinMode(13, INPUT);

  // buttons
  pinMode(11, INPUT); // lower button
  pinMode(12, INPUT); // upper button

  MIDI.begin(4); // Launch MIDI and listen to channel 4
}

void sendNote(int key) {
  MIDI.sendNoteOn(key, 120, 1);
  delay(50);
  MIDI.sendNoteOff(key, 120, 1);
  delay(50);
}

void loop()
{
  // 1 - get touch events
  if (digitalRead(3) == HIGH) {
    if (!buttonsPressed[0]) {
      debugPrint("A touched!");
      sendNote(40);
      buttonsPressed[0] = true;
    }
  } else if (buttonsPressed[0]) {
    buttonsPressed[0] = false;
  }
  if (digitalRead(5) == HIGH) {
    if (!buttonsPressed[1]) {
      debugPrint("B touched!");
      sendNote(36);
      buttonsPressed[1] = true;
    }
  } else if (buttonsPressed[1]) {
    buttonsPressed[1] = false;
  }
  if (digitalRead(8) == HIGH) {
    if (!buttonsPressed[2]) {
      debugPrint("c touched!");
      sendNote(42);
      buttonsPressed[2] = true;
    }
  } else if (buttonsPressed[2]) {
    buttonsPressed[2] = false;
  }
  if (digitalRead(13) == HIGH) {
    if (!buttonsPressed[3]) {
      debugPrint("D touched!");
      sendNote(38);
      buttonsPressed[3] = true;
    }
  } else if (buttonsPressed[3]) {
    buttonsPressed[3] = false;
  }


  if (digitalRead(11) == HIGH) { // enfoncé
    if (!buttonsPressed[4]) {
      // 0x02 = play, 06 = record, 07 = Record Exit
      byte command;
      if (buttonCounter == 0) {
        command = 0x06;
        buttonCounter += 1;
      } else if (buttonCounter == 1) {
        command = 0x01;
        buttonCounter = 0;
        //buttonCounter += 1;
      }
      byte Sysexvalue[] = {0xF0, 0x7F, 0x7F, 0x06, command, 0xF7};

      MIDI.sendSysEx(6, Sysexvalue, true);
      delay(100);

      buttonsPressed[4] = true;
    }
  } else if (buttonsPressed[4]) {
    buttonsPressed[4] = false;
  }

  if (digitalRead(12) == HIGH) { // enfoncé
    if (!buttonsPressed[5]) {
      // stop MIDI command
      byte Sysexvalue[] = {0xF0, 0x7F, 0x7F, 0x06, 0x01, 0xF7};
      MIDI.sendSysEx(6, Sysexvalue, true);
      buttonsPressed[5] = true;
    }
  } else if (buttonsPressed[5]) {
    buttonsPressed[5] = false;
  }

  for ( unsigned int i = 0; i < 4; i++)
  {
    int currentPos = map(analogRead(analogInList[i]), 654, 0, 0, 127);
    if ( currentPos != analogValues[i] ) {
      debugPrint("New slider value! ");
      //Serial.println(analogRead(analogInList[i]));
      //Serial.println(currentPos);
      MIDI.sendControlChange(controlChanges[i], currentPos, 1);
      analogValues[i] = currentPos;
    }

  }
/*

    if (pos != lastPos) {
      Serial.print("New slider value! ");


        inControlNumber The controller number (0 to 127), 1 = modulation wheel
        inControlValue  The value for the specified controller (0 to 127).
        inChannel The channel on which the message will be sent (1 to 16).
  */
  // 95 = phaser
  // tester aussi 71
  // -> keep thisMIDI.sendControlChange(95, pos, 1);
  /*MIDI.sendNoteOn(pos, 20, 1);
    delay(500);
    MIDI.sendNoteOff(pos, 20, 1);*/
  /*
      lastPos = pos;
    }

    if (pos2 != lastPos2) {
      Serial.print("New slider value for slider 2! ");
      debugPrint(pos2);
      lastPos2 = pos2;
    }
  */

  // 2 - send MIDI data


  delay(50); // to prevent Hairless from crashing
}

