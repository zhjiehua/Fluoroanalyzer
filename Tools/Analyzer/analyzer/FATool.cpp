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

	InitPort();//初始化串口

	//定时读取串口数据
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


/****************************串口初始化*****************************/
void FATool::InitPort(void)
{
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";
	qDebug() << "InitPort\r\n";

	//读取串口信息  
	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		qDebug() << "Name:" << info.portName();
		qDebug() << "Description:" << info.description();
		qDebug() << "Manufacturer:" << info.manufacturer();

		//这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去  
		QSerialPort serial;
		serial.setPort(info);
//		if (serial.open(QIODevice::ReadWrite))
		{
			//将串口号添加到cmb  
			ui.cmbPortName->addItem(info.portName());
			//关闭串口等待人为(打开串口按钮)打开  
//			serial.close();
		}
	}

	QStringList baudList;//波特率  
	QStringList parityList;//校验位  
	QStringList dataBitsList;//数据位  
	QStringList stopBitsList;//停止位  

	baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300"
		<< "600" << "1200" << "1800" << "2400" << "4800" << "9600"
		<< "14400" << "19200" << "38400" << "56000" << "57600"
		<< "76800" << "115200" << "128000" << "256000";

	ui.cmbBaudRate->addItems(baudList);
	ui.cmbBaudRate->setCurrentIndex(19); //115200
										 //ui.cmbBaudRate->setCurrentIndex(12); //9600

	parityList << QString::fromLocal8Bit("无") << QString::fromLocal8Bit("奇") << QString::fromLocal8Bit("偶");
	parityList << QString::fromLocal8Bit("标志");
	parityList << QString::fromLocal8Bit("空格");

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

	//设置按钮可以被按下  
	ui.btnOpen->setCheckable(true);

	ui.teReceiveData->setReadOnly(true);
}

/****************************串口设置******************************/
void FATool::on_btnOpen_clicked()
{
	if (ui.btnOpen->text() == QString::fromLocal8Bit("打开串口"))
	{
		serialPort = new QSerialPort(this);

		//设置串口号  
		serialPort->setPortName(ui.cmbPortName->currentText());
		//以读写方式打开串口  
		if (serialPort->open(QIODevice::ReadWrite))
		{
			//设置波特率  
			serialPort->setBaudRate(ui.cmbBaudRate->currentText().toInt());
			//设置数据位  
			serialPort->setDataBits(QSerialPort::Data8);
			//设置校验位  
			serialPort->setParity(QSerialPort::NoParity);
			//设置流控制  
			serialPort->setFlowControl(QSerialPort::NoFlowControl);
			//设置停止位  
			serialPort->setStopBits(QSerialPort::OneStop);

			//每秒读一次  
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

	//测试FFT
	//FFT_Test();

}

void FATool::SetNonSelectable(void)
{
	ui.btnOpen->setText(QString::fromLocal8Bit("关闭串口"));

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
	ui.btnOpen->setText(QString::fromLocal8Bit("打开串口"));

	ui.cmbPortName->setEditable(true);
	ui.cmbBaudRate->setEditable(true);
	ui.cmbDataBits->setEditable(true);
	ui.cmbParity->setEditable(true);
	ui.cmbStopBits->setEditable(true);
	ui.cmbFlowControl->setEditable(true);

	ui.cmbPortName->showPopup();
}

/****************************数据传输******************************/
void FATool::ReadComDataSlot()
{
	//读取串口数据  
	QByteArray readComData = serialPort->readAll();

	//将读到的数据显示到数据接收区的te中  
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

	//清除缓冲区  
	readComData.clear();
}

void FATool::on_btnSend_clicked()
{
	if (bgSend->checkedId() == 0)//hex
	{
		QString sendData = ui.teSendData->toPlainText();
		sendBufferHex = Misc::QString2Hex(sendData);

		//写入缓冲区
		if (serialPort)
			serialPort->write(sendBufferHex);
		else
            QMessageBox::about(nullptr, "Tips", "Port is not open");
	}
	else//ascii
	{
		sendBufferASCII = ui.teSendData->toPlainText();

		//写入缓冲区 
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
			if (cmdHead != CMD_HEAD)//指令第一个字节必须是帧头，否则跳过
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

		//凑够5字节得到完整帧
		if (cmdPos >= cmdLen)
		{
			cmdLen = 3;
			cmdSize = cmdPos; //指令字节长度
			cmdPos = 0; //复位指令指针
			cmdHead = 0;

			return cmdSize;
		}

	}
	return 0;//没有形成完整的一帧
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
	case 0x81: //系统变量
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
	case 0x83: //用户变量
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
		//滤波
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
		//第二次滤波
		//for (uint16 i = 0; i < adcData2.count(); i++)
		//{
		//	if (filterData2.count() > FILTER_LIST_MAX)
		//		filterData2.removeFirst();
		//	filterData2.append(adcData2.at(i));

		//	uint32 temp = orderFilter(filterData2, filterData2.count());

		//	plotData3.append(QPointF(i, (qreal)temp));
		//	adcData3.append(temp);
		//}

		//基线拟合
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


		//基线拟合
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

		//找最大点
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

		//求平均值
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

	//解析串口数据
	size = BufferFindCMD(); //从缓冲区中获取一条指令        
	if (size > 0)//接收到指令
	{
		ProcessMessage();//指令处理
	}
}
