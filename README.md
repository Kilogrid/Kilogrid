# KiloGrid_IRIDIA

## Starting Everything 

### Starting the KiloGui

First you have to open the gui project. It is located at 

```
C:\kilogrid_software\kilogrid_software\src\KiloGUI
```

Then open kilogui (should be a Qt Project file). 
This should then open the Qt editor. 
There you have to press play (green triangle in bottom left corner).
(The version you should run is 32 bit debug.) 

This opens Kilobots Toolkit.


### Start an experiment

In the Kilobots Toolkit window:
Make sure big arena is selected. 

Select configuration:
Here you can select your .kconf for your expeiment. Normally located at

```
Experiments/<your_experiment>/<your_conf>.kconf
```

Upload Kilogrid Program:
Here you can select the controller for the kilogrid module. Normally it should be located at 

```
Experiments/<your_experiment>/module/build/<your_module>.hex
```

After selecting the files 
1. press bootload (kilogrid should blink blue)
2. press upload (kilogrid should blink green/red and after successfully uploading should breath in white)
3. press setup (should stop breathing)
4. press run (should run the experiment)
5. press stop to quit the experiment 

### setup the robots

While the kilogrid is running you cannot configure the robots, thus you have to do it before you start the kilogrid-experiment.
You can do the configuration of the robot after pressing setup (test if this is really the case!).

Upload Kilobot Program:
Here you can select the controller which should be executed on the kilobot. Normally located at

```
Experiments/<your_experiment>/Kilobot/build/<your_kilo>.hex
```

After file selection place the robots on the kilogrid (in the middle of the module - makes it quicker - the robot sees all 4 cells).

1. press bootloading - the robots and the kilogrid should light up blue
2. press upload - the kilogrid blinks blue green, if the kilobots blink green the upload was successful 
3. press reset (at Kilobot Commands) - this resets everything 
4. press run - robots start to run their controller 

After the controllers started you can run the kilogrid and start your experiment.


## Compiling Code 
This is an instruction on how to compile code on the Computer which is connected to the kilogrid.

First start the app "Ubuntu".

This opens a terminal.
From there you can go to your experiment.
Here you can see how to compile the Module controller (aka the controller for the kilogrid).

```
cd /mnt/c/kilogrid_software/kilogrid_software/src/Experiments/<your_experiment>/Module/
make all
```

Next you can compile the kilobot controller like

```
cd /mnt/c/kilogrid_software/kilogrid_software/src/Experiments/<your_experiment>/Kilobot/
make all
```

## Calibrating robots
In the window Kilobots Toolkit under Kilobot Commands, press Calibration. 
This opens a new window where you can calibrate the robots. 
This is done on a yellow field on the kilogrid (you can use the white stencil as a wall). 
Put one robot on the field and start calibrating. 

Sometimes other robots start to move, even if they are not on the yellow field. 
Either close and open calibration again or put them on a tray. 

## Creating a new project 
Copy a existing Project. 
Then you have to adjust the .kconf file, Kilobot/KiloMain (robot controller) and Module/ModuleMain the module controller.  

TODO



## Other usefull actions 
### Check battery status
In the Kilobots Toolkit window you can check the current status of charge by pressing voltage at Kilobot Commands > Voltage. Then the robots should light up somewhat purple/orange?? pressing it again shows than the battery status. 



## File description

### .kconf 
This files handles the initial configuration for the module/cells of the kilogrid.

The structure should look something like 

```
address
y:4

data 
0x1
0x2
0x3
0x4

```

This is one example building block. 
Also you can use different addressing like 

```
y:10-11
module:0-1
x:10-11
```

## Debugging 

### If modules stop working
If a module stops working you also cannot stop the experiment. 
What you have to do is, first start the other computer, logg in on littleboy, open a terminal:

```
cd Documents/SmartArena/src/CellBootloader/build
```

Then you have to connect this computer with the not working cell (use an usb-usb cable and the AVRISP tool).
If the connection works the AVRISP tool should light up green. 

Then execute 

```
sudo avrdude -p m328p -P usb -c avrispmkII -U "flash:w:CellBootloader.hex:i" -v 
```

(see https://diode.group.shef.ac.uk/kilobots/index.php/Kilobot_Firmware for command).

Then it should bootload/reset the module and it should work again. 

What can be tried is using the PC which is used for controlling the kilogrid, open Ubuntu there and do the same steps.

The bootfile file is at 

```
cd /mnt/c/kilogrid_software/kilogrid_software/src/module/build/module_bootloader.hex
sudo avrdude -p m328p -P usb -c avrispmkII -U "flash:w:module_bootloader.hex:i" -v
```

### For setting the Dispatcher
First, backing up the running code:
```
sudo avrdude -p m328p -P usb -c avrispmkII -U "flash:r:Dispatcher.hex:i" -v
```
This should download the current .hex file. 
This seems to work lol. 

Second, uploading should be then the same as to some module:
``` 
sudo avrdude -p m328p -P usb -c avrispmkII -U "flash:w:Dispatcher.hex:i" -v
```

Finally, the last important thing is how to compile the code aka create dispatcher.hex 
``` 
cd /mnt/c/kilogrid_software/kilogrid_software/src/dispatcher/
make all
```
This seems to work as well.

PAY ATTENTION THE FILES ARE CASE SENSITIVE!

## General advice for running experiments 

Do not touch the robots while they charge. First turn of the charging station, than wait for 30 sec, than touch them. Otherwise it could harm the robot.

Charge the robot if its battery is yellow or below yellow. 
- robots should be charged in sleep mode
- the charge level is: 
-- green (fully charged)
-- blue (fine)
-- yellow (charge!)
-- red (should not happen)
-- black = dead
- charge the robots for 3 hours (not over night)

Compile size of the code (kilogrid & robots) should not exceed the space, e.g., should be under 100%

turn on/off = plug jumper 

states
- sleep: blink every 8 s
- reset: should be green, when blinking green it does not receive a message
- ...

The robots need to run before the kilogrid, e.g., start the robot controller, than the kilogrid controller (probably you have to send a start/init msg to the robots)

First start the kilogrid, than the kilobot toolkit - otherwise you get a connection error (can be fixed by disconnecting and then reconnecting)

### Useful weblinks

https://diode.group.shef.ac.uk/kilobots/index.php/Kilobots

https://kilobotics.com/docs/index.html

Documentation about the MCP2515 (https://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf)
or check on the local machine under c:/Manuals/

