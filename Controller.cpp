/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* Controller.cpp                                                       */
/*                                                                      */
/* Nick D'Ademo <nickdademo@gmail.com>                                  */
/*                                                                      */
/* Copyright (c) 2012 Nick D'Ademo                                      */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the "Software"), to deal in the Software without restriction, */
/* including without limitation the rights to use, copy, modify, merge, */
/* publish, distribute, sublicense, and/or sell copies of the Software, */
/* and to permit persons to whom the Software is furnished to do so,    */
/* subject to the following conditions:                                 */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  */
/* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   */
/* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    */
/* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     */
/* SOFTWARE.                                                            */
/*                                                                      */
/************************************************************************/

#include "Controller.h"
#include "ImageBuffer.h"

// Qt header files
#include <QtGui>

Controller::Controller()
{
} // Controller constructor

Controller::~Controller()
{
} // Controller destructor

bool Controller::connectToCamera(int deviceNumber, int imageBufferSize, bool dropFrame, int capThreadPrio, int procThreadPrio)
{
    // Local variables
    bool isOpened=false;
    // Store imageBufferSize in private member
    this->imageBufferSize=imageBufferSize;
    // Create image buffer with user-defined settings
    imageBuffer = new ImageBuffer(imageBufferSize,dropFrame);
    // Create capture thread
    captureThread = new CaptureThread(imageBuffer);
    // Attempt to connect to camera
    if((isOpened= captureThread->connectToCamera(deviceNumber)))
    {
        // Create processing thread
        processingThread = new ProcessingThread(imageBuffer,captureThread->getInputSourceWidth(),captureThread->getInputSourceHeight());
        // Start capturing frames from camera
        captureThread->start((QThread::Priority)capThreadPrio);
        // Start processing captured frames
        processingThread->start((QThread::Priority)procThreadPrio);
    }
    else
    {
        deleteCaptureThread();
        deleteImageBuffer();
    }
    return isOpened;
}

bool Controller::connectToVideoStream(QString&videoStreamAddr, int imageBufferSize, bool dropFrame, int capThreadPrio, int procThreadPrio)
{
	// Local variables
	bool isOpened = false;
	// Store imageBufferSize in private member
	this->imageBufferSize = imageBufferSize;
	// Create image buffer with user-defined settings
	imageBuffer = new ImageBuffer(imageBufferSize, dropFrame);
	// Create capture thread
	captureThread = new CaptureThread(imageBuffer);
	// Attempt to connect to camera
	if ((isOpened = captureThread->connectToCamera(videoStreamAddr)))
	{
		// Create processing thread
		processingThread = new ProcessingThread(imageBuffer, captureThread->getInputSourceWidth(), captureThread->getInputSourceHeight());
		// Start capturing frames from camera
		captureThread->start((QThread::Priority)capThreadPrio);
		// Start processing captured frames
		processingThread->start((QThread::Priority)procThreadPrio);
	}
	else
	{
		deleteCaptureThread();
		deleteImageBuffer();
	}
	return isOpened;
}


void Controller::disconnectCamera()
{

   if(processingThread->isRunning())
        stopProcessingThread();
    if(captureThread->isRunning())
        stopCaptureThread();
    clearImageBuffer();
    captureThread->disconnectCamera();
    deleteCaptureThread();
    deleteProcessingThread();

    deleteImageBuffer();
}

void Controller::stopCaptureThread()
{
    qDebug() << "About to stop capture thread...";
    captureThread->stopCaptureThread();
    // Take one frame off a FULL queue to allow the capture thread to finish
    if(imageBuffer->getSizeOfImageBuffer()==imageBufferSize)
        Mat temp=imageBuffer->getFrame();
    captureThread->wait();
    qDebug() << "Capture thread successfully stopped.";
} // stopCaptureThread()

void Controller::stopProcessingThread()
{
    qDebug() << "About to stop processing thread...";
    processingThread->stopProcessingThread();
    processingThread->wait();
    qDebug() << "Processing thread successfully stopped.";
}

void Controller::deleteCaptureThread()
{
    delete captureThread;
}

void Controller::deleteProcessingThread()
{
   delete processingThread;
}

void Controller::clearImageBuffer()
{
    imageBuffer->clearBuffer();
} // clearImageBuffer()

void Controller::deleteImageBuffer()
{
    // Delete image buffer
    delete imageBuffer;
} // deleteImageBuffer()
