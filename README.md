# Light-Control-ESP8266

This project has 3 components- light control, audio input, and IoT integration.

The light control and audio input are handled by the PIC24 Microcontroller as a part of my final project for EE 2361. The PIC uses SPI to change the setting of digital potentiometers (MCP41010) with the output of the pots tied to a non-addressable LED strip with a common ground. 

The audio input is also handled by the PIC via a basic microphone. The PIC performs Analog to Digital Conversion on the signal it receives from the microphone and finds the RMS voltage of the input, which roughly corresponds to volume. Ideally, I would've liked to use the Fast Fourrier Transform (FFT) to determine the frequency of the detected sound but I didn't have the time to do so for my project. The RMS value is then added to the value being "written" to the digital pots and the light flashes brighter according to the volume.

For IoT integration, the ESP8266 hosts a web page, allowing a user to enter the URL access it. The page shows a couple buttons which toggle functionality of the light. These include a "wheel" function, which smoothly rotates through the color wheel, a "boost" function which enables sound reactivity, and more.
