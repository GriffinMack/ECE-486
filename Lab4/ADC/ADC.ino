/*
  Griffin Mack
  11713813

  Lab4 - Introduction to Analog-to-Digital Converters

  *For this lab, user input is taken from the serial monitor. If the user enters a 'c', 
  *30 analog readings are taken from a potentiometer connected to pin A0. The analog values
  *are converted to digital and displayed to the user, along with the time taken for each conversion,
  *and an average conversion time. 
  *
  *Additionally, this lab utilizes the Arduino watchdog timer. If user input is received within 4 seconds of
  *prompting (valid or not), the watchdog timer is reset. If no input is found during the time, the board resets.

*/
#include <avr/wdt.h>

#define AnalogInputPin A0
#define MaxConversions 30
void setup() {
    /*program setup
    */
    Serial.begin(9600);                    //initialize serial monitor
    Serial.println("Board was reset...");  //notify of board reset
}


void loop() {
    /*main program loop.
    */
    bool restart_flag = false;        //stores flag to restart program without a reboot
    int conversion_time = 0;          //stores time taken for ADC to convert input to digital
    int start_time = 0;               //stores beginning time before ADC begins conversion
    int time_array[MaxConversions];   //stores all conversion times for calculation average time

    int analog_input = 0;             //stores analog input
    String user_input = "";           //stores user serial input
    promptUser();

    while(restart_flag == false){
        user_input = "";              //clear the user_input variable
        while (Serial.available()) {
            wdt_reset();              //reset the watchdog timer on valid or invalid input
            char c = Serial.read();   //get one byte from serial buffer
            user_input += c;          //add byte to the input string
            delay(2);                 //small delay to allow more accurate reading from serial
        }
        if (user_input == "c") {      //check if user input is a 'c'
            Serial.println("Starting a set of conversions:");
            for(int i = 0; i < MaxConversions; i++){          //take a set of 30 conversions
                start_time = micros();                        //store time conversion started in microseconds
                analog_input = analogRead(AnalogInputPin);    //read the analog input on A0 (value is 0-1023)
                conversion_time = micros() - start_time;      //calculate time taken to convert analog input
                time_array[i] = conversion_time;              //store the time for calculating the average
                printConversion(i, analog_input, conversion_time);
            }
            printAverageTime(time_array);
            restart_flag = true;                              //restart the program but do not reboot the device
            while (Serial.available() > 0) {
                Serial.read();                                //clear the serial buffer(ignore inputs during conversion)
            }
        }
        if(user_input != "c" && user_input !=""){             //check if user input is not 'c' and not blank
            Serial.println("Error: invalid user input - the only valid user input is 'c'");
            promptUser();                                     //re-prompt the user for input
        }
    }
    
    
}

void printAverageTime(int time_array[]){
    /*calculates average conversion time for all 30 readings
     * of the ADC. Then prints the value to the serial monitor
    */
    int total_time = 0;                    //stores sum of times in time_array
    for(int i = 0;i < MaxConversions; i++){
        total_time += time_array[i];       //add time array enter to sum
    }
    float time_average = total_time/(double)MaxConversions;  //calculate the average time (sum/entries)
    Serial.print("\navg conversion time = ");
    Serial.print(time_average);
    Serial.println(" usecs\n");
}

void printConversion(int measurement, int conversion_value, int conversion_time){
    /*prints converted analog signal to the serial monitor 
     *along with time taken to convert the analog value
    */
    Serial.print("#");
    Serial.print(measurement + 1);         //measurement is 0 based
    if(measurement + 1 < 10){              //conditional formatting if the measurement is two digits
        Serial.print(":    digital value = ");
    }
    else{
        Serial.print(":   digital value = ");

    }
    Serial.print(conversion_value, HEX);   //display the integer in HEX
    Serial.print("    Time = ");
    Serial.print(conversion_time);
    Serial.println(" usecs");
}

void promptUser(){
    /*prints prompt message to serial monitor
     */
    Serial.println("Enter 'c' to start a set of conversions(30 in total) >");
    wdt_enable(WDTO_4S);                   //start the 4 second watchdog timer
}
