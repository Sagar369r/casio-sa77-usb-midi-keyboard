const int numRows = 6;
int rowPins[numRows] = {2, 3, 4, 5, 6, 7}; 

const int numCols = 8;
int colPins[numCols] = {10, 14, 15, 16, 18, 19, 20, 21}; 

// Tracks if a key is physically being held down right now (stops duplicates)
bool keyCurrentlyPressed[numRows][numCols];

// Remembers the unique number (1 to 44) assigned to the key
int keyRegistrationNumber[numRows][numCols];

int keysFound = 0;

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("44-Key Checklist Verifier Ready!");
  Serial.println("Find all 44 keys. Duplicates will be flagged.");
  Serial.println("---------------------------------------------------------");

  for(int c = 0; c < numCols; c++) {
    pinMode(colPins[c], INPUT_PULLUP);
  }
  
  for(int r = 0; r < numRows; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], HIGH); 
  }

  // Wipe memory banks
  for(int r = 0; r < numRows; r++) {
    for(int c = 0; c < numCols; c++) {
      keyCurrentlyPressed[r][c] = false;
      keyRegistrationNumber[r][c] = 0;
    }
  }
}

void loop() {
  for(int r = 0; r < numRows; r++) {
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(50); // Stabilize voltage

    for(int c = 0; c < numCols; c++) {
      int currentState = digitalRead(colPins[c]);

      // If key is PRESSED and we haven't processed this exact press yet
      if(currentState == LOW && keyCurrentlyPressed[r][c] == false) {
        
        keyCurrentlyPressed[r][c] = true; // Lock it so it doesn't spam
        
        // Is this the very first time we have EVER seen this key?
        if(keyRegistrationNumber[r][c] == 0) {
          keysFound++;
          keyRegistrationNumber[r][c] = keysFound;
          
          Serial.print("SUCCESS: NEW KEY [");
          Serial.print(keysFound);
          Serial.print("/44] -> Row: ");
          Serial.print(rowPins[r]);
          Serial.print(" | Col: ");
          Serial.println(colPins[c]);
        } 
        // If we have seen it before, flag it as a duplicate
        else {
          Serial.print("Already registered: Key #");
          Serial.print(keyRegistrationNumber[r][c]);
          Serial.print(" (Row: ");
          Serial.print(rowPins[r]);
          Serial.print(" | Col: ");
          Serial.print(colPins[c]);
          Serial.println(")");
        }
        
        delay(50); // Debounce physical contact
      }
      
      // If key is LET GO, unlock it so it can be pressed again later
      else if(currentState == HIGH && keyCurrentlyPressed[r][c] == true) {
        keyCurrentlyPressed[r][c] = false;
        delay(10); // Debounce release
      }
    }
    
    digitalWrite(rowPins[r], HIGH);
  }
}
