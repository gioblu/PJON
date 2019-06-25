# SeriousBricks Ecosystem

## Abstract
The SeriousBricks Ecosystem is a PJON based bus system that is designed to equip Lego models with more features.
Why Lego? When working with sensors and actuators soon there comes the need of placing motors or sensors into housings and  build some mechanics around. So at every occasion you want to prototype your ideas at one point it comes to think about: How do I connect my motors? So normally you would go and 3D print stuff, or even do some mechanics. But: This requires equipment that might not be available in every household. Lego to the rescue: It provides a vast variety of construction material, that can even be modified, glued, hollowed whatnot. It is easily available, cheap (well, kind of) and can be used to build your mechanics. And of course: Building Lego and equipping models with sensors and motors and stuff is fun of its own and was the original idea of starting this project. As the original Mindstorms bricks are very limited, bulky, expensive and I wanted to build smaller models I came with this idea:
Create Lego bricks that can be put into small Lego models (so called "Minifig scale") and make my Lego worlds actually do stuff.

## The Vision
In the end I want to create an Ecosystem that:
- Allows me to place sensors, motors, controllers everything inside Lego models without gluing.
- Is as small in size as possible
- Can as generic as possible
- Can be easily programmed
- Can run without a desktop computer
- Can still connect to a desktop computer
- is somewhat modular, so you can create different models and put them still together in one scenario

## Approach
The approach was to have some mC that would control a model (say a crane). That would spin up a bus system to all the motors and sensors that that model would need. Say 3 motors, some LEDs, some buttons. If all that stuff would be hooked up directly to the controller, what would happen: You will run out of GPIO pins. You will spoil your model with tons of wires cause most likely the controller will be sitting on the base of a model. So instead the approach is to decouple the low level control from the model controller. Each sensor or motor would have a controller of its own communication with the model controller. This gives the freedom of placing smaller bricks whereever needed and reduce wires. The model controller can be placed outside the actual model and would only feed 4 wires into the actual model. These would be VDD, VCC, GND and Signal.
The hardware used is for now an arduino mini pro as the model controller, and ATTiny85 mC for controlling the peripherals (I call them "SeriousBricks"). It is planned to replace the mini pro with an ESP8266 to have WiFi connectivity at hand. In the actual baseline the model controller runs a fixed program that is flashed onto it, but later I plan a communication to a desktop computer. This then will control whole scenarios (consisting of more cranes, or trains or whatever you might think of).

## Contents
The contents of the project fall into different parts: Model controller and "SeriousBricks controller". The model controller part consists of several example programms, that will look like the programms you would run your model (be it a Lego model or something completly different does not matter). To shield the developer from the burdon to know the nuts and bolts of the PJON framework, all of this is covered in a library I called "BrightBus". This is to be included to the model controllers program. The SeriousBricks controllers do not use that framework, but react to the PJON messages at low level.
For now you find these "SeriousBricks":
- RemoteTimer
- RemoteButton
- RemoteStepper
Soon to come:
- RemoteLED
- RemoteDCMotor
Planned:
- RemoteServo
- RemoteNeoPixel
- RemoteNFC

All these will consist of two parts: The model controller library that is to be included into your model controller and the client that is to be flashed to the ATTiny for the SeriousBrick.
So the directory is structured like
```
SeriousBricks
|- bin
|   |- SeriousBricksModel
|   |- SeriousBricksClientDependencies
|
|- Model
|   |- BrightBus
|   |- RemoteStepper
|   |- RemoteButton
|   |- RemoteTimer
|   |- examples
|       |- TimerSample
|       |- ButtonSample
|       |- StepperSample
|       |- AllInOne
|
|- Client
|   |- ATTinyTimer
|   |- ATTinyButton
|   |- ATTinyStepper
|        |- PJONMinimal
|        |- MinimalAccelStepper
|
|- Layouts
      |- StepperLayout
      |- TimerLayout
      |- ButtonLayout
```
The PJONMinimal and MinimalAccelStepper are needed to reduce the footprint. Otherwise the code would not fit into an ATTiny. As soon as footprint of PJON framework is reduced I hope to get rid of it.

## How to use
Extract SerialBricksModel and SeriousBricksClientDependencies so you can use it in your arduino IDE. Extract client code to whereever your project resides. Open the client program you want to use and modify the "myId" define. This is the deviceId that you will use when communication with this particular brick. Flash the modified code to an ATTiny. Find the PCB layout in the layouts section. Manufacture it however you like best. There are Fritzing layouts, Gerbers and SVG files for you to use. Solder ATTiny85 and crystal to the pcb, perhaps some Dupont connectors or whatever you might find appropriate.
Upload the example you want to use to your MiniPro. Change the deviceId when you create one of the RemoteXXX component to that deviceId you want to address.
Wireing: The wiring is a bit tricky at the moment, because nothing is labelled. Best use a breadboard first. From the model controller: Hook up GND from the MiniPro to one rail, hook up +5V to another. PJON communication is done using Pin11 on the arduino, so put that on a third rail. If you do not want to drive motors you are done. If you need something that drains more power: Connect your power supply to GND and the say 9V to an other one. Note: RemoteStepper needs at least 9V to function, even if you are not driving steppers that need 9V.
Connect the remotes: GND, +5V and Signal(and 9-12V, or whatever you like) go to the appropriate rail on your breadboard. Power it up and find the bugs, do not expect it will run in the first go :)

Licenses
AccelStepper is copyright by ... Kudos. Original code can be found here:
PJON framework is under ... license
