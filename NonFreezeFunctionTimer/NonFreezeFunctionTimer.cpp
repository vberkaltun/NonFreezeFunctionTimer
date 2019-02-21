/*
 * NonFreezeFunctionTimer.cpp
 * NON FREEZE FUNCTION TIMER - 28.04.2018
 *
 * =============================================================================
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Berk Altun - vberkaltun.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * =============================================================================
 */

#include "NonFreezeFunctionTimer.h"

 //
 // Checks a given function pointer that was added at before or not.
 // This step is required for avoiding multiple addition of a function.
 //
 // <Param Name:"pointer"> Pointer of function.
 // <Returns> Boolean, if not added at before true, otherwise false.
 //
bool FunctionTimer::isRegistered(void(*pointer)(void))
{
	bool isRegisteredFlag = false;

	if (timerQueueArray != NULL)
	{
		for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		{
			if (pointer == timerQueueArray[currentQueueCount].pointer)
			{
				isRegisteredFlag = true;
				break;
			}
		}
	}

	return isRegisteredFlag;
}

//
// Checks the interval of a given function. Max and min range was declared 
// As constant. You can update it depending on your purpose and sketch.
//
// <Param Name:"intervalMillis"> Interval millis of a given function.
// <Returns> Boolean, if range is correct true, otherwise false.
//
bool FunctionTimer::checkRange(unsigned short intervalMillis)
{
	if (intervalMillis < DEFAULT_INTERVAL_MILLIS_MIN)
		return false;

	if (intervalMillis > DEFAULT_INTERVAL_MILLIS_MAX)
		return false;

	// Best case.
	return true;
}

// -----

//
// Constructor.
//
FunctionTimer::Timer()
{
}

// -----

//
// Try to attach a function to local function queue with default millis and status value.
// Based on default status value, the given function may start with auto trigger or not.
//
// <Param Name:"pointer"> Pointer of function.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::attach(void(*pointer)(void))
{
	return this->attach(pointer, DEFAULT_INTERVAL_MILLIS, DEFAULT_STATUS);
}

//
// <Param Name:"pointer"> Pointer of function.
// <Param Name:"intervalMillis"> Interval millis for each function call.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::attach(void(*pointer)(void), unsigned short intervalMillis)
{
	return this->attach(pointer, intervalMillis, DEFAULT_STATUS);
}

//
// <Param Name:"pointer"> Pointer of function.
// <Param Name:"enabledStatus"> Enabled status of the given function.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::attach(void(*pointer)(void), bool enabledStatus)
{
	return this->attach(pointer, DEFAULT_INTERVAL_MILLIS, enabledStatus);
}

//
// Best approach for adding a function to function queue. With this overload,
// You can pass all required variable yourself.
// 
// <Param Name:"pointer"> Pointer of function.
// <Param Name:"intervalMillis"> Interval millis for each function call.
// <Param Name:"enabledStatus"> Enabled status of the given function.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::attach(void(*pointer)(void), unsigned short intervalMillis, bool enabledStatus)
{
	bool attachFlag = false;

	// Makes cross check.
	if (!isRegistered(pointer) && checkRange(intervalMillis) && timerQueueCount < DEFAULT_QUEUE_SIZE)
	{
		// Two different approach for adding a new item to queue.
		if (timerQueueArray == NULL)
			timerQueueArray = (data *)malloc(sizeof(data)*(++timerQueueCount));
		else
			timerQueueArray = (data *)realloc(timerQueueArray, sizeof(data)*(++timerQueueCount));

		// Sets all default values of a new added item.
		timerQueueArray[timerQueueCount - 1].pointer = pointer;
		timerQueueArray[timerQueueCount - 1].intervalMillis = intervalMillis;
		timerQueueArray[timerQueueCount - 1].previousMillis = millis();
		timerQueueArray[timerQueueCount - 1].enabledStatus = enabledStatus;

		// Adding is successful, that is mean return type is true.
		attachFlag = true;
	}

	return attachFlag;
}

//
// <Param Name:"pointer"> Pointer of function that will delete.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::detach(void(*pointer)(void))
{
	bool detachFlag = false;

	if (isRegistered(pointer))
	{
		for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		{
			if (timerQueueArray[currentQueueCount].pointer == pointer)
			{
				// There a two approach state for detach.One of them is that 
				// Queue has only one item, another one is that there are 
				// Multiple item on the queue.
				if (timerQueueCount == 1)
				{
					timerQueueArray = NULL;
					timerQueueCount = 0;
				}
				else
				{
					timerQueueArray[currentQueueCount] = timerQueueArray[timerQueueCount - 1];
					timerQueueArray = (data *)realloc(timerQueueArray, sizeof(data)*(--timerQueueCount));
				}

				// Do not scan after all. we found it.
				detachFlag = true;
				break;
			}
		}
	}

	return detachFlag;
}

// -----

//
// Starts main thread of library.
//
void FunctionTimer::start()
{
	// Updates millis of all registered functions.
	for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		timerQueueArray[currentQueueCount].previousMillis = millis();

	timerEnabledStatus = true;
}

//
// Loops the main thread. Based on architecture of Arduino, you need to run this
// Function in loop function at your sketch. Otherwise, The library can not run 
// Cause of non call or trigger.
//
void FunctionTimer::loop()
{
	while (timerEnabledStatus && timerQueueArray != NULL)
	{
		for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		{
			if (timerQueueArray[currentQueueCount].enabledStatus == true)
			{
				// Get current millis value, we will use this value for comparing 
				// With the latest stored millis of each function. If specified time was passed, 
				// We will trigger function again or not.
				unsigned short currentMillis = millis();

				if (currentMillis - timerQueueArray[currentQueueCount].previousMillis >= timerQueueArray[currentQueueCount].intervalMillis)
				{
					timerQueueArray[currentQueueCount].previousMillis = currentMillis;
					timerQueueArray[currentQueueCount].pointer();
				}
			}
		}
	}
}

//
// Stops main thread of library.
//
void FunctionTimer::stop()
{
	timerEnabledStatus = false;
}

// -----

//
// Starts a function thread in library.
//
// <Param Name:"pointer"> Pointer of function that will stop.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::startProcess(void(*pointer)(void))
{
	bool startProcessFlag = false;

	// Checks registering of function.
	// In best case, given function registered and returns true.
	// Otherwise, returns false and we can not pass to inside of function.
	if (isRegistered(pointer))
	{
		for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		{
			if (timerQueueArray[currentQueueCount].pointer == pointer)
			{
				// Updates the previous millis also, this is required for avoiding
				// The auto trigger on status updating.
				timerQueueArray[currentQueueCount].previousMillis = millis();
				timerQueueArray[currentQueueCount].enabledStatus = true;

				startProcessFlag = true;
				break;
			}
		}
	}

	return startProcessFlag;
}

//
// Starts a function thread in library.
//
// <Param Name:"pointer"> Pointer of function that will stop.
// <Returns> Boolean, if successful true, otherwise false.
//
bool FunctionTimer::stopProcess(void(*pointer)(void))
{
	bool stopProcessFlag = false;

	if (isRegistered(pointer))
	{
		for (char currentQueueCount = 0; currentQueueCount < timerQueueCount; currentQueueCount++)
		{
			// save the last time you blinked the LED
			if (timerQueueArray[currentQueueCount].pointer == pointer)
			{
				timerQueueArray[currentQueueCount].enabledStatus = false;

				// Do not scan after all. we found it.
				stopProcessFlag = true;
				break;
			}
		}
	}

	return stopProcessFlag;
}

// -----

//
// Preinstantiate Object.
//
FunctionTimer NonFreezeFunctionTimer = FunctionTimer();
