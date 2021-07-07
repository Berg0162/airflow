# <img src="../main/images/AF_logo.png" width="64" height="64" alt="Airflow Icon"> &nbsp; Airflow

Thermoregulated Airflow Cooling for Indoor Cycling

etctera...

Work in progress...
# Electronic Components and Circuitry<br>
<img src="../main/images/AF_circuitry.png" width="900" height="500" ALIGN="middle" alt="Circuitry" > <br>
# PWM <br>

The AC Dimmer Module by Robotdyn is designed to control Alternating Current/voltage (110/220V). It can transfer control up to 400V/8А. In most cases, the AC Dimmer Module is used to turn the power ON/OFF or dim lamps or heating elements, it can also be used for fans, pumps, air cleaners, etcetera. A major benefit of the Robotdyn board is that the 110/220V part is (optically) isolated from the 5V logical control part, to minimize the possibility of high voltage damage of the attached low voltage microcontroller. The logical level of the Dimmer Module is tolerant to 5V and 3.3V, therefore it can be connected to a microcontroller with 5V and 3.3V level logic. <br>
<img src="../main/images/Duty_Cycle_animation.gif" width="200" height="160" ALIGN="right" alt="Duty Cycle" >
For our Arduino Feather nRF52 Express a dedicated Timer/Interrupt handling library has been applied, which allows external interrupts and process time interrupts to switch AC power ON/OFF to enable PWM on the fans. The AC Dimmer is connected to the Arduino Feather nRF52 board via two digital pins. One pin (interrupt) to detect the Zero Crossing of the 50 Hz AC Phase, which is used to initiate an interrupt signal to start the cut-off of the cycle. A second pin (PWM) to control the width (or time) of the cut-off duration of the cycle, which determines how long the current is ON/OFF.<br clear="left">

