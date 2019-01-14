# Complete-Fire-Red-Upgrade
A complete upgrade for Fire Red, including an upgraded Battle Engine.

As of right now, the code uses many graphical assests only found in Unbound, and thus
will likely crash if certain features are used in a rom that is not Pokemon Unbound.
The only feature that I've personally tested in a rom that was not Pokemon Unbound,
is the new EXP system. Multi Battles will likely work as well, although they have
not been tested on other roms.

Personalized options are available in src/config.h. The options can be customized
by commenting and uncommenting lines.

Installation Instructions:

1. Download devkitpro. Follow the instructions.
(Note: you can only install devkitARM)

2. Download the latest version of python (3.7.2).
After downloading and before proceeding to install make sure that the 'add to path' 
checkbox is ticked, otherwise you'll have to add the python path in the environment 
variables manually.

3. Download the master folder from this github page
(click 'Clone or Download', then 'Download Zip')

4. Get your ROM, rename it Pokemon Unbound.gba (this will be changed eventually) and 
place it the main (master) folder.

5. To decide the offsets where you want to insert the code:

a) In the 'insert.py' file in the folder 'scripts' change OFFSET_TO_PUT=YYY to the location 
   you want to insert the data (let it be X). Do this as well in the file 'make.py'.
  
b) In the 'linker.ld' file change ORIGIN = (0x08000000 +YYY) ORIGIN = (0x08000000 + X)

6. Run cmd.exe in the main folder. You can do this by typing 'cmd' and hitting enter in the 
url address or selecting 'run command prompt from here' from right clciking on empty space 
while holding the shift key.

7. In command prompt window, do one of the following:

a) Type 'python scripts//build.py', press Enter and then 'python scripts//insert.py' (here we 
   output a text file containing a list of all the offsets), press Enter.

b) Type 'python scripts//make.py'
  
A new gba file will appear named as test.gba and an offsets.ini file.
That is your resultant file.
