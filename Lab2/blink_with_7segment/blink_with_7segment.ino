/*
  Griffin Mack
  11713813

  Lab2 - Introduction to Digital Input/Output

  *For this lab, user input is taken from an external switch. Depending on the input,      
  *the Arduino onboard LED will blink, along with an external LED that is 180 out of phase.
  *
  *Additionally, each change of the switch increments a counter that is displayed in decimal,
  *and in hexidecimal on the serial display. This count's LSB will also be displayed on an 
  *external 7 segment display.
  *
  * pinout for LED & 7-segment display                  7-segment display diagram
  *  arduino     7-segment                                        aa          *NOTE: pin 1(o) will
  *    pin 1       o                                             f  b          only display if current
  *    pin 2       a                                             f  b          value is >9 (decimal)
  *    pin 3       b                                              gg
  *    pin 4       c                                             e  c
  *    pin 5       d                                             e  c
  *    pin 6       e                                              dd  o
  *    pin 7       f
  *    pin 8       g
  *    pin 9       o
  *    pin 10      LED(external)
*/
void setup() {
    Serial.begin(9600);                               // initialize serial:
    pinMode(11, INPUT);                               // initialize digital pin 11 as input(switch)
    pinMode(LED_BUILTIN, OUTPUT);                     // initialize digital pin LED_BUILTIN as output.
    for(int i = 2; i < 11; i++)
        pinMode(i, OUTPUT);                           // initialize digital pins 2-10 as output.
    }
void loop() {
    // variables used to blink LED's
    unsigned long previous_time = 0;
    unsigned long current_time = 0;
    const long interval = 1000;
    const int LED_pin = 10;
    int LED_state = LOW;

    // array storing patterns for 0-F(indexed accordingly)
    String patterns[16] = {"11111100", "01100000", "11011010","11110010", "01100110", "10110110", "10111110", "11100000",
                           "11111110", "11110110", "11101111", "00111111", "10011101", "01111011", "10011111", "10001111"};

    // variables used to monitor switch position
    const int switch_pin = 11;
    int switch_state = LOW;
    int previous_switch_state = LOW;
    int switch_counter = 0;

    // initializing 7-segment display
    displayNumber(patterns[0]);

    while(1){

        // check to see if the switch state has changed
        switch_state = digitalRead(switch_pin);
        if(switch_state != previous_switch_state){
            // if it has changed from LOW->HIGH, iterate the count and display
            if(switch_state == HIGH){
                switch_counter ++;                                        // increment the counter
                displayNumber(patterns[switch_counter % 16]);             // display the counter on 7-segment
                serialPrint(switch_state, switch_counter);                // display the counter on serial monitor
            }       
            // if it has changed from HIGH->LOW, turn off LED's
            else{
                digitalWrite(LED_pin, LOW);
                digitalWrite(LED_BUILTIN, LOW);
                LED_state = LOW;
                serialPrint(switch_state, switch_counter);                // notify user the switch is off on the serial monitor
            }
        // if the switch is HIGH, the LED's should be blinking
        }
        if(switch_state == HIGH){
            current_time = millis();                                      // outputs how long the program has been running
            if (current_time - previous_time >= interval) {               // check if it has been more than 1s since blinking
                previous_time = current_time;
                if (LED_state == LOW) LED_state = HIGH;
                else LED_state = LOW;
            digitalWrite(LED_BUILTIN, LED_state);
            digitalWrite(LED_pin, !LED_state);                            // external LED is always opposite of onboard LED
            }
        }
        // store the state of the switch to compare during next run
        previous_switch_state = switch_state;
    }
}

void displayNumber(String pattern){
    // use the pattern given to write HIGH or LOW to the respective pins
    for(int i = 0;i< 8;i++){
        if(pattern[i] == '1') digitalWrite(i+2,HIGH);
        else digitalWrite(i+2, LOW);
    }
}

void serialPrint(int switch_state, int switch_counter){
    // display the new count in decimal and hexidecimal
    // current count.. decimal:()    hex:0x()
    if(switch_state == HIGH){
        Serial.print("current count.. decimal: ");
        Serial.print(switch_counter);
        Serial.print("    hex: 0x");
        Serial.println(switch_counter, HEX);
    }
    // let the user know the switch is off
    else{
        Serial.println("off");
    }
}
