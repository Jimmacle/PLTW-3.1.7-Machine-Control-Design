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

int currentFloor;
int queue[3];
bool vbtn1;
bool vbtn2;
bool vbtn3;

bool queueContains(int n)
{
	for (int i = 0; i < sizeof(queue)/sizeof(int); i++) if (queue[i]==n) return true;
	return false;
}


task queueManager()
{
	while(1)
	{
		if(SensorValue(btn1) && !vbtn1) //on the rising edge of button 1
		{
			vbtn1 = true;
			if (currentFloor != 1 && !queueContains(1)) //if the elevator isn't already on that floor
			{
				for (int i = 0; i < sizeof(queue)/sizeof(int); i++) //check each value in the queue
				{
					if (queue[i]==0)
					{
						queue[i]=1; //add the floor to the first "empty" location in the array
						break; //don't check any other values
					}
				}
			}
		}
		else
		{
			if(!SensorValue(btn1)) vbtn1 = false;
		}

		if(SensorValue(btn2) && !vbtn2)
		{
			vbtn2 = true;
			if (currentFloor != 2 && !queueContains(2))
			{
				for (int i = 0; i < sizeof(queue)/sizeof(int); i++)
				{
					if (queue[i]==0)
					{
						queue[i]=2;
						break;
					}
				}
			}
		}
		else
		{
			if(!SensorValue(btn2)) vbtn2 = false;
		}

		if(SensorValue(btn3) && !vbtn3)
		{
			vbtn3 = true;
			if (currentFloor != 3 && !queueContains(3))
			{
				for (int i = 0; i < sizeof(queue)/sizeof(int); i++)
				{
					if (queue[i]==0)
					{
						queue[i]=3;
						break;
					}
				}
			}
		}
		else
		{
			if(!SensorValue(btn3)) vbtn3 = false;
		}

		wait1Msec(100);
	}
}

task elevatorMover()
{

}

task main()
{
	startTask(queueManager);
	startTask(elevatorMover);
	while(1){}

}
