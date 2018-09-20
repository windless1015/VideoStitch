/************************************************************************/
/* qt-opencv-multithreaded:                                             */
/* A multithreaded OpenCV application using the Qt framework.           */
/*                                                                      */
/* ProcessingThread.cpp                                                 */
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

#include "ImageBuffer.h"
#include "ProcessingThread.h"
#include "MatToQImage.h"

// Qt header files
#include <QDebug>
// OpenCV header files
#include <opencv/cv.h>
#include <opencv/highgui.h>
// Configuration header file
#include "Config.h"

ProcessingThread::ProcessingThread(ImageBuffer *imageBuffer, int inputSourceWidth, int inputSourceHeight) : QThread(),
                                   imageBuffer(imageBuffer),
                                   inputSourceWidth(inputSourceWidth),
                                   inputSourceHeight(inputSourceHeight)
{
    // Initialize variables
    stopped=false;
    sampleNo=0;
    fpsSum=0;
    avgFPS=0;
    fps.clear();
    currentROI=Rect(0,0,inputSourceWidth,inputSourceHeight);
    // Store original ROI
    originalROI=currentROI;
} // ProcessingThread constructor

ProcessingThread::~ProcessingThread()
{
} // ProcessingThread destructor

void ProcessingThread::run()
{
    while(1)
    {
        /////////////////////////////////
        // Stop thread if stopped=TRUE //
        /////////////////////////////////
        stoppedMutex.lock();
        if (stopped)
        {
            stopped=false;
            stoppedMutex.unlock();
            break;
        }
        stoppedMutex.unlock();
        /////////////////////////////////
        /////////////////////////////////

        // Save processing time
        processingTime=t.elapsed();
        // Start timer (used to calculate processing rate)
        t.start();

        // Get frame from queue, store in currentFrame, set ROI
        currentFrame=Mat(imageBuffer->getFrame(),currentROI);

        updateMembersMutex.lock();
        ///////////////////
        // PERFORM TASKS //
        ///////////////////
        // Note: ROI changes will take effect on next frame
        if(resetROIFlag)
            resetROI();
        else if(setROIFlag)
            setROI();
       
		//�Ҷȱ任
		//cvtColor(currentFrame, currentFrameGrayscale, CV_BGR2GRAY);
		//frame = MatToQImage(currentFrameGrayscale);
         
		frame=MatToQImage(currentFrame);
        updateMembersMutex.unlock();

        // Update statistics
        updateFPS(processingTime);
        currentSizeOfBuffer=imageBuffer->getSizeOfImageBuffer();
        // Inform GUI thread of new frame (QImage)
        emit newFrame(frame);
    }
    qDebug() << "Stopping processing thread...";
}

void ProcessingThread::updateFPS(int timeElapsed)
{
    // Add instantaneous FPS value to queue
    if(timeElapsed>0)
    {
        fps.enqueue((int)1000/timeElapsed);
        // Increment sample number
        sampleNo++;
    }
    // Maximum size of queue is DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH
    if(fps.size()>PROCESSING_FPS_STAT_QUEUE_LENGTH)
        fps.dequeue();
    // Update FPS value every DEFAULT_PROCESSING_FPS_STAT_QUEUE_LENGTH samples
    if((fps.size()==PROCESSING_FPS_STAT_QUEUE_LENGTH)&&(sampleNo==PROCESSING_FPS_STAT_QUEUE_LENGTH))
    {
        // Empty queue and store sum
        while(!fps.empty())
            fpsSum+=fps.dequeue();
        // Calculate average FPS
        avgFPS=fpsSum/PROCESSING_FPS_STAT_QUEUE_LENGTH;
        // Reset sum
        fpsSum=0;
        // Reset sample number
        sampleNo=0;
    }
} // updateFPS()

void ProcessingThread::stopProcessingThread()
{
    stoppedMutex.lock();
    stopped=true;
    stoppedMutex.unlock();
} // stopProcessingThread()

void ProcessingThread::setROI()
{
    // Save selection as new (current) ROI
    currentROI=selectionBox;
    qDebug() << "ROI successfully SET.";
    // Reset flag to FALSE
    setROIFlag=false;
} // setROI()

void ProcessingThread::resetROI()
{ 
    // Reset ROI to original if not already
    if((currentROI.x!=originalROI.x)&&(currentROI.y!=originalROI.y)&&(currentROI.width!=originalROI.width)&&(currentROI.height!=originalROI.height))
    {
        currentROI=originalROI;
        qDebug() << "ROI successfully RESET.";
    }
    // Reset flag to FALSE
    resetROIFlag=false;
} // resetROI()



void ProcessingThread::updateTaskData(struct TaskData taskData)
{
    QMutexLocker locker(&updateMembersMutex);
    this->setROIFlag=taskData.setROIFlag;
    this->resetROIFlag=taskData.resetROIFlag;
    this->selectionBox.x=taskData.selectionBox.left();
    this->selectionBox.y=taskData.selectionBox.top();
    this->selectionBox.width=taskData.selectionBox.width();
    this->selectionBox.height=taskData.selectionBox.height();
}

int ProcessingThread::getAvgFPS()
{
    return avgFPS;
} // getAvgFPS()

int ProcessingThread::getCurrentSizeOfBuffer()
{
    return currentSizeOfBuffer;
} // getCurrentSizeOfBuffer()

Rect ProcessingThread::getCurrentROI()
{
    return currentROI;
} // getCurrentROI();
