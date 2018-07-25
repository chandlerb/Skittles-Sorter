# Skittles-Sorter

Task:

Design and develop your own machine (built using laser cut and/or 3D printed materials) that sorts a standard bag of skittles into 5 different bins by color (purple, yellow, green, orange, red, blue). The machine must be able to sort a standard (vending machine sized) bag of skittles in under 3 minutes. Your machine should be able to work immediately (i.e. once plugged in, or immediately after pressing a start" button), and should work without human intervention.

Materials:
Item:
Quantity
Stepper Motor
1
Servo
1
4in bolts
12
Nuts 
50+
2in screws
7
Color Sensor
1
** Access to Laser cutter **
 ** Access to 3D printer **
	
Procedure:

1.	Begin design and components for overall structure and completion of entire machine. 
a.	Feeder, in our case rotary feeder. 
b.	Color sensing, our case part of rotary feeder. 
c.	Distributing, our funnel cup to curved distribution piece.
d.	Collection, tubes from our distribution piece to our cups. 
2.	Iteratively design and create different parts for machine. 
a.	Get skittles to run through the machine and fall into a designated place, no need to sort skittles at this point is necessary. 
3.	Alter given code to begin the process to sort skittles. 
a.	Find rotation difference for rotary feeder to find the correct spot where the skittle can be read in from the color sensor 
i.	Get readings from the color sensor, and normalize the color of the skittles for your specific environment, change the averages in the given code. 
b.	Get the feeding mechanism to work in unison with the rotation of the rotary feeder to feed the correct skittles to the right input slot. 
c.	Continually check and renormalize the colors for skittles as they will change with multiple iterations of machine change. 

Software Design:

For the software, most of the code was given to us. But we had to add code for the servo and the stepper. For the stepper, we had to add code that makes the color sensor sense when a skittle is over the sensor. For the servo, the code moved the servo so that the skittle would drop into the appropriate tube. Another aspect of the code that we had to change was the calibration of the colors. In order to do that we had to get readings from a certain number of skittles and then take the average of the red, blue, and yellow values that the sensor gave us.

IMPLIMENTATION/EXECUTION:

1.	Once all preliminary steps are completed within procedure you may begin the construction of the machine. 
2.	Once the machine is completed, testing on the skittles is necessary to normalize the environment and the colors the color sensor is receiving. 
3.	Begin to progressively adding more parts to the sorter such as the color feeder. 
4.	Once the sorter is completed and works you may complete the following project with the following step. 
To start the project completed, the feeding board in the rotary feeder must be set to a known location so that the skittles will stop on-top of the color sensor and get a correct reading. Once centered plug in the Arduino and after a short delay the rotary feeder will begin to spin. At that point, it is alright to pour a bag of skittles into the designated collection point. If project is completed properly it should sort the skittles correctly. 

Results/Conclusion:

The final project was a working Skittles Sorter, that once fed a bag of skittles would wort the color and place the correct skittles into the designated cup. 

Difficulty’s/Issues:

1.	One difficulty that we had was the color sensor calibration was always slightly different depending on the lighting, even though where our color sensor was is as close to enclosed as we could get. 
2.	Another issue that we had is the servo would move to quickly and drag skittles to the wrong tube. To fix that we could have added a longer delay giving the skittles time to drop into the correct tube.

Distribution:

Kylee: 3D printing/Color Sensing 
Chandler: Laser cutting/Servo/Stepper coding
However, both teammates also contributed on all parts of the project. 
