#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Structures.h"
class CameraConnectDialog;
class Controller;

namespace Ui {
class MainWindow;
}

#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	void initializeTaskDataStructure();
	void setInitialGUIState();
	void signalSlotsInit();

private:
	CameraConnectDialog *cameraConnectDialog;

	Controller *controller;

	TaskData taskData;
	QString appVersion;
	int sourceWidth; //视频源的宽度
	int sourceHeight; //视频源的高度
	DeviceType deviceType; //设备类型,
	int imageBufferSize; //图像缓冲区大小
	bool isCameraConnected;//是否连接标记位


private slots:
	void connectToCamera();
	void disconnectCamera();
	void clearImageBuffer();//清除img buffer
	void updateFrame(const QImage &frame);
	void newMouseData(struct MouseData);
	void updateMouseCursorPosLabel(); //更新鼠标位置


    void on_pushButton_clicked();

signals:
	void newTaskData(struct TaskData taskData);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
