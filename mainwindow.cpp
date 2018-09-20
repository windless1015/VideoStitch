#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/highgui.hpp>
#include "Controller.h"
#include "CameraConnectDialog.h"
#include <QtWidgets\qmessagebox.h>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

	// Create controller
	controller = new Controller;
	// Save application version in QString variable
	appVersion = QUOTE(APP_VERSION);

	//initializeTaskDataStructure();
	// Initialize GUI
	setInitialGUIState();
	// Connect signals to slots
	signalSlotsInit();
	// Initialize flag
	isCameraConnected = false;
}

MainWindow::~MainWindow()
{
	if (isCameraConnected)
		controller->disconnectCamera();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::initializeTaskDataStructure()
{
	taskData.setROIFlag = false;
	taskData.resetROIFlag = false;
} // initializeTaskDataStructure()

void MainWindow::setInitialGUIState()
{
	//////////////////////////////////////////
	// Enable/disable appropriate GUI items //
	//////////////////////////////////////////
	// Menu
	ui->actionConnectCamera->setEnabled(true);
	ui->actionDisconnectCamera->setEnabled(false);

	ui->frameLabel->setText("No camera connected.");
	ui->imageBufferBar->setValue(0);
	ui->imageBufferLabel->setText("[000/000]");
	ui->captureRateLabel->setText("");
	ui->processingRateLabel->setText("");
	ui->videoSourceLabel->setText("");
	ui->cameraResolutionLabel->setText("");
	ui->roiLabel->setText("");
	ui->mouseCursorPosLabel->setText("");
	ui->clearImageBufferButton->setDisabled(true);


} // setInitialGUIState()

void MainWindow::signalSlotsInit()
{
	connect(ui->actionConnectCamera, SIGNAL(triggered()), this, SLOT(connectToCamera()));
	connect(ui->actionDisconnectCamera, SIGNAL(triggered()), this, SLOT(disconnectCamera()));
	connect(ui->actionEixt, SIGNAL(triggered()), this, SLOT(close()));
	/*connect(grayscaleAction, SIGNAL(toggled(bool)), this, SLOT(setGrayscale(bool)));
	connect(smoothAction, SIGNAL(toggled(bool)), this, SLOT(setSmooth(bool)));
	connect(dilateAction, SIGNAL(toggled(bool)), this, SLOT(setDilate(bool)));
	connect(erodeAction, SIGNAL(toggled(bool)), this, SLOT(setErode(bool)));
	connect(flipAction, SIGNAL(toggled(bool)), this, SLOT(setFlip(bool)));
	connect(cannyAction, SIGNAL(toggled(bool)), this, SLOT(setCanny(bool)));
	connect(imageProcessingSettingsAction, SIGNAL(triggered()), this, SLOT(setImageProcessingSettings()));
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
	connect(clearImageBufferButton, SIGNAL(released()), this, SLOT(clearImageBuffer()));
	*/
	connect(ui->frameLabel, SIGNAL(onMouseMoveEvent()), this, SLOT(updateMouseCursorPosLabel()));
} // signalSlotsInit()


void MainWindow::connectToCamera()
{
	// Create CameraConnectDialog instance
	cameraConnectDialog = new CameraConnectDialog(this);

	if (cameraConnectDialog->exec() == 1)
	{
		// Store image buffer size in local variable
		imageBufferSize = cameraConnectDialog->getImageBufferSize();
		//获取设备类型
		deviceType = cameraConnectDialog->getDeviceType();

		//如果连接的设备是usb 摄像头则还需要设备号
		if (deviceType == USB_CAMERA)
		{
			int deviceNumber = cameraConnectDialog->getDeviceNumber();
			if ((isCameraConnected = controller->connectToCamera(deviceNumber, imageBufferSize,
				cameraConnectDialog->getDropFrameCheckBoxState(),
				cameraConnectDialog->getCaptureThreadPrio(),
				cameraConnectDialog->getProcessingThreadPrio())))
			{

				connect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));
				connect(controller->processingThread, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame(QImage)));
				connect(this, SIGNAL(newTaskData(struct TaskData)), controller->processingThread, SLOT(updateTaskData(struct TaskData)));

				initializeTaskDataStructure();
				emit newTaskData(taskData);
				ui->imageBufferBar->setMinimum(0);
				ui->imageBufferBar->setMaximum(imageBufferSize);
				ui->actionConnectCamera->setEnabled(false);
				ui->actionDisconnectCamera->setEnabled(true);
				ui->clearImageBufferButton->setEnabled(true);
				sourceWidth = controller->captureThread->getInputSourceWidth();
				sourceHeight = controller->captureThread->getInputSourceHeight();
				ui->videoSourceLabel->setText("USB_CAMERA");
				ui->cameraResolutionLabel->setText(QString::number(sourceWidth) + QString("x") + QString::number(sourceHeight));
			}
			// Display error dialog if camera connection is unsuccessful
			else
			{
				QMessageBox::warning(this, "错误:", "无法连接视频源.");
				delete cameraConnectDialog;
			}
		}
		
	}
}

void MainWindow::disconnectCamera()
{
	// Check if camera is connected
	if (controller->captureThread->isCameraConnected())
	{
		//// Disconnect connections (5)
		disconnect(ui->frameLabel, SIGNAL(newMouseData(struct MouseData)), this, SLOT(newMouseData(struct MouseData)));
		disconnect(controller->processingThread, SIGNAL(newFrame(QImage)), 0, 0);
		//disconnect(this->imageProcessingSettingsDialog, SIGNAL(newImageProcessingSettings(struct ImageProcessingSettings)), controller->processingThread, SLOT(updateImageProcessingSettings(struct ImageProcessingSettings)));
		//disconnect(this, SIGNAL(newImageProcessingFlags(struct ImageProcessingFlags)), controller->processingThread, SLOT(updateImageProcessingFlags(struct ImageProcessingFlags)));
		disconnect(this, SIGNAL(newTaskData(struct TaskData)), controller->processingThread, SLOT(updateTaskData(struct TaskData)));
		// Delete dialogs
		delete cameraConnectDialog;
		//delete imageProcessingSettingsDialog;
		//// Reset flag
		isCameraConnected = false;
		// Set GUI
		setInitialGUIState();
		// Disconnect camera
		controller->disconnectCamera();
	}
	// Display error dialog
	else
		QMessageBox::warning(this, "错误:", "已经视频源已经断开.");
}

void MainWindow::clearImageBuffer()
{
	controller->clearImageBuffer();
}

void MainWindow::updateFrame(const QImage &frame)
{
	//// Show [number of images in buffer / image buffer size] in imageBufferLabel in main window
	ui->imageBufferLabel->setText(QString("[") + QString::number(controller->processingThread->getCurrentSizeOfBuffer()) +
		QString("/") + QString::number(imageBufferSize) + QString("]"));
	//// Show percentage of image bufffer full in imageBufferBar in main window
	ui->imageBufferBar->setValue(controller->processingThread->getCurrentSizeOfBuffer());
	//// Show processing rate in captureRateLabel in main window
	ui->captureRateLabel->setNum(controller->captureThread->getAvgFPS());
	ui->captureRateLabel->setText(ui->captureRateLabel->text() + " fps");
	//// Show processing rate in processingRateLabel in main window
	ui->processingRateLabel->setNum(controller->processingThread->getAvgFPS());
	ui->processingRateLabel->setText(ui->processingRateLabel->text() + " fps");
	//// Show ROI information in roiLabel in main window
	ui->roiLabel->setText(QString("(") + QString::number(controller->processingThread->getCurrentROI().x) + QString(",") +
		QString::number(controller->processingThread->getCurrentROI().y) + QString(") ") +
		QString::number(controller->processingThread->getCurrentROI().width) +
		QString("x") + QString::number(controller->processingThread->getCurrentROI().height));
	//// Display frame in main window
	ui->frameLabel->setPixmap(QPixmap::fromImage(frame));
}


void MainWindow::newMouseData(struct MouseData mouseData)
{
	// Local variables
	int x_temp, y_temp, width_temp, height_temp;
	// Set ROI
	if (mouseData.leftButtonRelease)
	{
		// Copy box dimensions from mouseData to taskData
		taskData.selectionBox.setX(mouseData.selectionBox.x() - ((ui->frameLabel->width() - controller->captureThread->getInputSourceWidth())) / 2);
		taskData.selectionBox.setY(mouseData.selectionBox.y() - ((ui->frameLabel->height() - controller->captureThread->getInputSourceHeight())) / 2);
		taskData.selectionBox.setWidth(mouseData.selectionBox.width());
		taskData.selectionBox.setHeight(mouseData.selectionBox.height());
		// Check if selection box has NON-ZERO dimensions
		if ((taskData.selectionBox.width() != 0) && ((taskData.selectionBox.height()) != 0))
		{
			// Selection box can also be drawn from bottom-right to top-left corner
			if (taskData.selectionBox.width()<0)
			{
				x_temp = taskData.selectionBox.x();
				width_temp = taskData.selectionBox.width();
				taskData.selectionBox.setX(x_temp + taskData.selectionBox.width());
				taskData.selectionBox.setWidth(width_temp*-1);
			}
			if (taskData.selectionBox.height()<0)
			{
				y_temp = taskData.selectionBox.y();
				height_temp = taskData.selectionBox.height();
				taskData.selectionBox.setY(y_temp + taskData.selectionBox.height());
				taskData.selectionBox.setHeight(height_temp*-1);
			}
			// Check if selection box is not outside window
			if ((taskData.selectionBox.x()<0) || (taskData.selectionBox.y()<0) ||
				((taskData.selectionBox.x() + taskData.selectionBox.width())>sourceWidth) ||
				((taskData.selectionBox.y() + taskData.selectionBox.height())>sourceHeight))
			{
				// Display error message
				QMessageBox::warning(this, "ERROR:", "Selection box outside range. Please try again.");
			}
			else
			{
				// Set setROIFlag to TRUE
				taskData.setROIFlag = true;
				//// Update task data in processingThread
				emit newTaskData(taskData);
				// Set setROIFlag to FALSE
				taskData.setROIFlag = false;
			}
		}
	}
	// Reset ROI
	else if (mouseData.rightButtonRelease)
	{
		// Set resetROIFlag to TRUE
		taskData.resetROIFlag = true;
		// Update task data in processingThread
		emit newTaskData(taskData);
		// Set resetROIFlag to FALSE
		taskData.resetROIFlag = false;
	}
}


void MainWindow::updateMouseCursorPosLabel()
{
	// Update mouse cursor position in mouseCursorPosLabel in main window
	ui->mouseCursorPosLabel->setText(QString("(") + QString::number(ui->frameLabel->getMouseCursorPos().x()) +
		QString(",") + QString::number(ui->frameLabel->getMouseCursorPos().y()) +
		QString(")"));
	// Show ROI-adjusted cursor position if camera is connected
	if (isCameraConnected)
		ui->mouseCursorPosLabel->setText(ui->mouseCursorPosLabel->text() +
		QString(" [") + QString::number(ui->frameLabel->getMouseCursorPos().x() - (640 - controller->processingThread->getCurrentROI().width) / 2) +
		QString(",") + QString::number(ui->frameLabel->getMouseCursorPos().y() - (480 - controller->processingThread->getCurrentROI().height) / 2) +
		QString("]"));
}

