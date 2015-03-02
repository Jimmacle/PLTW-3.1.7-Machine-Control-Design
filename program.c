#pragma config(Sensor, in1,    pot,            sensorPotentiometer)
#pragma config(Sensor, in2,    lightSensor,    sensorReflection)
#pragma config(Sensor, dgtl1,  btn1,           sensorTouch)
#pragma config(Sensor, dgtl2,  btn2,           sensorTouch)
#pragma config(Sensor, dgtl3,  btn3,           sensorTouch)
#pragma config(Sensor, dgtl4,  sonar,          sensorSONAR_mm)
#pragma config(Sensor, dgtl6,  led1,           sensorLEDtoVCC)
#pragma config(Sensor, dgtl7,  led2,           sensorLEDtoVCC)
#pragma config(Sensor, dgtl8,  led3,           sensorLEDtoVCC)
#pragma config(Motor,  port2,           liftMotor,     tmotorVex393_MC29, openLoop)

int currentFloor = 1;
int queue[3];
bool vbtn1;
bool vbtn2;
bool vbtn3;

//QUEUE FUNCTIONS AND PROCESS
bool queueContains(int n)
{
	for (int i = 0; i < sizeof(queue)/sizeof(int); i++) if (queue[i]==n) return true;
	return false;
}

bool addToQueue(int flr)
{
	if (currentFloor != flr && !queueContains(flr)) //if the elevator isn't already on that floor or queued to go to that floor
	{
		for (int i = 0; i < sizeof(queue)/sizeof(int); i++) //check each value in the queue
		{
			if (queue[i]==0)
			{
				queue[i]=flr; //add the floor to the first "empty" location in the array
				return true; //don't check any other locations, return that the floor was added
			}
		}
	}
	return false; //return that the floor wasn't added
}

void changeLEDs()
{
	SensorValue[led1] = 0;
	SensorValue[led2] = 0;
	SensorValue[led3] = 0;
	switch (currentFloor)
	{
	case 1:
		SensorValue[led1] = 1;
		break;

	case 2:
		SensorValue[led2] = 1;
		break;

	case 3:
		SensorValue[led3] = 1;
		break;
	}
}

void updateQueues()	//push queues up when a queue is used already
{
	int qFlr = queue[0];
	switch(qFlr)
	{
		case 1:
			vbtn1 = false;
			break;

		case 2:
			vbtn2 = false;
			break;

		case 3:
			vbtn3 = false;
			break;
	}

	queue[0] = queue[1];
	queue[1] = queue[2];
	queue[2] = NULL;
}

task queueManager()
{
	while(1) //poll the states of the 3 buttons, and if one is pressed add its floor to the queue
	{
		if(SensorValue(btn1) && !vbtn1) //on the rising edge of button 1
		{
			vbtn1 = true;
			addToQueue(1);
			clearTimer(T1); //clears timer every time button is pressed
		}
		else if(!SensorValue(btn1)) vbtn1 = false;

		if(SensorValue(btn2) && !vbtn2) //on the rising edge of button 2
		{
			vbtn2 = true;
			addToQueue(2);
			clearTimer(T1);
		}
		else if(!SensorValue(btn2)) vbtn2 = false;

		if(SensorValue(btn3) && !vbtn3) //on the rising edge of button 3
		{
			vbtn3 = true;
			addToQueue(3);
			clearTimer(T1);
		}
		else if(!SensorValue(btn3)) vbtn3 = false;

		wait1Msec(100); //debounce buttons
	}
}

//ELEVATOR FUNCTIONS AND PROCESS
void moveToFloor(int flr)
{
	if(flr>0)
	{
		//sonar: 	|1st floor	|2nd floor	|3rd floor
		//(about)	|0mm				|90mm				|180mm
		int dBtwFlr = 90;	//distance between floors
		int distance = dBtwFlr*(flr-1);	//distance to desired floor

		if(currentFloor<flr)
		{
			while(SensorValue[sonar]<distance)
				motor[liftMotor] = -127; //until sonar reaches distance
			motor[liftMotor] = 0;
			currentFloor = flr;
		}
		else if(currentFloor>flr)
		{
			while(SensorValue[sonar]>distance)
				motor[liftMotor] = 127; //until sonar reaches distance
			motor[liftMotor] = 0;
			currentFloor = flr;
		}
		else
			motor[liftMotor] = 0;
	}
}

void safetyMech()
{
	if(time1(T1)>20000)//when timer hits 20 seconds
		moveToFloor(1);
}

//MAIN TASK AND PROCESS
task main()
{
	startTask(queueManager);

	while(1) //add elevator control code here
	{
		int qFloor = queue[0];	//define first floor to go to

		changeLEDs();
		moveToFloor(qFloor);	//move to first floor in queue
		updateQueues();	//update queues
		safetyMech();
	}
}
