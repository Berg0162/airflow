// -------------------------------------------------------
// Test program for timers controlling 2 fans attached with ROBOTDYN modules
// ------------------------------------------------------
// 

#include <Wire.h>

#include <nrf_timer.h>
#include <Timer.h>

// define the FAN pin connections
#define PWM_PIN_U_FAN  (12UL) // Upper Fan 
#define ISR_PIN        (11UL) // AC Cycle Zero Cross detection pin (for both FANS)
#define PWM_PIN_L_FAN  (10UL) // Lower FAN

#define LED_PIN_BLUE (LED_BLUE)
#define LED_PIN_BUILTIN (LED_BUILTIN)

// Values are in microseconds and a half 50Hz cycle is 10.000us = 10 ms !
#define MAX_TIME_OFF (9000)
#define MIN_TIME_OFF (0)

// -------------------------------------------------------
// NOTICE type (int) !!
int L_FAN_period; // microseconds cut out of a half AC cycle; 
// NOTICE type (int) !!
int U_FAN_period; // microseconds cut out of a half AC cycle;
//-------------------------------------------------------
bool U_FAN_countUP = false;
int U_FAN_perc = 95;
bool L_FAN_countUP = true;
int L_FAN_perc = 5;

int percStep = 5;
uint8_t volatile ISR_cnt = 0;
#define MAX_ISR_CNT 50

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Timers 2 Fans testing Example");
  Serial.println("-----------------------------\n");
    
    pinMode(LED_PIN_BLUE, OUTPUT);
    digitalWrite(LED_PIN_BLUE, LOW);
    pinMode(LED_PIN_BUILTIN, OUTPUT);
    digitalWrite(LED_PIN_BUILTIN, HIGH);
    
    pinMode(ISR_PIN, INPUT_PULLDOWN);  // Set Zero Cross Interrupt pin
    
    pinMode(PWM_PIN_U_FAN, OUTPUT);
    digitalWrite(PWM_PIN_U_FAN, LOW); // Start at OFF state = LOW
    pinMode(PWM_PIN_L_FAN, OUTPUT);
    digitalWrite(PWM_PIN_L_FAN, LOW); // Start at OFF state = LOW

    
    U_FAN_period = map(U_FAN_perc, 0, 100, MAX_TIME_OFF, MIN_TIME_OFF);
    L_FAN_period = map(L_FAN_perc, 0, 100, MAX_TIME_OFF, MIN_TIME_OFF);

    // ISR Function is called 2 times in one full 50Hz cycle --> 100 times in a second
    attachInterrupt(digitalPinToInterrupt(ISR_PIN), zero_Cross_ISR, RISING);  // Set Zero-Crossing Interrupt Service Routine
    // define 2 timers for 2 different FANS to independently function !!!
    NrfTimer1.attachInterrupt(&U_FAN_Duty_Cycle_Ends, 300); // microseconds
    NrfTimer2.attachInterrupt(&L_FAN_Duty_Cycle_Ends, 400); // microseconds
    
    Serial.println("Starting...");
}

void zero_Cross_ISR(void) {
    digitalWrite(PWM_PIN_U_FAN, LOW); // Duty Cycle time starts for U_FAN_period time
    digitalWrite(PWM_PIN_L_FAN, LOW); // Duty Cycle time starts for L_FAN_period time
    ISR_cnt += 1;
    NrfTimer1.attachInterrupt(&U_FAN_Duty_Cycle_Ends, U_FAN_period); // microseconds
    NrfTimer2.attachInterrupt(&L_FAN_Duty_Cycle_Ends, L_FAN_period); // microseconds
}

void U_FAN_Duty_Cycle_Ends(void) {
    digitalWrite(PWM_PIN_U_FAN, HIGH); // Set AC cycle ON --> HIGH 
    }

void L_FAN_Duty_Cycle_Ends(void) {
    digitalWrite(PWM_PIN_L_FAN, HIGH); // Set AC cycle ON --> HIGH 
}

void loop() {
  if (ISR_cnt >= MAX_ISR_CNT) 
    { // New dimming value after ## ISR ticks............   
    Serial.printf("L: %3d U: %3d - ", L_FAN_perc, U_FAN_perc);
    if (U_FAN_perc >= 100){
      digitalWrite(LED_PIN_BUILTIN, LOW);
      U_FAN_countUP = false; 
    } else {
      if (U_FAN_perc <= 0) {
      digitalWrite(LED_PIN_BUILTIN, HIGH);
      U_FAN_countUP = true;}
    }
    if (U_FAN_countUP) { 
      U_FAN_perc += percStep; }
    else { 
      U_FAN_perc -= percStep; }
    // ----------------------------
    if (L_FAN_perc >= 100){
      digitalWrite(LED_PIN_BLUE, LOW);
      L_FAN_countUP = false; 
    } else {
    if (L_FAN_perc <= 0) {
      digitalWrite(LED_PIN_BLUE, HIGH);
      L_FAN_countUP = true; }
    }
    if (L_FAN_countUP) { 
      L_FAN_perc += percStep; }
    else { 
      L_FAN_perc -= percStep; } // ---
    ISR_cnt = 0; 
    U_FAN_period = map(U_FAN_perc, 0, 100, MAX_TIME_OFF, MIN_TIME_OFF);
    L_FAN_period = map(L_FAN_perc, 0, 100, MAX_TIME_OFF, MIN_TIME_OFF);
    Serial.printf("L-per: %05d U-per: %05d \n", L_FAN_period, U_FAN_period);
    }
//   Sleep
    __WFE(); // Enter System ON sleep mode (OFF mode would stop timers)
    __SEV(); // Make sure any pending events are cleared
    __WFE(); // More info about this sequence:
// devzone.nordicsemi.com/f/nordic-q-a/490/how-do-you-put-the-nrf51822-chip-to-sleep/2571
}
