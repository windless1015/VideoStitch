
#include "CameraConnectDialog.h"

// Qt header files
#include <QtGui>
#include <QtWidgets/QMessageBox>
// Configuration header file
#include "Config.h"

CameraConnectDialog::CameraConnectDialog(QWidget *parent) : QDialog(parent)
{
    // Setup dialog
    setupUi(this);
    // deviceNumberEdit (device number) input string validation
    QRegExp rx_deviceNumber("[0-9]\\d{0,2}"); // Integers 0 to 999
	QRegExpValidator *validatorDeviceNumber = new QRegExpValidator(rx_deviceNumber, 0);
	deviceNumberEdit->setValidator(validatorDeviceNumber);
    // imageBufferSizeEdit (image buffer size) input string validation
    QRegExp rx_imgBufferSize("[0-9]\\d{0,2}"); // Integers 0 to 999
	QRegExpValidator *validatorImgBufferSize = new QRegExpValidator(rx_imgBufferSize, 0);
	imageBufferSizeEdit->setValidator(validatorImgBufferSize);

    setupComboBoxes();
    resetToDefaults();
    connect(resetToDefaultsPushButton,SIGNAL(released()),SLOT(resetToDefaults()));
	//对于
	connect(usbCameraRadioBtn, SIGNAL(clicked(bool)), SLOT(onClickUsbCameraRadioBtn()));
	connect(ipCameraRadioBtn, SIGNAL(clicked(bool)), SLOT(onClickIpCameraRadioBtn()));
	connect(readVideoSteamRadioBtn, SIGNAL(clicked(bool)), SLOT(onClickReadVideoStreamRadioBtn()));
} 


//获取设备类型,默认是usb 摄像头, 网络摄像机, 网络视频流
DeviceType CameraConnectDialog::getDeviceType()
{
	if (cameraButtonGroup->checkedButton() == (QAbstractButton*)usbCameraRadioBtn)
	{
		return USB_CAMERA;
	}
	else if (cameraButtonGroup->checkedButton() == (QAbstractButton*)ipCameraRadioBtn)
	{
		return IP_CAMERA;
	}
	else if (cameraButtonGroup->checkedButton() == (QAbstractButton*)readVideoSteamRadioBtn)
	{
		return VIDEOSTREAM;
	}
	return NO_DEVICE;
}


int CameraConnectDialog::getDeviceNumber()
{
	if (cameraButtonGroup->checkedButton() == (QAbstractButton*)usbCameraRadioBtn)
    {
        if(deviceNumberEdit->text().isEmpty())
        {
            QMessageBox::warning(this->parentWidget(), "警告:","设备号为空,不能连接到任何有效设备.");
            return -1;
        }
        return deviceNumberEdit->text().toInt();
    }
	else return -1;
}

QString CameraConnectDialog::getVideoStreamAddress()
{
	QString videoAddress = videoStreamAddressLineEdit->text();
	if (videoAddress.isEmpty())
	{
		return "";
	}
	else return videoAddress;
}

int CameraConnectDialog::getImageBufferSize()
{
    // Set image buffer size to default if field is blank
    if(imageBufferSizeEdit->text().isEmpty())
    {
        QMessageBox::warning(this->parentWidget(), "警告:","Image Buffer大小设置为空,请输入正确有效的数字.");
        return DEFAULT_IMAGE_BUFFER_SIZE;
    }
    // Set image buffer size to default if field is zero
    else if(imageBufferSizeEdit->text().toInt()==0)
    {
        QMessageBox::warning(this->parentWidget(), "警告:","Image Buffer大小不可为0,请输入正确有效的数字.");
        return DEFAULT_IMAGE_BUFFER_SIZE;;
    }
    // Use image buffer size specified by user
    else
        return imageBufferSizeEdit->text().toInt();
} // getImageBufferSize()

bool CameraConnectDialog::getDropFrameCheckBoxState()
{
    return dropFrameCheckBox->isChecked();
}

//设置combobox的值
void CameraConnectDialog::setupComboBoxes()
{
    QStringList threadPriorities;
    threadPriorities<<"Idle"<<"Lowest"<<"Low"<<"Normal"<<"High"<<"Highest"<<"Time Critical"<<"Inherit";
    capturePrioComboBox->addItems(threadPriorities);
    processingPrioComboBox->addItems(threadPriorities);
    // Set to defaults

}

int CameraConnectDialog::getCaptureThreadPrio()
{
    return capturePrioComboBox->currentIndex();
}

int CameraConnectDialog::getProcessingThreadPrio()
{
    return processingPrioComboBox->currentIndex();
}

void CameraConnectDialog::resetToDefaults()
{
	usbCameraRadioBtn->setChecked(true);
	deviceNumberEdit->setEnabled(true);
    // 默认是
    if(DEFAULT_CAMERA_DEV_NO!=-1)
    {
        
        deviceNumberEdit->setText(QString::number(DEFAULT_CAMERA_DEV_NO));
    }
    else
    {
		
        deviceNumberEdit->clear();
    }
	videoStreamAddressLineEdit->setDisabled(true);
	lineEdit_ip_camera_address->setDisabled(true);
	lineEdit_ip_camera_port->setDisabled(true);
	lineEdit_ip_camera_user->setDisabled(true);
	lineEdit_ip_camera_psw->setDisabled(true);

    // Image buffer size
    imageBufferSizeEdit->setText(QString::number(DEFAULT_IMAGE_BUFFER_SIZE));
    // Drop frames
    dropFrameCheckBox->setChecked(DEFAULT_DROP_FRAMES);
    // Capture thread
    if(DEFAULT_CAP_THREAD_PRIO==QThread::IdlePriority)
        capturePrioComboBox->setCurrentIndex(0);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::LowestPriority)
        capturePrioComboBox->setCurrentIndex(1);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::LowPriority)
        capturePrioComboBox->setCurrentIndex(2);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::NormalPriority)
        capturePrioComboBox->setCurrentIndex(3);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::HighPriority)
        capturePrioComboBox->setCurrentIndex(4);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::HighestPriority)
        capturePrioComboBox->setCurrentIndex(5);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::TimeCriticalPriority)
        capturePrioComboBox->setCurrentIndex(6);
    else if(DEFAULT_CAP_THREAD_PRIO==QThread::InheritPriority)
        capturePrioComboBox->setCurrentIndex(7);
    // Processing thread
    if(DEFAULT_PROC_THREAD_PRIO==QThread::IdlePriority)
        processingPrioComboBox->setCurrentIndex(0);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::LowestPriority)
        processingPrioComboBox->setCurrentIndex(1);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::LowPriority)
        processingPrioComboBox->setCurrentIndex(2);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::NormalPriority)
        processingPrioComboBox->setCurrentIndex(3);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::HighPriority)
        processingPrioComboBox->setCurrentIndex(4);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::HighestPriority)
        processingPrioComboBox->setCurrentIndex(5);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::TimeCriticalPriority)
        processingPrioComboBox->setCurrentIndex(6);
    else if(DEFAULT_PROC_THREAD_PRIO==QThread::InheritPriority)
        processingPrioComboBox->setCurrentIndex(7);
} // resetToDefaults()

void CameraConnectDialog::onClickUsbCameraRadioBtn()
{
	deviceNumberEdit->setEnabled(true);
	lineEdit_ip_camera_address->setDisabled(true);
	lineEdit_ip_camera_port->setDisabled(true);
	lineEdit_ip_camera_user->setDisabled(true);
	lineEdit_ip_camera_psw->setDisabled(true);
	videoStreamAddressLineEdit->setDisabled(true);
}

void CameraConnectDialog::onClickIpCameraRadioBtn()
{
	lineEdit_ip_camera_address->setEnabled(true);
	lineEdit_ip_camera_port->setEnabled(true);
	lineEdit_ip_camera_user->setEnabled(true);
	lineEdit_ip_camera_psw->setEnabled(true);
	deviceNumberEdit->setDisabled(true);
	videoStreamAddressLineEdit->setDisabled(true);
}

void CameraConnectDialog::onClickReadVideoStreamRadioBtn()
{
	videoStreamAddressLineEdit->setEnabled(true);
	deviceNumberEdit->setDisabled(true);
	lineEdit_ip_camera_address->setDisabled(true);
	lineEdit_ip_camera_port->setDisabled(true);
	lineEdit_ip_camera_user->setDisabled(true);
	lineEdit_ip_camera_psw->setDisabled(true);
}

