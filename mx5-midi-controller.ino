// Controller for MX5 multi-effects

#include <MIDI.h>

// Create and bind the MIDI interface to the default hardware Serial port
MIDI_CREATE_DEFAULT_INSTANCE();

// MIDI CC codes corresponding to the various functions
#define RECORD_OVERDUB 70
#define START_STOP 69
#define PEEL 72
#define MUTE 73

#define NUM_BUTTONS 4
#define DEBOUNCE_DELAY 50 // milliseconds. Need to debounce button presses or we get a stream 
                          // of unwanted MIDI messages.

struct Button {
  int pin;                 // pin to which the button is attached
  int midi_cc;             // MIDI Control Change code to send when button is pressed
  bool pushed;             // State of button
  unsigned long last_push; // Time when the last push was registered
};

Button buttons[NUM_BUTTONS] = {
  {10, RECORD_OVERDUB, false, 0},  // bottom-left button
  {5,  START_STOP, false, 0},      // bottom-right button
  {12, PEEL, false, 0},            // top-left button
  {3,  MUTE, false, 0}             // top-right button
};

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  
  // Initialise all the pins
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  // for debugging
  //pinMode(13, OUTPUT);
}

void loop() {

  // Check the state of each button
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    Button* b = &buttons[i]; // Don't be tempted to do "Button b = buttons[i]"!
                             // This seems to create a copy of the struct so it
                             // doesn't update the right members.
                             
    if (digitalRead(b->pin) == LOW)
    {
      // The button is being pushed. Record the time.
      unsigned long push_time = millis();
      // Check to see if this is the first time we've seen the button pushed since the last release,
      // and that enough time has passed since the last push 
      if (!b->pushed && (push_time - b->last_push) > DEBOUNCE_DELAY) {
        // Need to send CC ON immediately followed by OFF. The looper will only 
        // start when it receives the OFF.
        MIDI.sendControlChange(b->midi_cc, 127, 1);
        MIDI.sendControlChange(b->midi_cc, 0, 1);
        //digitalWrite(13,HIGH);
      }
      b->pushed = true;
      b->last_push = push_time;
    }
    else
    {
      // button has been released
      b->pushed = false;
      //digitalWrite(13,LOW);
    }
  }
}
