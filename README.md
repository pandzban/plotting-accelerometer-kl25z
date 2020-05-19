# plotting-accelerometer-kl25z
Plotting three axes of accelerometer 3D built in board NPX kl25z using Python script.

Made as project for intermodular communication course.

It uses microcontrollers I2C module to configure and read data of three axes from sensor MMA8451Q built in that board.
The data is accumulated and averaged or send further over UART and OpenSDA port to COM port on PC.
The second part of project is receiving this data in Python using Pyserial and plotting it in real time using Pyplot.
The script and embedded program allows for changing configurations of sensor, as well as configuration of onboard averaging and pausing work of program and module. 

How to communicate between PC and MC using UART over USB, you can see below:
![Transmission between MC and PC over UART standard](https://img.techpowerup.org/200519/infografika.png)


Here is short video presenting interface of our program on the PC side:
[![Plotting accelerometer kl25z](https://img.techpowerup.org/200518/scr.png)](https://www.youtube.com/watch?v=m-bJwO0NVRs)
