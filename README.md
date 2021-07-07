# <img src="../main/images/AF_logo.png" width="64" height="64" alt="Airflow Icon"> &nbsp; Airflow

Thermoregulated Airflow Cooling for Indoor Cycling

etctera...

Work in progress...
# Electronic Components and Circuitry<br>
<img src="../main/images/AF_circuitry.png" width="900" height="500" ALIGN="middle" alt="Circuitry" > <br>
# PWM <br>

The AC Dimmer Module by Robotdyn is designed to control Alternating Current/voltage (110/220V). It can control AC levels up to 400V/8А. In most cases, the AC Dimmer Module is used to turn the power ON/OFF or <b>dim</b> lamps or heating elements. It can be used as well with fans, pumps, air cleaners, etcetera. A major benefit of the Robotdyn board is that the 110/220V part is (optically) isolated from the 5V logical control part, to minimize the possibility of high voltage damage of the attached low voltage microcontroller. The logical level of the Dimmer Module is tolerant to 5V and 3.3V, therefore it can be connected to a microcontroller with 5V and 3.3V level logic. <br>
<img src="../main/images/Duty_Cycle_animation.gif" width="200" height="160" ALIGN="right" alt="Duty Cycle" >
For our Arduino Feather nRF52 Express a dedicated Timer/Interrupt handling library has been applied, which allows external interrupts and process internal time interrupts to switch AC power ON/OFF to enable PWM on the fans. The AC Dimmer is connected to the Arduino Feather nRF52 board via two digital pins. One pin (interrupt) is to detect the Zero Crossing of the 50 Hz AC Phase, which is applied to initiate an interrupt signal to start the cut-off of the AC cycle. A second pin (PWM) is to control the width (or time) of the cut-off duration of the cycle, which determines how long the current is switched ON/OFF. This results in a software controlled duty cycle of the fan that will force the fan to proportionally blow the cooling air harder or softer. <br clear="left">

+ Timer library
[View the code of the Timer library] (../main/arduino/libraries/Timer/Timer.cpp) <br>
[View the header of the Timer library] (../main/arduino/libraries/Timer/Timer.h) <br>


```C++
// Values are in microseconds and a half 50Hz cycle is 10.000us = 10 ms !
#define MAX_TIME_OFF_UPPER (6950U) // Maximal time of half an AC Cycle (10.000us) to be CUT-OFF in microseconds!
#define MAX_TIME_OFF_LOWER (6920U) // Maximal time of half an AC Cycle (10.000us) to be CUT-OFF in microseconds!
#define MIN_TIME_OFF (2000U)   // Minimal time to process for full power --> NO (!) CUT-OFF situation in microseconds!
```

```C++
void zero_Cross_ISR(void) {
  // Normal mode
  digitalWrite(PWM_PIN_L_FAN, LOW); // Set AC cycle OFF --> LOW
  digitalWrite(PWM_PIN_U_FAN, LOW); // Set AC cycle OFF --> LOW

  NrfTimer2.attachInterrupt(&LFanDutyCycleEnds, ActualLowerFanPeriod); // microseconds !
  NrfTimer1.attachInterrupt(&UFanDutyCycleEnds, ActualUpperFanPeriod); // microseconds !
}

void UFanDutyCycleEnds(void) {
  digitalWrite(PWM_PIN_U_FAN, HIGH); // Set AC cycle ON --> HIGH
}

void LFanDutyCycleEnds(void) {
  digitalWrite(PWM_PIN_L_FAN, HIGH); // Set AC cycle ON --> HIGH
}

```



