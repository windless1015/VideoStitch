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
	int sourceWidth; //��ƵԴ�Ŀ��
	int sourceHeight; //��ƵԴ�ĸ߶�
	DeviceType deviceType; //�豸����,
	int imageBufferSize; //ͼ�񻺳�����С
	bool isCameraConnected;//�Ƿ����ӱ��λ


private slots:
	void connectToCamera();
	void disconnectCamera();
	void clearImageBuffer();//���img buffer
	void updateFrame(const QImage &frame);
	void newMouseData(struct MouseData);
	void updateMouseCursorPosLabel(); //�������λ��


    void on_pushButton_clicked();

signals:
	void newTaskData(struct TaskData taskData);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
