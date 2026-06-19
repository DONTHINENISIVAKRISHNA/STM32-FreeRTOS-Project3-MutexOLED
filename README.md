# STM32-FreeRTOS-Project3-MutexOLED
Mutex as shared resource-SSD1306 OLED + FreeRTOS Scheduler on STM32 Nucleo-F103RB
I implemented, a real time embedded system, where two freeRTOS tasks share one OLED display safely using a Mutex.
so,here we are doing two tasks, using FreeRTOS Mutex Function, one is display Task, and other one is Counter Task, here both tasks share one I2C bus and one variable in RAM, so we have to observe here that two tasks are using one resource, this may creates a conflict between them, so to protect the resource, we are using mutex function , Mutex ensures only one task accesses the shared resource at a time.
Display Task- reads counter- writes to OLED - every 500ms
Counter Task- increments counter in RAM - every 1000 ms
using STM32CUBEMX, STM32IDE, using ssd1306 library, these are part of the software side, and the STM32Nucleo-F103RB, SSD1306 OLED, PC for running and seeing the visuals in putty terminal, so it gives a real understanding of how Embedded systems working , managing tasks smoothly in our daily applications, such as Aerospace, Medical , Automotive industries. for the application in Aerospace, they manage Flight computer navigation + telemetery + sensors tasks simultaneously. 
For more details about this project , see the attached pdf to this repository.
