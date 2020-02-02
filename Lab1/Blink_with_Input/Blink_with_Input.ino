/*
  Blink
*/
char readString[50];
int charsRead;
int count = 0;                                          //dummy variable for displaying prompt to Serial
void setup() {
    //initialize serial:
    Serial.begin(9600);
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(12, OUTPUT);
}

void loop() {
//  Serial.print("Please input a character: ");

    if(count == 0){
        Serial.print("Please input a character: \n");
        count = count + 1;
    }
    if (Serial.available()) {
        charsRead = Serial.readBytesUntil('\n', readString, sizeof(readString) - 1);
        while(charsRead == 1 && readString[0] != 's' && readString[0] == 'g') {   //value inputed is one character, is a 'g', is NOT a 's'
            if(count == 1){
                Serial.print("Please input a character: \n");
                count = count + 1;
            }
            blinkLED();
            if (Serial.available()) {                     //monitor for an 's' input
                charsRead = Serial.readBytesUntil('\n', readString, sizeof(readString) - 1);
            }
        }
        digitalWrite(12, LOW);                        // turn the LED to OFF (HIGH is the voltage level)
        count = 0;
    }
}

void blinkLED() {
    digitalWrite(LED_BUILTIN, HIGH);                  // turn the onboard LED on
    digitalWrite(12, LOW);                            // turn the LED off
    delay(2000);                                      // wait for two seconds
    digitalWrite(LED_BUILTIN, LOW);                   // turn the onboard LED off
    digitalWrite(12, HIGH);                           // turn the LED on
    delay(1000);                                      //wait for one second 
}
