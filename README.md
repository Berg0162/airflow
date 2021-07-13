# <img src="../main/images/AF_logo.png" width="64" height="64" alt="Airflow Icon"> &nbsp; AIRFLOW
<img src="../main/images/AIRFLOW_com.png" width="900" height="500" align = "middle" alt="Airflow"> <br>
# Thermoregulated Airflow Cooling for Indoor Cycling <br>
The <b>AIRFLOW</b> is a smart device that accurately generates the ideal airflow velocity for an optimally thermoregulated human body when heat production balances heat loss. Every minute the <b>AIRFLOW</b> smart device determines the very appropriate airflow velocity to reconcile the Heat Balance Equation.
<img src="../main/images/HeatBalancePic_small.png" width="400" height="100" align = "right" alt="Heat Balance"> For indoor cycling on a static trainer a stable Heat Balance is paramount. Thermal heat stress may seriously affect the performance, overall productivity, safety and health of an individual. Discomfort at least, even heat illness and at worst heat stroke are three phases of the reaction of the human body when exposed to an unstable Heat Balance (hot, humid environment and inadequate airflow). <br clear="left">
<img src="../main/images/AIRFLOW_equation.png" width="500" height="140" align = "right" alt="Airflow equation">
The Heat Balance Equation can be rewritten in such a way that the requested Airflow Velocity (<b>V<sub>a</sub></b>) is a function of the other terms and can be calculated at any time!
The <b>AIRFLOW</b> smart device continuously tunes accordingly the requested airflow velocity of the fan(s) for a stable Heat Balance during all phases of an indoor cycling workout, warm-up, intensity intervals, intermittent recovery and at cooldown. The cyclist has no on-the-way interference and can fully concentrate on the demands of the stationay trainer workout, always facing the ideal airstream that will cool him/her appropriately. <br clear="left">
In a seperate document the Heat Balance Equation and the applied algorithms are explained and elaborated see: <br>
* [View Heat Balance And Algorithms Elaborated](../main/docs/Heat_Balance_And_Algorithms_Elaborated.pdf) <br>
* [View Calculation Example in Appendix](../main/docs/APPENDIX.pdf) <br>

# What is needed to determine the critical variables of the Heat Balance Equation <br>

* <b>Heart Rate Monitor (Dual ANT+ and) Bluetooth LE transmitter</b> 
<img src="../main/images/heart_rate_monitor.jpg" width="200" height="150" ALIGN="right" alt="Heart rate monitor" > <br>
The <b>AIRFLOW</b> device needs continuous measurement of the heart rate to determine the critical mean body temperature which is proportional to the netto heat that is stored in the body. Most cyclists are used to wear a heart rate band during a workout and when that band is transmitting data over BLE it will be suitable for the AIRFLOW device. Notice that many of the (older) devices allow only <b>one</b> BLE connection at the time, which means that you cannot concurrently connect your cyling computer and Zwift computer and/or <b>AIRFLOW</b> device over BLE with the heart rate band... If possible: Use ANT+ for regular connections and only BLE for the AIRFLOW device!
<br clear="left">

* <b>A Power meter with (Dual ANT+ and) Bluetooth LE transmitter</b>
<img src="../main/images/power_meters.png" width="350" height="200" ALIGN="right" alt="Power meters" > <br>
The <b>AIRFLOW</b> device needs continuous measurement of the critical cycling power, produced during a workout, to determine how much energy the body is generating.
<ins>The Power meter of your smart indoor trainer can supply the power measurements that you push during a workout.</ins> Or a dedicated power meter mounted on the bike at the crank, pedals or rear hub as long as it is capable of transmitting power data over Bluetooth Low Energy (BLE). Notice that many of the (older) devices allow only <b>one</b> BLE connection at the time, which means that you cannot concurrently connect your cyling computer and Zwift computer and/or AIRFLOW device over BLE with the power meter... If possible: Use ANT+ for regular connections and only BLE for the <b>AIRFLOW</b> device!
<br clear="left">

* <b>Temperature & Humidity Sensor</b> <br>
Ambient air temperature and relative humidity are critical variables during any serious workout. These are measured with a Adafruit sensor continously and is part of the electronic circuitry that processes all measurements! <br>

# Electronic Components and Circuitry of AIRFLOW device<br>
* Adafruit Sensirion SHT31-D - Temperature & Humidity Sensor
* Adafruit OLED 128x64 I2C blue display
* Robotdyn AC Light Dimmer Module, 1 Channel, 3.3V/5V logic, AC 50/60hz, 220V/110V
* Adafruit Feather nRF52840 Express
* Table Fan 220/110 Volt AC 50/60 hz
<br>
In the following image the wiring of the electronic components is shown.<br>
<img src="../main/images/AF_circuitry.png" width="900" height="500" ALIGN="middle" alt="Circuitry" > <br>

# PWM <br>

The AC Dimmer Module by Robotdyn is designed to control Alternating Current/voltage (110/220V). It can control AC levels up to 400V/8А. In most cases, the AC Dimmer Module is used to turn the power ON/OFF or <b>dim</b> lamps or heating elements. It can be used as well with fans, pumps, air cleaners, etcetera. A major benefit of the Robotdyn board is that the 110/220V part is (optically) isolated from the 5V logical control part, to minimize the possibility of high voltage damage of the attached low voltage microcontroller. The logical level of the Dimmer Module is tolerant to 5V and 3.3V, therefore it can be connected to a microcontroller with 5V and 3.3V level logic. <br>
For our Arduino Feather nRF52 Express a dedicated Timer/Interrupt handling library has been applied, which allows external interrupts and process internal time interrupts to switch AC power ON/OFF to enable PWM on the fans. The AC Dimmer is connected to the Arduino Feather nRF52 board via two digital pins. One pin (interrupt) is to detect the Zero Crossing of the 50 Hz AC Phase, which is applied to initiate an interrupt signal to start the cut-off of the AC cycle. A second pin (PWM) is to control the width (or time) of the cut-off duration of the cycle, which determines how long the current is switched ON/OFF. This results in a software controlled duty cycle of the fan that will force the fan to proportionally blow the cooling air harder or softer. 

# Timer library <br>
[View the code of the Timer library](../main/arduino/libraries/Timer/Timer.cpp) <br>
[View the header of the Timer library](../main/arduino/libraries/Timer/Timer.h) <br>

I am much obliged to Cedric Honnet who designed the Timer library initially as a part of the Hivetracker project, ref: [Original Hivetracker Timer library](https://github.com/HiveTracker/Timer). The library code has been updated to conform the more recent nRF52-timer implementations on nRF52 boards.<br>

# All Relevant Code Snippets in Main program for handling duty cycles of both fans<br>
<img src="../main/images/Duty_Cycle_animation.gif" width="160" height="110" ALIGN="right" alt="Duty Cycle" >
The global variables <b>ActualUpperFanPerc</b> and <b>ActualLowerFanPerc</b> are determined by the outcome of the Heat-Balance-Equation and set to the appropriate percentage of fan capacity (0-100%) and this is translated to precise time intervals (in microseconds) for the duty cycle by <b>SetBothFanPeriods(void)</b>. Every 10 milliseconds a Zero Cross (external) interrupt is detected and the duty cycles for both fans are set separately using 2 of the accurate internal nRF52 microsecond-timers.<br clear="left">

```C++
#include <nrf_timer.h> // Native nRF52 timers library
#include <Timer.h>     // Heavy duty micro(!)seconds timer library based on nRF52 timers, needs to reside in the libraries folder
```

```C++
// Feather nRF52840 I/O Pin declarations for connection to the ROBOTDYN AC DIMMER boards
#define PWM_PIN_U_FAN  (12U) // Upper Fan 
#define ISR_PIN        (11U) // AC Cycle Zero Cross detection pin (for both FANS)
#define PWM_PIN_L_FAN  (10U) // Lower FAN

// Values are in microseconds and a half 50Hz cycle is 10.000us = 10 ms !
#define MAX_TIME_OFF_UPPER (6950U) // Maximal time of half an AC Cycle (10.000us) to be CUT-OFF in microseconds!
#define MAX_TIME_OFF_LOWER (6920U) // Maximal time of half an AC Cycle (10.000us) to be CUT-OFF in microseconds!
#define MIN_TIME_OFF (2000U)       // Minimal time to process for full power --> NO (!) CUT-OFF situation in microseconds!
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

```C++
void SetBothFanPeriods(void) {
  // ------------------------------------------------------------------------
  ActualUpperFanPeriod = int(map(ActualUpperFanPerc, 0, 100, MAX_TIME_OFF_UPPER, MIN_TIME_OFF));
  ActualLowerFanPeriod = int(map(ActualLowerFanPerc, 0, 100, MAX_TIME_OFF_LOWER, MIN_TIME_OFF));
}
```



