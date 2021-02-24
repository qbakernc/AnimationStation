# AnimationStation
Created a C# forms application that can control a 8x16 LED matrix. This application can be used to program animations on to the matrix graphically instead of writting code. This done by coding the microcontroller to accept and save the animation commands from the forms application and loop them once the application sends the start command. The application connects to the application using a bluetooth transmitter. Communication options have to be selected to connect to the receiver. There are four delay options that can be used in between each frame. 1000ms, 500ms, 200ms, and 100ms. The Latch button is used to save a frame and its delay time. 

What I learned
* Learned how to create a C# forms application and have it communicate with a microcontroller.
* Learned how to program a microcontroller to accept and save data from an outside source.

Component List
* CuteDigi Atmega 128D development board (https://cutedigi.com/cutedigi-atmel-mega128d-mini-development-board/)
* 8x8 LED Matrix x2 (https://www.adafruit.com/product/455)
* Max7219 Display Driver x2 (https://www.sparkfun.com/products/9622)
* SparkFun BlueSMIRF Silver bluetooth  receiver (https://www.sparkfun.com/products/12577)

Links  
[Video Demonstration](https://www.dropbox.com/s/wbgh0z3rslg0sky/AnimationStation.mp4?dl=0 "Animation Station")
