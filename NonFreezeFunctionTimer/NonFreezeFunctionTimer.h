/*
 * NonFreezeFunctionTimer.h
 * NON FREEZE FUNCTION TIMER - 29.03.2018
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

#ifndef NONFREEZEFUNCTIONTIMER_H
#define NONFREEZEFUNCTIONTIMER_H

#include <stdlib.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

 //
 // Constants. Please be carefully when updating all of this. Otherwise, 
 // Your sketch may not run very well as before. Do not forget that,
 //
 // The MIN value Must be smaller than MAX value. 
 // INTERVAL value should not be zero or minus. 
 // QUEUE size should not be bigger then your memory size of device.
 //
#define DEFAULT_INTERVAL_MILLIS 500
#define DEFAULT_INTERVAL_MILLIS_MIN 10
#define DEFAULT_INTERVAL_MILLIS_MAX 60000
#define DEFAULT_QUEUE_SIZE 32
#define DEFAULT_STATUS true

class FunctionTimer
{
private:

	// A structure that includes all required info's.
	struct data
	{
		void(*pointer)(void) = NULL;
		bool enabledStatus = DEFAULT_STATUS;
		unsigned short previousMillis = 0;
		unsigned short intervalMillis = DEFAULT_INTERVAL_MILLIS;
	};

	struct data *timerQueueArray = NULL;
	char timerQueueCount = 0;
	bool timerEnabledStatus = false;

	// -----

	bool isRegistered(void(*pointer)(void));
	bool checkRange(unsigned short intervalMillis);

public:

	// Constructor.
	FunctionTimer();

	bool attach(void(*pointer)(void));
	bool attach(void(*pointer)(void), unsigned short intervalMillis);
	bool attach(void(*pointer)(void), bool enabledStatus);
	bool attach(void(*pointer)(void), unsigned short intervalMillis, bool enabledStatus);
	bool detach(void(*pointer)(void));

	void start();
	void loop();
	void stop();

	bool startProcess(void(*pointer)(void));
	bool stopProcess(void(*pointer)(void));
};

//
// We are making this for avoiding an instance of this class. Now, you do not have to
// Declare an instance of this class and can use it directly in your project.
//
extern FunctionTimer NonFreezeFunctionTimer;

#endif