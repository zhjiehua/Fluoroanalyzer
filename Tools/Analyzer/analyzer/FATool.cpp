#include "FATool.h"
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QButtonGroup>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include "Misc.h"


#define CMD_HEAD	0xA55A


FATool::FATool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	bgRcv = new QButtonGroup(this);
	bgSend = new QButtonGroup(this);
	bgRcv->addButton(ui.rdbRcvHex, 0);
	bgRcv->addButton(ui.rdbRcvASCII, 1);
	bgSend->addButton(ui.rdbSendHex, 0);
	bgSend->addButton(ui.rdbSendASCII, 1);
	ui.rdbRcvASCII->setChecked(true);
	//ui.rdbSendASCII->setChecked(true);
	ui.rdbSendHex->setChecked(true);
	connect(bgRcv, SIGNAL(buttonClicked(int)), this, SLOT(bgRcvButtonsClicked(int)));
	connect(bgSend, SIGNAL(buttonClicked(int)), this, SLOT(bgSendButtonsClicked(int)));

	InitPort();//��ʼ������

	//��ʱ��ȡ��������
	mainTimer = new QTimer(this);
	connect(mainTimer, SIGNAL(timeout()), this, SLOT(UartDataParse()));
	mainTimer->start(5);

	plotter.setWindowTitle(QObject::tr("Fluoroanalyzer ADC data"));
}


/*********************************************************************************
*
*
*
*
**********************************************************************************/


/****************************���ڳ�ʼ��*****************************/
void FATool::InitPort(void)
{
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";

	//��ȡ������Ϣ  
	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		qDebug() << "Name:" << info.portName();
		qDebug() << "Description:" << info.description();
		qDebug() << "Manufacturer:" << info.manufacturer();

		//�����൱���Զ�ʶ�𴮿ں�֮����ӵ���cmb�����Ҫ�ֶ�ѡ������������б�ķ�ʽ��ӽ�ȥ  
		QSerialPort serial;
		serial.setPort(info);
//		if (serial.open(QIODevice::ReadWrite))
		{
			//�����ں���ӵ�cmb  
			ui.cmbPortName->addItem(info.portName());
			//�رմ��ڵȴ���Ϊ(�򿪴��ڰ�ť)��  
//			serial.close();
		}
	}

	QStringList baudList;//������  
	QStringList parityList;//У��λ  
	QStringList dataBitsList;//����λ  
	QStringList stopBitsList;//ֹͣλ  

	baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300"
		<< "600" << "1200" << "1800" << "2400" << "4800" << "9600"
		<< "14400" << "19200" << "38400" << "56000" << "57600"
		<< "76800" << "115200" << "128000" << "256000";

	ui.cmbBaudRate->addItems(baudList);
	ui.cmbBaudRate->setCurrentIndex(19); //115200
										 //ui.cmbBaudRate->setCurrentIndex(12); //9600

	parityList << QString::fromLocal8Bit("��") << QString::fromLocal8Bit("��") << QString::fromLocal8Bit("ż");
	parityList << QString::fromLocal8Bit("��־");
	parityList << QString::fromLocal8Bit("�ո�");

	ui.cmbParity->addItems(parityList);
	ui.cmbParity->setCurrentIndex(0);

	dataBitsList << "5" << "6" << "7" << "8";
	ui.cmbDataBits->addItems(dataBitsList);
	ui.cmbDataBits->setCurrentIndex(3);

	stopBitsList << "1";
	stopBitsList << "1.5";
	stopBitsList << "2";

	ui.cmbStopBits->addItems(stopBitsList);
	ui.cmbStopBits->setCurrentIndex(0);

	//���ð�ť���Ա�����  
	ui.btnOpen->setCheckable(true);

	ui.teReceiveData->setReadOnly(true);
}

/****************************��������******************************/
void FATool::on_btnOpen_clicked()
{
	if (ui.btnOpen->text() == QString::fromLocal8Bit("�򿪴���"))
	{
		serialPort = new QSerialPort(this);

		//���ô��ں�  
		serialPort->setPortName(ui.cmbPortName->currentText());
		//�Զ�д��ʽ�򿪴���  
		if (serialPort->open(QIODevice::ReadWrite))
		{
			//���ò�����  
			serialPort->setBaudRate(ui.cmbBaudRate->currentText().toInt());
			//��������λ  
			serialPort->setDataBits(QSerialPort::Data8);
			//����У��λ  
			serialPort->setParity(QSerialPort::NoParity);
			//����������  
			serialPort->setFlowControl(QSerialPort::NoFlowControl);
			//����ֹͣλ  
			serialPort->setStopBits(QSerialPort::OneStop);

			//ÿ���һ��  
			timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(ReadComDataSlot()));
			timer->start(100);

			SetNonSelectable();
		}
		else
		{
            QMessageBox::about(nullptr, "Tips", "Port is not open");
			return;
		}
	}
	else
	{
		timer->stop();
		SetSelectable();
		serialPort->close();
	}

	//����FFT
	//FFT_Test();

}

void FATool::SetNonSelectable(void)
{
	ui.btnOpen->setText(QString::fromLocal8Bit("�رմ���"));

	ui.cmbPortName->setEditable(false);
	ui.cmbBaudRate->setEditable(false);
	ui.cmbDataBits->setEditable(false);
	ui.cmbParity->setEditable(false);
	ui.cmbStopBits->setEditable(false);
	ui.cmbFlowControl->setEditable(false);

	ui.cmbPortName->hidePopup();
}

void FATool::SetSelectable(void)
{
	ui.btnOpen->setText(QString::fromLocal8Bit("�򿪴���"));

	ui.cmbPortName->setEditable(true);
	ui.cmbBaudRate->setEditable(true);
	ui.cmbDataBits->setEditable(true);
	ui.cmbParity->setEditable(true);
	ui.cmbStopBits->setEditable(true);
	ui.cmbFlowControl->setEditable(true);

	ui.cmbPortName->showPopup();
}

/****************************���ݴ���******************************/
void FATool::ReadComDataSlot()
{
	//��ȡ��������  
	QByteArray readComData = serialPort->readAll();

	//��������������ʾ�����ݽ�������te��  
	if (!readComData.isNull())
	{
		if (bgRcv->checkedId() == 0)//hex
		{
			ui.teReceiveData->append(readComData.toHex());
		}
		else//ascii
		{
			ui.teReceiveData->append(readComData.data());
		}


		uartBuffer.append(readComData);

		qDebug() << readComData;
	}

	//���������  
	readComData.clear();
}

void FATool::on_btnSend_clicked()
{
	if (bgSend->checkedId() == 0)//hex
	{
		QString sendData = ui.teSendData->toPlainText();
		sendBufferHex = Misc::QString2Hex(sendData);

		//д�뻺����
		if (serialPort)
			serialPort->write(sendBufferHex);
		else
            QMessageBox::about(nullptr, "Tips", "Port is not open");
	}
	else//ascii
	{
		sendBufferASCII = ui.teSendData->toPlainText();

		//д�뻺���� 
		if (serialPort)
			serialPort->write(sendBufferASCII.toLatin1());
		else
            QMessageBox::about(nullptr, "Tips", "Port is not open");
	}
}

void FATool::on_btnClrRcv_clicked()
{
	ui.teReceiveData->clear();
	uartBuffer.clear();
}

void FATool::bgRcvButtonsClicked(int id)
{
	if (bgRcv->checkedId() == 0)//hex
	{
		ui.teReceiveData->clear();
		ui.teReceiveData->append(uartBuffer.toHex());
	}
	else//ascii
	{
		ui.teReceiveData->clear();
		ui.teReceiveData->append(uartBuffer.data());
	}
}

void FATool::bgSendButtonsClicked(int id)
{
	if (bgSend->checkedId() == 0)//hex
	{
		sendBufferASCII = ui.teSendData->toPlainText();

		ui.teSendData->clear();
		ui.teSendData->append(sendBufferHex.toHex());
	}
	else//ascii
	{
		QString sendData = ui.teSendData->toPlainText();
		sendBufferHex = Misc::QString2Hex(sendData);

		ui.teSendData->clear();
		ui.teSendData->append(sendBufferASCII);
	}
}


quint32 FATool::BufferFindCMD(void)
{
	quint32 cmdSize = 0;
	quint32 size;
	char data;
	static quint32 cmdPos = 0;
	static quint16 cmdHead = 0;
	static quint8 cmdLen = 3;

	while (uartBuffer.size())
	{
		data = uartBuffer.at(0);
		uartBuffer.remove(0, 1);

		cmdHead = (cmdHead << 8) | data;

		if (cmdPos == 0)
		{
			if (cmdHead != CMD_HEAD)//ָ���һ���ֽڱ�����֡ͷ����������
				continue;
			else
			{
				dataBuffer.append((char)(CMD_HEAD >> 8));
				cmdPos++;
			}
		}

		dataBuffer.append((char)data);
		cmdPos++;

		if (cmdPos == 3) cmdLen = data + 3;

		//�չ�5�ֽڵõ�����֡
		if (cmdPos >= cmdLen)
		{
			cmdLen = 3;
			cmdSize = cmdPos; //ָ���ֽڳ���
			cmdPos = 0; //��λָ��ָ��
			cmdHead = 0;

			return cmdSize;
		}

	}
	return 0;//û���γ�������һ֡
}

void FATool::ProcessMessage(void)
{
	quint8 cmd = dataBuffer.at(3);
	quint32 temp;
	quint32 index;
	char *ch = dataBuffer.data();

	double a = 0;
	double b = 0;

	uint8_t maxNum;
	int32_t maxPos[3];

	switch (cmd)
	{
	case 0x81: //ϵͳ����
	{
		quint8 addr = dataBuffer.at(4);
		quint8 len = dataBuffer.at(5);

		//if (addr == 0x20)//RTC
		//{
		//	quint8 year = dataBuffer.at(6);
		//	quint8 mon = dataBuffer.at(7);
		//	quint8 date = dataBuffer.at(8);
		//	quint32 d = hmiDriver->BCD2Int(year) * 365
		//		+ hmiDriver->BCD2Int(mon) * 12
		//		+ hmiDriver->BCD2Int(date);

		//	qDebug() << "d = " << d;
		//}
	}
	break;
	case 0x83: //�û�����
	{
		quint16 addr = dataBuffer.at(4) << 8 | dataBuffer.at(5);
		quint8 len = dataBuffer.at(6);

		if (addr == 0x000C)
			qDebug() << "read data is : " << dataBuffer.data() + 7;
	}
	break;


	case 0x8A: //ADC data
		temp = (quint32)(quint8)dataBuffer.at(7) + ((quint32)(quint8)dataBuffer.at(6) << 8) + ((quint32)(quint8)dataBuffer.at(5) << 16) + ((quint32)(quint8)dataBuffer.at(4) << 24);
		//temp = (uint32)(uint8)dataBuffer.at(4) + ((uint32)(uint8)dataBuffer.at(5) << 8) + ((uint32)(uint8)dataBuffer.at(6) << 16) + ((uint32)(uint8)dataBuffer.at(7) << 24);

		//index = (uint16)(uint8)dataBuffer.at(9) + ((uint16)(uint8)dataBuffer.at(8) << 8);
		//index = (uint32)(uint8)dataBuffer.at(11) + ((uint32)(uint8)dataBuffer.at(10) << 8) + ((uint32)(uint8)dataBuffer.at(9) << 16) + ((uint32)(uint8)dataBuffer.at(8) << 24);

		//qDebug() << "index = " << index;
		//qDebug() << "temp = " << temp;
		//adcData.append(QPointF(index, (qreal)temp));
		//plotData.append(QPointF(adcDataIndex++, (qreal)temp));
		//plotData.append(QPointF((qreal)index, (qreal)temp));
		adcData.append(temp);
		break;
	case 0x8B:
		adcData.clear();
		filterData.clear();
		adcData2.clear();
		filterData2.clear();
		adcData3.clear();

		adcData4.clear();
		adcData5.clear();
		plotData2.clear();
		plotData3.clear();

		plotData.clear();
		break;
	case 0x8C:
		adcDataIndex = 0;
		dataBuffer.clear();

#define FILTER_LIST_MAX 0
		//�˲�
		for (quint16 i = 0; i < adcData.count(); i++)
		{
			if (filterData.count() > FILTER_LIST_MAX)
				filterData.removeFirst();
			filterData.append(adcData.at(i));

			quint32 temp = Misc::orderFilter(filterData, filterData.count());

			plotData.append(QPointF(i, (qreal)adcData.at(i)));
			//plotData2.append(QPointF(i, (qreal)temp));

			//if(i < adcData.count()/2)
			adcData2.append(temp);
		}
		//�ڶ����˲�
		//for (uint16 i = 0; i < adcData2.count(); i++)
		//{
		//	if (filterData2.count() > FILTER_LIST_MAX)
		//		filterData2.removeFirst();
		//	filterData2.append(adcData2.at(i));

		//	uint32 temp = orderFilter(filterData2, filterData2.count());

		//	plotData3.append(QPointF(i, (qreal)temp));
		//	adcData3.append(temp);
		//}

		//�������
		Misc::LeastSquare(adcData2, &a, &b);

		qDebug() << "a = " << a;
		qDebug() << "b = " << b;

		for (quint16 i = 0; i < adcData2.count(); i++)
		{
			adcData4.append(a*i + b);
			plotData2.append(QPointF(i, (qreal)adcData4.at(i)));

			if (adcData2.at(i) < a*i + b)
				adcData3.append(adcData2.at(i));
			else
				adcData3.append(a*i + b);
		}


		//�������
		Misc::LeastSquare(adcData3, &a, &b);

		qDebug() << "a = " << a;
		qDebug() << "b = " << b;

		for (quint16 i = 0; i < adcData3.count(); i++)
		{
			adcData5.append(a*i + b);
			plotData3.append(QPointF(i, (qreal)adcData5.at(i)));
		}


		//for (uint16 i = 0; i < adcData3.count(); i++)
		//{
		//	adcData5.append(adcData3.at(i) - adcData4.at(i));
		//	plotData3.append(QPointF(i, (qreal)adcData5.at(i)));
		//}

		//for (uint16 i = 0; i < adcData5.count(); i++)
		//	plotData.append(QPointF(i, (qreal)adcData5.at(i)));

		plotter.setCurveData(0, plotData);
		//plotter.setCurveData(1, plotData2);
		plotter.setCurveData(2, plotData3);
		plotter.show();

		//������
		/*maxNum = MaxPoint(adcData5, maxPos, 100000);
		qDebug() << "maxNum = " << maxNum;*/
		//{
		//	for (uint8_t i = 0; i < maxNum; i++)
		//	{
		//		QVector<QPointF> plotData4;
		//		plotData4.clear();
		//		plotData4.append(QPointF(maxPos[i], 0));
		//		plotData4.append(QPointF(maxPos[i], (qreal)adcData5.at(maxPos[i])));
		//		plotter.setCurveData(3+i, plotData4);
		//	}
		//}

		//��ƽ��ֵ
#define AVERAGE_WIN	10
		//for (uint8_t i = 0; i < maxNum; i++)
		//{
		//	uint64_t average = 0;
		//	uint32_t index = maxPos[i]- AVERAGE_WIN;
		//	for (uint8_t j = 0; j < 2 * AVERAGE_WIN; j++)
		//		average += adcData5.at(index + j);
		//	average /= 2 * AVERAGE_WIN;

		//	QVector<QPointF> plotData4;
		//	plotData4.clear();
		//	plotData4.append(QPointF(maxPos[i], 0));
		//	plotData4.append(QPointF(maxPos[i], (qreal)average));
		//	plotter.setCurveData(3 + i, plotData4);

		//	qDebug() << "maxPoint" << i << "(" << maxPos[i] << ")" << " = " << average;
		//}

		//{
		//	QFile file("E:\\GitHubRepository\\CPAP\\CPAP_SimMCULogic\\CPAP_SimMCULogic\\RawData.txt");
		//	file.open(QIODevice::WriteOnly | QIODevice::Text);
		//	QTextStream stream(&file);
		//	for (uint16 i = 0; i < adcData.count(); i++)
		//	{
		//		stream << adcData.at(i) << "\n";
		//	}
		//	file.close();
		//}


		//{
		//	QFile file("E:\\GitHubRepository\\CPAP\\CPAP_SimMCULogic\\CPAP_SimMCULogic\\TestData.txt");
		//	file.open(QIODevice::WriteOnly | QIODevice::Text);
		//	QTextStream stream(&file);

		//	double vec[512];
		//	int len = sizeof(vec) / sizeof(double);
		//	quint16 i = 0;
		//	//for (i = 0; i < len; i++)
		//	//	vec[i] = adcData.at(i);
		//	for (i = 0; i < len; i++)
		//	{
		//		vec[i] = sin(2 * 3.1416 * 20 / 50 * i) + sin(2 * 3.1416 * 2 / 50 * i) + sin(2 * 3.1416 * 11 / 50 * i);
		//		//vec[i] = sin(i / 2 / 3.1416);
		//		plotData.append(QPointF(i, (qreal)vec[i]));

		//		stream << vec[i] << "\n";
		//		
		//	}
		//	file.close();
		//	plotter.setCurveData(0, plotData);

		//	Complex *inVec = new Complex[len];
		//	Complex *outVec = new Complex[len];
		//	Complex *invert = new Complex[len];

		//	memset(inVec, 0, len * sizeof(Complex));
		//	for (int i = 0; i < len; i++)
		//		inVec[i].rl = vec[i];

		//	// Fourier transformation
		//	t.fft(inVec, len, outVec);

		//	//t.ifft(outVec, len, invert);

		//	for (uint16 i = 0; i < len/2; i++)
		//	{
		//		qreal power = sqrt(outVec[i].rl*outVec[i].rl + outVec[i].im*outVec[i].im);
		//		//qreal power = sqrt(inVec[i].rl*inVec[i].rl + inVec[i].im*inVec[i].im);
		//		plotData3.append(QPointF(i, power));
		//	}
		//	plotter.setCurveData(1, plotData3);
		//}

		//plotter.show();

		break;
	default:
		break;
	}

	dataBuffer.clear();
}

void FATool::UartDataParse()
{
	qint32 size;

	//������������
	size = BufferFindCMD(); //�ӻ������л�ȡһ��ָ��        
	if (size > 0)//���յ�ָ��
	{
		ProcessMessage();//ָ���
	}
}
