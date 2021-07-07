# Arduino-HTML-GUI-FOR-I2C-Devices

![Photo_demo_GUI](QN8027GUI.jpg?raw=true)

1. What it is for?
------------------
its for quickly testing I2C devices on arduino. + with GUI interface + without any library
this tool will be most useful when you are planning to write a library by just using its datasheet(which has its register map)
 
2. How to setup:
----------------
on arduino: forget everything and just install given code.
on PC: just double click and open host.html in chrome. + enable Serial api in chrome by going to chrome://falgs in address bar.

3. Then?
---------- 
then connect your chip to arduino on A4,A5 pins, then connect arduino to PC. and in host.html, click on connect and select COM port for arduino.
then create a text file with a format which is explained below. open that file in host.html and you will see a GUI interface which will controll your chip!

4. text file Format:
----------------------
currently, you can add five types of elements on you GUI.
1. checkbox - for toggling a single bit.
2. combo	- for selecting multiple options(setting multiple bits)
3. text box - for setting entire register of chip (6,7 or 8 bits).
4. frame    - to arrange all of these 3 elements in categories.

you can quickly checkout uses of this format in a file given with this source code(named QN8027.txt) which can controll QN8027 FM transmitter chip.

1. checkbox: format of checkbox is as following-
CB ! caption ! id ! defaultvalue ! javascript code to execute.

for example-
CB!Power On!0!0!{tb(e,'0x2C','0x00','0b00100000');}

here CB stands for checkbox.
exclaimation mark is used as primary saprator everywhere.

caption is the caption of this checkbox.
id is 0 which means no id. otherwise you can provide any text as id. so that you can enable/disable or access its value from other element.
next 0 is defaultvalue, 0 means unchecked. 1 mean checked. that means when GUI will be created, this checkbox will appear unchecked.

last item is script that will run when you click checkbox. this script will run both times(when you check or uncheck).
script in this example uses a function named 'tb' which means togglebit. which is a single all in one function for checkboxes.
it takes 3 parameters. 1. I2C deviceID 2. register ID, 3. mask.
so here is 0x2C is device address of QN8027.
0x00 is register where powering on/off functionality resides.
mask indicates the bit number which is responsible for this functionality.
in this mask, bit number 6 from right is responsible for power on/off.

so in this example, you can see how its very easy to create such GUI just directly from chip's datasheet.

2. combo: format of combo is as following-
CB ! caption ! id ! defaultvalue !options1:chipvalues1|options2:chipvalue2! javascript code to execute.
nothing new, accept for 1 item- options1:chipvalues1|options2:chipvalue2
this will create combo items. which will appear as option1,option2.... etc in GUI, but by selecting them, their actuall value chipvalue1,chipvalue2....will be sent
to the chip. the javascript code will be executed merely by selecting option.

for better understanding, look at this example -
SO!No Audio Auto Off!0!2!After 58sec:0|After 59s:1|After 60s:2|Never:3!{sosend(e,'0x2C','0x02','0b00110000');}

here as you can see, this line will create a combo with caption "No Audio Auto Off". with options, "After 58s" ,"After 59s","After 60s" and "Never".
when user selects first option("After 58s"), javascript code executes. this code uses this option's actuall value(which is 0).
as you can see, javascript code uses a single function named "sosend" which is all in one function for combo boxes. it automatically sets value of selected
option using provided mask to the chip.
it takes e as first parameter. which is fixed. which denotes 'this'(means element itself).

here also consider the default value, which is 2, that means 2+1 = 3rd option will appear as selected when GUI will be created.

so here again, you can see how easy its to write such GUI directly from datasheet.

3. textbox: format of textbox is same as checkboxes. here we will directly see an example-
TB!Frequency Deviation(0-148kHz)!0!75!{var cv = parseInt(e.value)/0.58; var chipval = parseInt(cv);sv('0x2C','0x11','0b11111111',chipval);}

as you can see, it has default value 75. but you can also see there is no all in one function for textboxes. instead it uses "sv", which stands for "set value".
its a predefined function which sets provided value to the chip. here you manually converts actual values to chip register values.

4. frame: frame only takes 3 parameters-
FR!Radio!width:25%;background-color:teal;color:white;margin:2px;padding:5px;border:1px solid black;border-radius:5px;display:inline-block;

which are - FR(keyword), Radio(caption) and css code for decoration.

5. HOW it works?
----------------
core functionality is made of arduino code and javascript.
javascript has a function named "writeToStream". which sends data to arduino.
on the other side, arduino uses serial.read() function to receive this data.

on the next simplicity level, javascript uses a function named 'sendToCOM'. which is capable of converting '0b' and '0x' type string to integer value.
and it can send 5 bytes at a time. first byte is function number. second is device address, third is register address, fourth is mask and last is value.
on the other side, received data is collected to a array named 'alldata[]'.
 and when all five bytes are received, it starts processing these bytes. it looks at first byte for command type and send rest four bytes to appropriate functions
 accordingly.
 before storing these bytes to array, it has to do some special. actully, from chrome, you cant send values above 127 directly. for example, when you send value 128
 it is received in two bytes , first is 194 and second is 0. arduino code combine these two byte into one to get actull value 128.
 
 on the next simplicity level, javascript uses built in functions that utilize sendToCOM function. which are -
 sv : stands for "set value"
 await gv: stands for "get value"
 tb : toggle bit. used for checkboxes. sets bit according to check boxe's checked status.
 sendTB: send toggle bit. which can be used for toggling bit whithout knowing its previous value.
 delay: to delay for some time.
 
 get value function actully sends needed parameters to arduino code for getting value. arduino code then sends that value back using Serial.println() function.
 but with a prefix ":RTN:". until then, javascript waits for returning value using waitForRTN() function. when this value is received back in logs, log filter 
 sets global variable and then this waitForRTN() function returns with received value.
 
 6. RESTRICTED values: these values are already used in this interpreter. so dont use them in you codes.
 ---------------------
	a. log : log as element id is already used for log window.
	b. !: this symbol is used for main list saprator.
	c. new line: it is used in script's line saprator.
	
to better understanding, open host.html and QN8027.txt with notepad and try to understand code.

7. TODO:
---------
there is a lot more to improve. currently it only supports 8 bit registers. that can easiely be extended to 16 bit registers.
it is also possible to support SPI protocol using this. but thats for future.

--Manoj Bhakar PCM--
	
