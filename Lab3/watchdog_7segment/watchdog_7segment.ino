/*
  Griffin Mack
  11713813

  Lab3 - 

  *For this lab, the user is prompted for an integer between 0-15 from the serial monitor. 
  *
  *If the user enters a value in 4 seconds, their reaction time is printed to the screen and their
  *   input is displayed on the 7-segment.
  *If the user enters invalid data, an error message is displayed and the user can try again
  *If the user doesn't enter data, the 7-segment decimal blinks for 5 seconds and the board resets
  *
  * pinout for LED & 7-segment display                  7-segment display diagram
  *  arduino     7-segment                                        aa          *NOTE: pin 10(o) will
  *                                                              f  b          only display if current
  *    pin 2       a                                             f  b          value is >9 (decimal)
  *    pin 3       b                                              gg
  *    pin 4       c                                             e  c
  *    pin 5       d                                             e  c
  *    pin 6       e                                              dd  o
  *    pin 7       f
  *    pin 8       g
  *    pin 9       o
*/
#define DECIMAL_PIN 9

void setup() {
    /* setup for Serial Monitor and digital IO pins
     */
    Serial.begin(115200);                                  //initialize serial
    for(int i = 2; i < 10; i++) {
        pinMode(i, OUTPUT);                                //initialize digital pins 2-9 as output(7-segment)
    }
}

void(* resetFunc) (void) = 0;                              //function to reset arduino board

void loop() {
    /* start of program (main loop found below)
     */
    //array storing patterns for 0-F on 7-segment display
    //final entries in array corresponds to no input, initialization, and wrong input states repectively
    //    EX) Displaying 0:  0 1 2 3 4 5 6 7
    //                       a b c d e f g h
    //                       1 1 1 1 1 1 0 0
    String patterns[19] = {"11111100", "01100000", "11011010", "11110010",
                           "01100110", "10110110", "10111110", "11100000",
                           "11111110", "11110110", "11101111", "00111111",
                           "10011101", "01111011", "10011111", "10001111", 
                           "00000001", "00000010", "10010010"};
            
    // serial input variables
    int user_int = 0;                                     //stores converted user input from serial monitor
    String user_input = "";                               //stores user input from serial monitor
    bool new_input = false;                               //flag to tell if new input has been recieved
    bool reset_flag = true;                               //board resets if this flag is true


    // variables used for reaction timing
    unsigned long start_time = 0;                         //the time when the user is prompted for input
    unsigned long current_time = 0;                       //current run time
    int output_time = 0;                                  //time outputted to user (always between 0-4000 ms)
    const int timer_interval = 4000;                      //amount of time given for reaction timer

    startupMessage();                                     //inform the user about the system
    while(1) {
        //reset all variables to their default state
        displayNumber(patterns[17]);                      //initializing 7-segment display(-)
        reset_flag = true;                                //system should reset after unless told otherwise
        new_input = false;
        user_input = "";
        
        // inform the user how to use the counter, and start the counter
        Serial.println("Please enter an integer value between 0-15(decimal values only):");
        start_time = millis();  //grab the current time
        current_time = start_time;
        output_time = 0;

        while(current_time - start_time <= timer_interval && reset_flag == true){
            if (Serial.available() > 0) {
                //see if user has inputed anything
                user_input = Serial.readString();
                new_input = true;                         //flags a user input
                output_time = current_time - start_time;  //obtain more user friendly time
            }
            if(new_input == true){
                user_int = user_input.toInt();            //convert string input into an Int. 
                                                          //Since non-integers convert to '0', 
                                                          //must string compare for this case
                if(user_input == "0" || (user_int >= 1 && user_int <= 15)){   //check if the input is between 0-15
                    displayNumber(patterns[user_int]);    //display user input on the 7-segment display
                    //display the reaction time to the serial monitor
                    Serial.print("Nice Reaction! Input: '");
                    Serial.print(user_input);                 
                    Serial.print("' Time: ");
                    Serial.print(output_time / 1000);
                    Serial.print(".");
                    Serial.println(output_time % 1000);   //output: "Nice Reaction! Input: 'z' Time: s.xxx
                    delay(5000);                          //small delay so user can read serial and 7-segment

                }
                else{                                     //input was provided but not in the correct range
                    //display error message to serial
                    Serial.print("INVALID INPUT: '");
                    Serial.print(user_input);
                    Serial.println("' please input an integer between 0-15...");
                    displayNumber(patterns[18]);          //display top, middle, and bottom bars on 7-segment
                    delay(5000);                          //small delay so user can read error message
                }
                reset_flag = false;                       //don't hard reset device, just restart timer
            }
            current_time = millis();
        }
        if(reset_flag == true){
            //it has been more than 4 seconds, time to reset
            Serial.println("No input provided, resetting device");
            clearDisplay();                               //assure 7-segment is off
            blinkDecimal();                               //blink decimal LED for 5 seconds
            delay(5000);                                  //small delay before reset
            resetFunc();                                  //hard reset of the arduino board
        }
    }      
}

void displayNumber(String pattern) {
    /* uses the pattern provided to write HIGH or LOW to the respective 7-segment pins
     */
    int offset = 2;                                      //digital outputs start at pin 2, not pin 0
    for(int i = 0;i< 8;i++) {
        if(pattern[i] == '1') {
          digitalWrite(i+offset, HIGH);                  //turn segment LED on if the pattern is 1
        }
        else {
          digitalWrite(i+offset, LOW);                   //turn segment LED off if the pattern is 0
        }
    }
}

void clearDisplay() {
    /* turns off all LED's on the 7-segment display
     */
    int offset = 2;                                      //digital outputs start at pin 2, not pin 0
    for(int i = 0;i< 8;i++) {
        digitalWrite(i+offset, LOW);                     //turn segment LED off
    }
}

void blinkDecimal() {
    /* blinks decimal LED on seven segment display 5 times
     */
    for(int i = 0;i < 5; i++){
        digitalWrite(DECIMAL_PIN, HIGH);                 //turn decimal LED on
        delay(500);
        digitalWrite(DECIMAL_PIN, LOW);                  //turn LED decimal off
        delay(500);
    }
}

void startupMessage(){
    /*prints startup message to serial monitor for user to understand the system
     */
    Serial.println("System started...\n");
    Serial.print("When prompted, a reaction timer will start.");
    Serial.println(" Enter an integer value between 0-15 to test your reaction time");
    Serial.println("If no input is detected, the system will reset");
    Serial.println("If invalid input is detected, the timer will reset.\n\n");
    delay(2000);                                        //give user time to read prompt
}
