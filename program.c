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
task queueManager()
{
	while(1) //poll the states of the 3 buttons, and if one is pressed add its floor to the queue
	{
		if(SensorValue(btn1) && !vbtn1) //on the rising edge of button 1
		{
			vbtn1 = true;
			addToQueue(1);
		}
		else if(!SensorValue(btn1)) vbtn1 = false;

		if(SensorValue(btn2) && !vbtn2) //on the rising edge of button 2
		{
			vbtn2 = true;
			addToQueue(2);
		}
		else if(!SensorValue(btn2)) vbtn2 = false;

		if(SensorValue(btn3) && !vbtn3) //on the rising edge of button 3
		{
			vbtn3 = true;
			addToQueue(3);
		}
		else if(!SensorValue(btn3)) vbtn3 = false;

		wait1Msec(100); //debounce buttons
	}
}

//MAIN TASK AND PROCESS
task main()
{
	startTask(queueManager);
	while(1) //add elevator control code here
	{
		changeLEDs();

	}
}
