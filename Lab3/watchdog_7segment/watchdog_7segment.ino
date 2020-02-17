/*
  Griffin Mack
  11713813

  Lab2 - Introduction to Digital Input/Output

  *For this lab, user input is taken from an external switch. Depending on the input,      
  *the Arduino onboard LED will blink, along with an external LED that is 180 out of phase.
  *
  *Additionally, each change of the switch increments a counter that is displayed in decimal,
  *and in hexidecimal on the serial display. The hex count will also be displayed on an 
  *external 7 segment display.
  *
  * pinout for LED & 7-segment display                  7-segment display diagram
  *  arduino     7-segment                                        aa          *NOTE: pin 1(o) will
  *    pin 2       o                                             f  b          only display if current
  *    pin 3       a                                             f  b          value is >9 (decimal)
  *    pin 4       b                                              gg
  *    pin 5       c                                             e  c
  *    pin 6       d                                             e  c
  *    pin 7       e                                              dd  o
  *    pin 8       f
  *    pin 9       g
  *    pin 10       o
  *    pin 11      LED(external)
*/
#define DECIMAL_PIN 10
#define SWITCH_PIN 11

void setup() {
    /* setup for Serial Monitor and digital IO pins
     */
    Serial.begin(115200);                                      // initialize serial
    pinMode(SWITCH_PIN, INPUT);                                // initialize digital pin 11 as input(switch)
    pinMode(LED_BUILTIN, OUTPUT);                              // initialize digital pin LED_BUILTIN as output
    for(int i = 2; i < 11; i++) {
        pinMode(i, OUTPUT);                                    // initialize digital pins 2-10 as output(7-segment)
    }
}

void loop() {
    /* start of program (main loop found below)
     */
    //array storing patterns for 0-F on 7-segment display
    //final entries in array corresponds to initialization, no input, and error state repectively
    //    EX) Displaying 0:  0 1 2 3 4 5 6 7
    //                       a b c d e f g h
    //                       1 1 1 1 1 1 0 0
    String patterns[19] = {"11111100", "01100000", "11011010", "11110010",
                           "01100110", "10110110", "10111110", "11100000",
                           "11111110", "11110110", "11101111", "00111111",
                           "10011101", "01111011", "10011111", "10001111", 
                           "00000001", "10000000", "01001001"};
    // watchdog and input variables
    int incoming = 0;
    unsigned long start_time = 0;
    unsigned long current_time = 0;
    const int interval = 4000;
    bool new_input = false;
    bool reset_flag = false;

    // variables used to blink LED's
    unsigned long previous_time = 0;
    unsigned long new_time = 0;
    const int blink_interval = 500;
    int LED_state = LOW;


    while(1) {
        displayNumber(patterns[16]);                       // initializing 7-segment display(-)
        Serial.println("Please enter an integer value between 0-15:");        // inform the user how to use the counter
        reset_flag = true;

        start_time = millis();  //grab the current time
        current_time = start_time;

        //if it has been less than 4 seconds since start
        while(current_time - start_time <= interval){
            Serial.println(current_time);
            
            if (Serial.available() > 0) {
                //see if user has inputed anything
                incoming = Serial.parseInt();
                Serial.println(incoming);
                new_input = true;
            }

            if(new_input = true){
                //check if the input is between 0-15
                if(incoming >= 0 && incoming <= 15){
                    //input is valid, display the input on 7-segment
                    displayNumber(patterns[incoming]);
                    //display the reaction time to the serial monitor s.xxx
                    Serial.print("Nice Reaction! Time: ");
                    Serial.println(incoming);
                }
                else{
                    //display error on 7-segment, error on serial
                    displayNumber(patterns[18]);
                    //restart the timer, not the board
                }
                reset_flag = false;
            }

            current_time = millis();   //grab new time for next run
        }
        
        if(reset_flag == true){
            //it has been more than 4 seconds, time to reset
            Serial.println("No input provided, resetting 7-segment");
            displayNumber(patterns[17]);   //18 is the decimal point only
            //the decimal should blink now for 5 seconds
            //board should now be reset
        }
    }
        
}

void displayNumber(String pattern) {
    /* uses the pattern provided to write HIGH or LOW to the respective 7-segment pins
     *  INPUT:  pattern to print
     *  OUTPUT: number displayed on 7-segment display
     */
    int offset = 2;                                            // digital outputs start at pin 2, not pin 0
    for(int i = 0;i< 8;i++) {
        if(pattern[i] == '1') {
          digitalWrite(i+offset, HIGH);                        // turn segment LED on if the pattern is 1
        }
        else {
          digitalWrite(i+offset, LOW);                         // turn segment LED off if the pattern is 0
        }
    }
}

void serialPrint(int switch_counter) {
    /* prints the current switch counter to the Serial Monitor
     *  INPUT:  current counter values
     *  OUTPUT: "current count.. decimal: 1    hex: 0x1"
     */
    Serial.print("current count.. decimal: ");
    Serial.print(switch_counter);                              // display switch_counter in decimal format
    Serial.print("    hex: 0x");
    Serial.println(switch_counter % 16, HEX);                  // display switch_counter in hex (only 0-F)
}

int blinkLED(int* LED_state) {
    /* takes current LED state and reverses it(blinks)
     *  INPUT:  current LED state (onboard LED)
     *  OUTPUT: onboard LED and external LED flipped state
     *  RETURN: new LED state
     */
    if (*LED_state == LOW) {
      *LED_state = HIGH;                                       // toggle LED LOW->HIGH
    }
    else {
      *LED_state = LOW;                                        // toggle LED HIGH->LOW
    }
    digitalWrite(DECIMAL_PIN, *LED_state);
    return *LED_state;                                         // return new LED state
}
