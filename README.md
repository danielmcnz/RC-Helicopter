# Remote Control Heliopter Software

Software for a RC Helicopter controlled using an STM32 and a Cortex M4 Microntroller.
The software was programmed using Code Composer Studio in order to upload the code to a Cortex M4 MCU.
The software controls the altitude and yaw using the Cortex M4 with a custom board with buttons as the controller. 
A landing and take-off sequence is designed for smooth flight and landing. 
Figure 1 shows the helicopter in the remote labs website used for the project.

<p align="center">
  <img src="https://github.com/danielmcnz/RC-Helicopter/assets/24379868/102f055f-c651-4405-a279-cf100f3adfc9" />
  
  <p align="center">Figure 1: Remote lab for uploading and controlling a helicopter</p>
</p>

## Controls
- A switch is used to indicate whether the helicopter should take-off and fly, or land. 
- Two buttons are used for altitude control.
- A further two buttons are used for yaw control.

## Sequence of Operations
- When turned on, the take-off sequence takes the helicopter into a hovering state, waiting further input.
- Once the take-off sequence has been activated, the yaw and altitude can be controlled with the provided buttons.
Each yaw change moves the helicopter by 15 degrees in either direction, while the altitude changes by 10% of the max altitude with each change.
- After flying, the helicopter can be landed by changing the switch, and this will automatically move the helicopter to a reference point, land, and power off the motors.

## Features
- Time-based Scheduler (kernel) for timing required tasks
- OLED display on Cortex MCU and Serial output when using the remote lab (Figure 1)
- PID control for the altitude and yaw movement
- PWM control for main and tail rotors
- Finite State Machine used to determine the current state of the helicopter
