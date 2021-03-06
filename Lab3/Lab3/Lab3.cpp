// Lab3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <string.h>

const double Fs = 1000.0;
const double PI = 3.14159265359;

double eventResponse[1000];
double amplitude = 1.2;
double fo = 20.0;
double alpha = 10.0;

// Save event data
std::string eventFileName;
std::ofstream eventOFS;

int main()
{
	std::cout << "Running..." << std::endl;

	eventFileName = "../../EventData.csv";
	eventOFS.open(eventFileName);

	for (int i = 0; i < Fs; i++)
	{
		eventResponse[i] = amplitude * exp(-alpha * i / Fs) * sin(2 * PI * fo * i / Fs);

		eventOFS << i << "," << eventResponse[i] << std::endl;
	}

	eventOFS.close();

	/*Pseudo code*/
	bool isEventOn = false;
	bool isOutOfWall = true;
	int eCount = 0;

	UPDATE()
	{
		if (x < wall)
		{
			force = -k*x - b*v;

			if( isOutOfWall ) isEventOn = true;

			if (isEventOn)
			{
				force += eventResponse[eCount];
				eCount++;
				if (eCount == 1000)
				{
					eCount = 0;
					isEventOn = false;
				}
			}

			isOutOfWall = false;
		}		
		else
		{
			isOutOfWall = true;
			isEventOn = false;
			eCount = 0;
		}
	}

    return 0;
}

