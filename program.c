#pragma config(Sensor, in1,    pot,            sensorPotentiometer)
#pragma config(Sensor, in2,    lightSensor,    sensorReflection)
#pragma config(Sensor, dgtl1,  btn1,           sensorTouch)
#pragma config(Sensor, dgtl2,  btn2,           sensorTouch)
#pragma config(Sensor, dgtl3,  btn3,           sensorTouch)
#pragma config(Sensor, dgtl4,  sonar,          sensorSONAR_mm)
#pragma config(Sensor, dgtl6,  led1,           sensorLEDtoVCC)
#pragma config(Sensor, dgtl7,  led2,           sensorLEDtoVCC)
#pragma config(Sensor, dgtl8,  led3,           sensorLEDtoVCC)
#pragma config(Motor,  port2,           liftMotor,     tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//VARIABLES
int currentFloor = 1; //floor elevator is currently in
int queue[3];	//create an array "queue" of size 3
int target;	//represents the floor elevator is headed to
bool vbtn1;	//variable checking if first floor is already in the queue
bool vbtn2;	//variable checking if second floor is already in the queue
bool vbtn3;	//variable checking if third floor is already in the queue

//QUEUE FUNCTIONS AND PROCESS
bool queueContains(int n)	//checks if the queue contains a floor
{
	for (int i = 0; i < sizeof(queue)/sizeof(int); i++) if (queue[i]==n) return true;	//if floor is in the queue, return true
	return false;	//else return false
}

bool addToQueue(int flr)	//function that adds floors to queue
{
	if (currentFloor != flr && !queueContains(flr)) //if the elevator isn't already on that floor or queued to go to that floor
	{
		for (int i = 0; i < sizeof(queue)/sizeof(int); i++) //check each value in the queue
		{
			if (queue[i]==0)	//if index of array queue is 0
			{
				queue[i]=flr; //add the floor to the first "empty" location in the array
				return true; //don't check any other locations, return that the floor was added
			}
		}
	}
	return false; //return that the floor wasn't added
}

task changeLEDs()	//task that changes LEDs according to the height
{
	while(1)	//change LEDs when elevator changes floors, by height
	{
		int height = SensorValue(sonar);
		if(height > 0 && height < 90)	//first floor
		{
			SensorValue[led1] = 1;	//LED for first floor is on
			SensorValue[led2] = 0;	//LED for second floor is off
			SensorValue[led3] = 0;	//LED for third floor is off
		}

		else if(height > 90 && height <150)	//second floor
		{
			SensorValue[led1] = 0;	//LED for first floor is off
			SensorValue[led2] = 1;	//LED for second floor is on
			SensorValue[led3] = 0;	//LED for third floor is off
		}

		else if(height > 150 && height < 210)	//third floor
		{
			SensorValue[led1] = 0;	//LED for first floor is off
			SensorValue[led2] = 0;	//LED for second floor is off
			SensorValue[led3] = 1;	//LED for third floor is on
		}
	}
}

task queueManager()	//task that manages queue array
{
	while(1) //poll the states of the 3 buttons, and if one is pressed add its floor to the queue
	{
		if(SensorValue(btn1) && !vbtn1) //on the rising edge of button 1
		{
			vbtn1 = true;	//make vbtn1 true, indicating that floor 1 is already in the queue
			addToQueue(1);	//add floor 1 to the queue
			clearTimer(T1); //clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn1)) vbtn1 = false;	//if button 1 is not pressed, vbtn1 = false

		if(SensorValue(btn2) && !vbtn2) //on the rising edge of button 2
		{
			vbtn2 = true;	//make vbtn2 true, indicating that floor 2 is already in the queue
			addToQueue(2);	//add floor 2 to the queue
			clearTimer(T1);	//clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn2)) vbtn2 = false;	//if button 2 is not pressed, vbtn1 = false

		if(SensorValue(btn3) && !vbtn3) //on the rising edge of button 3
		{
			vbtn3 = true;	//make vbtn3 true, indicating that floor 3 is already in the queue
			addToQueue(3);	//add floor 3 to the queue
			clearTimer(T1);	//clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn3)) vbtn3 = false;	//if button 3 is not pressed, vbtn1 = false

		wait1Msec(100); //debounce buttons
	}
}

//MAIN TASK AND PROCESS
task main()	//main task
{
	startTask(queueManager);	//start task "queueManager"
	startTask(changeLEDs);	//start task "changeLEDs"
	currentFloor = 0;	//set currentFloor to 0
	addToQueue(1);	//add floor 1 to queue, making the elevator start at floor 1
	while(1) //add elevator control code here
	{
		if(time1(T1)>10000) addToQueue(1);	//if timer, clearing when button is pressed, reaches 10 seconds, go to 1st floor
		if(queue[0] != 0)	//when there is a queue
		{
			currentFloor = queue[0];	//make currentFloor equal to first queue
			target = queue[0]*60;	//target floor is at height of desired floor times 60 mm
			if (SensorValue(sonar) < target)	//if current floor is lower than desired floor, go up
			{
				motor[liftMotor] = 127;	//goes up
				while(SensorValue(sonar) < target){}	//while the sonar value is less than the target height
				motor[liftMotor] = -10;	//goes the other way for .1 seconds to exactly stop
				wait1Msec(100);	//for .1 seconds
				motor[liftMotor] = 0;	//stop
			}
			else if (SensorValue(sonar) > target)	//if current floor is higher than desired floor, go down
			{
				motor[liftMotor] = -127;	//go down
				while(SensorValue(sonar) > target){}	//while the sonar value is greater than the target height
				motor[liftMotor] = 10;	//goes the other way for .1 seconds to exactly stop
				wait1Msec(100);	//for .1 seconds
				motor[liftMotor] = 0;	//stop
			}
			queue[0] = queue[1];	//removes first queue as it is used, moves the next queue to the first queue
			queue[1] = queue[2];	//moves the third queue to the second queue
			queue[2] = 0;	//third place of the queue is empty(0)
		}
	}
}
