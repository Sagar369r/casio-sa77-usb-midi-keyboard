#include "MIDIUSB.h"

const int numRows = 6;
int rowPins[numRows] = {2, 3, 4, 5, 6, 7}; 

const int numCols = 8;
int colPins[numCols] = {10, 14, 15, 16, 18, 19, 20, 21}; 

// The exact key numbers (1-44) mapped to your Row/Col intersections
// 0 means it is one of the 4 intentionally blank spots on the Casio PCB
const int keyMap[6][8] = {
  // Cols: 10, 14, 15, 16, 18, 19, 20, 21
  {  0,  0,  0,  0, 24, 44, 25, 26 }, // Row 2
  { 22, 23, 43, 42, 40, 20, 41, 21 }, // Row 3
  { 17, 18, 19, 39, 15, 37, 16, 38 }, // Row 4
  { 35, 36, 14, 13, 10, 34, 11, 12 }, // Row 5
  {  8,  9, 33, 32, 30,  6, 31,  7 }, // Row 6
  {  3,  4,  5, 29,  1, 27,  2, 28 }  // Row 7
};

// Lookup table to convert your Key Numbers (1-44) into standard MIDI Pitches
// Index 0 is ignored. Indices 1-26 are White Keys. Indices 27-44 are Black Keys.
// Starts at F2 (MIDI 41) and ends at C6 (MIDI 84)
const byte midiPitch[45] = {
  0, 
  41, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, // White Keys
  42, 44, 46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70, 73, 75, 78, 80, 82  // Black Keys
};

// State tracking for polyphony (chords)
bool keyState[numRows][numCols];

void setup() {
  for(int c = 0; c < numCols; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  for(int r = 0; r < numRows; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH); 
  }
  for(int r = 0; r < numRows; r++) {
    for(int c = 0; c < numCols; c++) {
      keyState[r][c] = false;
    }
  }
}

void loop() {
  for(int r = 0; r < numRows; r++) {
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(20); // Zero-latency stabilization

    for(int c = 0; c < numCols; c++) {
      int keyNum = keyMap[r][c];
      
      // Skip empty matrix intersections
      if (keyNum == 0) continue; 

      bool isPressed = (digitalRead(colPins[c]) == LOW);

      // Note On Trigger
      if(isPressed && !keyState[r][c]) {
        keyState[r][c] = true;
        noteOn(0, midiPitch[keyNum], 127);
        MidiUSB.flush();
      }
      
      // Note Off Trigger
      else if(!isPressed && keyState[r][c]) {
        keyState[r][c] = false;
        noteOff(0, midiPitch[keyNum], 0);
        MidiUSB.flush();
      }
    }
    digitalWrite(rowPins[r], HIGH);
  }
}

// MIDIUSB wrapper functions
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, (byte)(0x90 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, (byte)(0x80 | channel), pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
