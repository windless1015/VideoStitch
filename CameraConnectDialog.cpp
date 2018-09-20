
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
} 


//��ȡ�豸����,Ĭ����usb ����ͷ, ���������, ������Ƶ��
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
            QMessageBox::warning(this->parentWidget(), "����:","�豸��Ϊ��,�������ӵ��κ���Ч�豸.");
            return -1;
        }
        return deviceNumberEdit->text().toInt();
    }
	else return -1;
}

int CameraConnectDialog::getImageBufferSize()
{
    // Set image buffer size to default if field is blank
    if(imageBufferSizeEdit->text().isEmpty())
    {
        QMessageBox::warning(this->parentWidget(), "����:","Image Buffer��С����Ϊ��,��������ȷ��Ч������.");
        return DEFAULT_IMAGE_BUFFER_SIZE;
    }
    // Set image buffer size to default if field is zero
    else if(imageBufferSizeEdit->text().toInt()==0)
    {
        QMessageBox::warning(this->parentWidget(), "����:","Image Buffer��С����Ϊ0,��������ȷ��Ч������.");
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

//����combobox��ֵ
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
    // Ĭ����
    if(DEFAULT_CAMERA_DEV_NO!=-1)
    {
        
        deviceNumberEdit->setText(QString::number(DEFAULT_CAMERA_DEV_NO));
    }
    else
    {
		
        deviceNumberEdit->clear();
    }
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
