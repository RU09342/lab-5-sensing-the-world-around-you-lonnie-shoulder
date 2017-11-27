# Sensors and Signal Conditioning

## Sensing Light Intensity
In this lab 3 sensors are utilized, phototransistor, photodiode, and photoressistor. The phototransistor generates different currents based on different light intensities. The photodiode generates different voltages based on the light intensities, and photoressistors generate different resistances based on the the light intensity. The circuits for these sensors can be seen bellow.

To read this data into your microcontroller, we must use the ADC. To make sure that the voltage is not too high or too low it can be amplified and then it will be passed into the ADC. The ADC works by comparing the voltage on the input pin to a voltage of 3.3 volts. The percision of the ADC is
![equation](http://latex.codecogs.com/gif.latex?%5Cfrac%7B3.3%7D%7B2%5E%7Bx%7D%7D) volts. 
So in a 12 bit ADC each bit is 0.000805 volts and in a 10 bit ADC each bit is 0.0032 volts.





# Sensors and Signal Conditioning
One of the biggest limitations of the microcontroller is the fact that it lives in a digital world. We need certain components to be able to translate the analog world to the digital world and vice-versa. In this lab, we need to be able to convert some electrical phenomena into something our microcontroller can understand. For this, we need a few building blocks to do this.

## Sensors
Sensors in a nutshell convert a physical phenomena into an electrical phenomena. For example, and accelerometer converts an acceleration into a change in capacitance (in the case of some MEMS devices). This electrical phenomena does not have to be a voltage, and as such, we need to be able to convert our sensor output to a voltage. From there, we can read in this voltage through our Analog to Digital Converter (ADC).

## Signal Conditioning
The signal conditioning chain can be broken up into a few main building blocks. In the beginning you have your sensor which converts something into the electrical world. We then need a block which can convert that resultant electrical phenomena into a voltage. From here, this voltage may be too small/large for our microcontroller, so we need to amplify the signal. From there, we may need to de-noise the signal by filtering. After all of this, we might be at a point where we are ready to read the signal into our processor.

## Task
For this part of the lab, you need to focus on two main aspects: utilizing and reading the ADC, and conditioning a sensor to provide you with a decent output. To do this, you will need to build the proper circuitry to take a measurement from sensors which convert a physical phenomena into:
* Voltage
* Current
* Resistance

## Deliverables

### Code
Your code for this section should focus heavily on the ADC and being able to pull data from it. Your code need to communicate back to your computer using UART at 9600 Baud and send the ADC contents at 1 reading per second to start. This really is meant for you to see whether or not your signal conditioning is actually working and you can see changes in your sensor reading. This code should be very very similar to code you have written before and should be simple to port between your processors.

### Hardware
The hardware portion should be the same for each of the processors. You will need to have a total of 3 circuits made, each corresponding to a different sensor. You need to look at the range of measurements and the amount of resolution you can get out of your ADC and determine how to convert, scale, and filter your signal. Don't forget the fact that you will need to convert to a voltage, making some of these circuits not so trivial. The main goal as the hardware designer for these sensors is to provide the microprocessor with the best resolution to maximize the effectiveness of the ADC.

### README
The README for this part of the lab should talk briefly about how the ADC code works, but focus way more on the hardware aspect. You need to include schematics of your circuits, and well as supporting simulation/calculations which show that your circuits should work. You should talk about what types of circuits you are using and how they actually work.