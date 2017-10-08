#include <MIDI.h>
 
MIDI_CREATE_DEFAULT_INSTANCE();

bool isSendingNote = false;
const int AnalogIn  = A0;
int readingIn = 0;

void setup() 
{
  
  pinMode(13, OUTPUT);
  MIDI.begin(4);                      // Launch MIDI and listen to channel 4
  //Serial.begin(57600);
  Serial.begin(115200);
} 
 
void loop() 
{
  readingIn = analogRead(AnalogIn);

  // map(value, fromLow, fromHigh, toLow, toHigh)
  int pos = map(readingIn, 0, 1000, 40, 70);

  if( ( pos > 40 ) && (isSendingNote == false) ){
    isSendingNote = true;
    Serial.println("We send some MIDI stuff");
    
    digitalWrite(13, HIGH);
    MIDI.sendNoteOn(pos, 127, 1);    // Send a Note (pitch 42, velo 127 on channel 1)
    delay(250);                // Wait for a second
    MIDI.sendNoteOff(pos, 0, 1);     // Stop the note
    digitalWrite(13, LOW);
    Serial.println("ENd stuff");
    isSendingNote = false;
  }

  delay(15);
} 

