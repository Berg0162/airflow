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

# Controlling Air Velocity of Fans with PWM <br>

The AC Dimmer Module by Robotdyn is designed to control Alternating Current/voltage (110/220V). It can control AC levels up to 400V/8А. In most cases, the AC Dimmer Module is used to turn the power ON/OFF or <b>dim</b> lamps or heating elements. It can be used as well with fans, pumps, air cleaners, etcetera. A major benefit of the Robotdyn board is that the 110/220V part is (optically) isolated from the 5V logical control part, to minimize the possibility of high voltage damage of the attached low voltage microcontroller. The logical level of the Dimmer Module is tolerant to 5V and 3.3V, therefore it can be connected to a microcontroller with 5V and 3.3V level logic. <br>
<img src="../main/images/Duty_Cycle_animation.gif" width="160" height="110" ALIGN="right" alt="Duty Cycle" >
For our Arduino Feather nRF52 Express a dedicated Timer/Interrupt handling library has been applied, which allows external interrupts and process internal time interrupts to switch AC power ON/OFF to enable PWM on the fans. The AC Dimmer is connected to the Arduino Feather nRF52 board via two digital pins. One pin (interrupt) is to detect the Zero Crossing of the 50 Hz AC Phase, which is applied to initiate an interrupt signal to start the cut-off of the AC cycle. A second pin (PWM) is to control the width (or time) of the cut-off duration of the cycle, which determines how long the current is switched ON/OFF. This results in a software controlled duty cycle of the fan that will force the fan to proportionally blow the cooling air harder or softer. This way the airflow velocity of the fan(s) can be controlled precisely and set to the appropriate level! <br clear="left">

# Timer library <br>
[View the code of the Timer library](../main/arduino/libraries/Timer/Timer.cpp) <br>
[View the header of the Timer library](../main/arduino/libraries/Timer/Timer.h) <br>

I am much obliged to Cedric Honnet who designed the Timer library initially as a part of the Hivetracker project, ref: [Original Hivetracker Timer library](https://github.com/HiveTracker/Timer). The library code has been updated to conform the more recent nRF52-timer implementations on nRF52 boards.<br>

# All Relevant Code Snippets in Main program for handling duty cycles of both fans<br>

The global variables <b>ActualUpperFanPerc</b> and <b>ActualLowerFanPerc</b> are determined by the outcome of the Heat-Balance-Equation and set to the appropriate percentage of fan capacity (0-100%) and this is translated to precise time intervals (in microseconds) for the duty cycle by <b>SetBothFanPeriods(void)</b>. Every 10 milliseconds a Zero Cross (external) interrupt is detected and the duty cycles for both fans are set separately using 2 of the accurate internal nRF52 microsecond-timers.<br>

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
# Code snippet of Heat Balance Equation routine
Notice that Heart Rate (HRM) and Cycling Power (CPS) is measured at their respective device dependent frequencies, usually once to 4 times per second! The incoming HRM and CPS data are processed continously. Once per minute the <b>HeatBalanceAlgorithm</b> routine is called to calculate all terms of the Heat Balance Equation and to update the appropriate airflow velocity of the fans.

```C++
void HeatBalanceAlgorithm(void) {
  double F2, F3;
  float DeltaPska, DeltaTska, Psa;
  float Psk = 0.0; // Partial vapor pressure at skin temperature
  float Pair = 0.0; // Partial vapor pressure at ambient air temperature
  // -----------------------------------------------------------------------------------------------
  // NOTICE: Dry-Bulb Temperature EQUALS, in indoors situation, the Ambient Air Temperature --> Tdb = Tair
  //------------------------------------------------------------------------------------------------
  // Algorithm that estimates Core Temp. based on HeartRateMeasurement >>>>> AT ONE MINUTE INTERVALS !!
  if (IsConnectedToHRM) { // Only calculate and update Core Temperature when connected to HRM-strap
    EstimateTcore(Tcore_prev, v_prev, (double)HBM_average); // Estimate the Core Body Temperature from Heart Beat sequence
  }
  v_prev = v_cur;

  // Calculate environmental variables
  Psa = (float)Pantoine((double)Tair); // in units kPa
  Pair = RH * Psa; // saturated water vapour pressure at ambient temperature corrected for Relative Humidity (default = 70%)

  // Calculate Metabolic Energy
  MetEnergy = (double)( CPS_average * ((float)(100 / GE)) ) / Ad; // in Joules/m2 --> individualized (div Ad)

  // Estimate now the mean body skin temperature from
  Tskin = EstimateSkinTemp(Tair, Pair, Vair, MetEnergy, (float)Tcore_cur);

  // Continued: Calculate environmental variables and determine the appropriate Delta's
  Psk = (float)Pantoine((double)Tskin); // saturated water vapour pressure at the wetted skin surface in units kPa
  DeltaPska = (Psk - Pair); // Calculate the DeltaPska pKa
  DeltaTska = (Tskin - Tair); // Calculate DeltaTska in degrees Celsius

  // Determine now the Mean Body Temperature from estimated Core Temperature and mean estimated Skin temperature
  Tbody_cur = (float)0.67 * Tcore_cur + 0.33 * Tskin; // equation according to Kerslake, 1972 (confirmed in other papers)

  // Calculate how much heat has been produced since previous round
  if (IsConnectedToHRM) { // Only calculate and update HeatChange when connected to HRM-strap
    if (Tcore_cur > Tcore_start) { // Skip first measurement(s) until a steady effort is delivered --> Tcore_start temperature has reached!
      DeltaStoredHeat = HeatChange(Tbody_cur, Tbody_prev, HC_Interval); // Heat Change calculated in J/m2
    } else {
      DeltaStoredHeat = 0.0;
    }
  }
  // ...... some tweaking ....
  if ( (DeltaStoredHeat < 0) && TWEAK ) { // TWEAK Heat loss  !
    StoredHeat += (1.50*DeltaStoredHeat); // Tcore-algorithm underestimates Heat Loss (Cool down) --> Increase artificially Heat Loss contribution !!!
  } else {
    StoredHeat += DeltaStoredHeat;   // equation  S == internally stored body heat NO Tweaking
  }

  // Calculate Core Temp change
  DeltaTc = (Tcore_cur - Tcore_prev);

  // Store the actual T-results now for comparison in the next round to determine the T-Delta's
  Tcore_prev = Tcore_cur;
  Tbody_prev = Tbody_cur;

  // Calculate Terms from Heat balance equation
  F2 = (double)(Hc * DeltaTska + He * DeltaPska); // equation  1 (C+E) without Va(exp 0.84)
  Radiation = (double)Hr * DeltaTska;        // equation  Radiation heat exchange

  // equation component H  use CPS_average for "External Work"
  HeatProduced = (double)( CPS_average * ((float)(100/GE) - 1) ) / Ad; // in J/m2 equation 1 H = (M-W)/Ad & GE = W/M -> M = W/GE
  SumHeatProduced += HeatProduced * HC_Interval / 1000; // sum of total heat produced during the workout, units in kJ/m2
  SumEnergyProduced += MetEnergy * HC_Interval / 1000; // in kJ/m2 --> individualized

  // Solve Full Heat Balance equation with HeatProduced (H) AND HeatStored (S) to find air velocity !
  // Heat Balance equation:
  // StoredHeat represents the stored internal heat (S) that is to be removed together with the produced heat (H),
  // the sum of both (!) is the heat we want to exchange with the environment...
  F3 = (HeatProduced + StoredHeat - Radiation) / F2; // right side of the FULL heat balance equation S = StoredHeat
  // POW with decimal exponent (1.1905) can't process a negative base (F3), --> POW equation gives "nan" result!
  if ( F3 > 0 ) {
    Vair = (float)pow(F3, 1.1905);         // in equation exp == 0.84 --> 84/100 -> 100/84 = 1.1905
  } else Vair = 0.0;

  // Now that we know Va, C and E can be re-calculated !! Notice H and R are independent of airflow speed so they hold the same values!!
  Convection = (float)pow((double)Vair, 0.84) * Hc * DeltaTska; // 0.84 to conform with previous calculations !!
  Ereq = (float)(HeatProduced + StoredHeat - Radiation - Convection); // Requested Evaporative Heat exchange with the environment
  if (Ereq < 0) {
    Ereq = 0;  // in startup phase value can be negative force to zero !
  }
  //Sweat Rate = (147 + 1.527*Ereq - 0.87*Emax) --> The Original equation of Conzalez et al. 2009
  // Adapted since in our heat balance equation Ereq = Emax (by definition!) -> rewritten to: SwRate = 147 +(0.657*Ereq)
  if (Ereq > 0) {
    SwRate = (147 + (0.657 * Ereq)); // per milli liter fluid is equal to weight in grams -> Notice SwRate is in gr/m2/hour
  } else SwRate = 0;

  HeatState = HeatBalanceState((int)Ereq);

  if (OpMode == HEATBALANCE) { // Values have changed -> translate to Airflow intensity
    ActualUpperFanPerc = (uint16_t)(AirFlowToFanPercFactor[BikePos]*Vair + 0.5); // conversion from outcome Heat-Balance-Equation to percentage Fan intensity
    ActualUpperFanPerc = constrain(ActualUpperFanPerc, 0, 100);
    AdjustForFanBalance();
    SetFanThresholds();
    SetBothFanPeriods(); // Translate Perc to Microseconds Duty cycle Fan operation
  }
}
```
# Bike Position
```C++
// Variables that account for BIKE POSITION
#define UP   (0U) // Upright Bike Position
#define DP   (1U) // Dropped Bike Position (straight arms!)
#define TTP  (2U) // Time-Trial Bike Position
const char* BikePos_str[] = { "UP", "DP", "TTP" };
uint16_t BikePos = UP; // Default set to UP
```
<img src="../main/images/Bike_Positions.png" width="668" height="214" ALIGN="right" alt="Bike position" >
The frontal area of a cyclist meets most of the airflow that the fan(s) generate and that area is dependent on the bike position. In the algorithm the bike position is taken into account when calculating the ideal airflow for exchanging heat with the environment. The user can set/change the preferred position at any time with the help of the Airflow Companion App (<b>Algorithmic Settings</b>).<br clear="right">

# Matching V<sub>air</sub> and Fan Capacity<br>
Airflow speed of the impellor of the fan is linear proportional to the rotation frequency. According to the First Fan Affinity Law: Volumetric air flow is proportional to RPM (Impellor Rotations Per Minute). <br>
* Higher impellor frequency results in proportional higher air flow speed. Just <b>ONE</b> multiplying factor (FUP) is therefore appropriate for the whole range of fan operation!
* Measurement with a handheld anemometer showed that the applied fan generates, at max capacity, an airflow velocity of about 30 km/hour! NOTICE: This is fully dependent of the Fan's mechanical properties, size, power, manufacturer, etcetera and most likely shall be different in your case! <br>

The fans are set (by the software) from 0% - 100% duty cycle (no flow - max flow). To map the fan capacity percentage to the requested (Heat Balanced) Airflow (<b>V<sub>air</sub></b>) follow this rule: At a certain set of situational values (RH, air temperature, Gross Efficiency, cyling power induced, etcetera) the heat balance equation results in a requested Airflow of 8.32 m/s (3.6 * 8.32 = 30 km/h) and this should equal the very same <b>Fan Airflow Velocity</b>, in our case: max fan capacity (100%).
* When a requested airflow speed of 8.32 m/s is reached the fan(s) should operate close to 100% capacity --> therefore the multiplying factor (<b>FUP</b>) should be 100/8.32 = 12! In other words: (FUP * Requested Airflow) == (12 (No Dimension) * 8.32 (m/s)) = 100% of the max fan capacity (resulting in 30 km/h air velocity)
```C++
float Vair = 0.0; // Calculated requested AirFlow as a result of the Heat Balance equation

const float FUP = 12.0;  // The (constant) multiplying factor
// The AirFlowToFanPercFactor Conversion Factor can be derived taking into account the compensation for smaller sized frontal areas of the rider in
// the different bike positions with their appropriately increased factors!
const float AirFlowToFanPercFactor[] = {FUP,        // Upright Bike Position
                                       (FUP + 0.4), // Dropped Bike Position (straight arms!)
                                       (FUP + 0.8)};// Time-Trial Bike Position
#define MPS (1U) // Show Vair in meters per second m/s
#define KPH (2U) // Show Vair in kilometers per hour km/h
#define MPH (3U) // Show Vair in Miles per hour mph
const uint8_t AirSpeedUnit = KPH;
```
# OLED 128x64 Presentation Sequence
<img src="../main/images/OLED_Scr_1.jpg" width="400" height="400" ALIGN="right" alt="Oled scr collage 1" >
At start-up (power on) the user is informed about the BLE connection process of the <b>AIRFLOW</b> device with Heart Rate Monitor, Power Meter and Smart Phone.
On the top bar, icons are shown that indicate the (BLE or I2C) connection status of: Heart Rate Monitor, Power Meter, Smart Phone, Humidity and Temperature. During operation a sequence of (<b>11</b>) informative screens are shown on the Blue Oled display that detail the measured or calculated critical values that determine the Heat Balance Equation.<br clear="left">

```C++

// Create a SoftwareTimer that will drive our OLED display sequence.
SoftwareTimer RoundRobin; // Timer for OLED display show time
#define SCHEDULED_TIME 10000 // Time span to show an OLED Display screen in millis
int Scheduled = -1; // Counter for current OLED display screen, start to show SETTINGS Oled Display first !!
#define MAX_SCHEDULED 11 // Number of Oled Display screens to show sequentially -> Settings Screen (-1) does not count!
// Display Settings Sequence Show ("1") NoShow ("0")
char DisplaySettings[MAX_SCHEDULED + 1] = "00000000000"; // MAX_SCHEDULED display screen sequence --> all OFF
// -------------------------------------------------------

```
<br><br>
<img src="../main/images/OLED_Scr_2.jpg" width="400" height="400" ALIGN="right" alt="Oled scr collage 2" >
On the top bar, the double chevron to the left always indicates whether you <b>netto</b> gain (up) or loose (down) internal heat during the workout! Every 10 seconds the content of a new screen in the sequence (of <b>11</b>) is shown. 2 Screens in the sequence (#1 and #9) change of data set during display. The user can switch the <b>11</b> screens to be shown in the sequence <b>on/off</b>, in accordance with his/her preference and at any time during operation with the help of the Airflow Companion App (<b>Display Settings</b>).
<br><br><br><br><br><br clear="left">

# Airflow Companion App <br>
<img src="../main/images/AIRFLOW_app.jpg" width="800" height="800" ALIGN="middle" alt="Companion app" > <br>
From the start of the project it was decided to develop an AIRFLOW Companion App that would allow changing settings and if all went wrong to allow a strict manual control of the fans.
I have little experience with App development and decided to build one (for Android) in the accessible environment of [MIT App Inventor 2](https://appinventor.mit.edu).<br>
+ Download the <b>MIT App Inventor</b> AIRFLOW Companion App code with extension *file*<b>.aia</b>
+ [Visit at AppInventor](https://appinventor.mit.edu), You can get started by clicking the orange "Create Apps!" button from any page on the website.
+ Get started and upload the AIRFLOW Companion App code. Since some time <b>MIT App Inventor</b> also supports the Apple platform, I have not tested the code for IOS use!
+ Or upload the AIRFLOW Companion App <b>APK</b> to your Android device directly and install the APK. Android will call this a security vulnerability!<br>
# Operation Flow in Setup<br>
+ At start-up (power-on) the AIRFLOW device starts scanning for appropriate BLE devices in the neighbourhood, first it tries to establish a HRM <b>and</b> Power meter connection! The OLED display shows the different steps, progress and details of the running process. Notice that the scanning time for devices is limited (to 30 seconds) and have HRM and Power activated before you start the AIRFLOW device!
+ Now start the Companion app and have it scanning for the AIRFLOW device!
+ For a limited time period the AIRFLOW device advertises for a smartphone! Have the Airflow Companion App running and actively looking for the Airflow device. 
+ If detected, the Airflow Companion App establishes a connection over BLE (with the AIRFLOW device). From that moment on the Nordic UART service (a.k.a. BLEUART) for exchange of information is applied. A simple dedicated protocol was implemented that allows for bidirectional exchange of short strings containing diagnostic messages and/or operation settings between smartphone and AIRFLOW device.
+ After connection is established, the Airflow device sends the latest (persistent) settings data to allow the Companion App user to assess the current, previously user set or default, values.
+ If something went wrong, simply reset (power off/on) the AIRFLOW device and the above procedure is repeated!
+ <b>Regular operation is now started. The AIRFLOW device controls the fans (within settings) using current HRM and Power data.</b>
+ Whenever the user changes the current settings or control data during operation, the Airflow Companion App sends these to the Airflow device to immediately apply them. <br>
# Critical Power on the fly
<img src="../main/images/CP_W_Prime_Oled.png" width="147" height="147" ALIGN="left" alt="Companion app" >
In addition to an ideal airflow velocity for an optimally thermoregulated human body, the <b>AIRFLOW</b> user can get (as a bonus) insight in the development of his/her <b>Critical Power</b> when training intensity is intense and long enough! The applied <b>Arduino nRF52480 Express</b> CPU is so powerfull that Real Time calculation of <b>CP</b> and <b>W Prime</b> can be accomplished aside all the calculations for determining the Heat Balance terms and setting the fans to the appropriate blowing capacity. Setting on/off this functionality and/or changing your starting <b>CP</b> and <b>W Prime</b> values are also an integral part of the AIRFLOW Companion app! 
If you want to know in more detail about the science, math and implementation...<br clear="right">

[See RT-Critical-Power](https://github.com/Berg0162/RT-Critical-Power)
