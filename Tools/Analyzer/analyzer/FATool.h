#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_FATool.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "Plotter.h"

class FATool : public QMainWindow
{
	Q_OBJECT

public:
	FATool(QWidget *parent = Q_NULLPTR);


	void InitPort(void);
	void SetNonSelectable(void);
	void SetSelectable(void);

	quint32 BufferFindCMD(void);
	void ProcessMessage(void);

	//void FFT_Test(void);

public slots:
	void on_btnOpen_clicked();   //
	void on_btnSend_clicked();
	void on_btnClrRcv_clicked();

	void bgRcvButtonsClicked(int id);
	void bgSendButtonsClicked(int id);

	void ReadComDataSlot();   //
	void UartDataParse();

	///*********************************************************************/
	//void on_btnOxgInc_clicked();
	//void on_btnOxgDec_clicked();
	//void on_btnTmpInc_clicked();
	//void on_btnTmpDec_clicked();
	//void on_btnStxStp_clicked();
	//void on_btnCmb_clicked();
	//void on_btnStpBpr_clicked();

	//void on_btnChangePage_clicked();
	//void on_btnFontColorChange_clicked();

	//void on_btnWriteValue_clicked();
	//void on_btnReadValue_clicked();

	//void on_btnPrint_clicked();

	//void on_btnInitQRModel_clicked();
	/*********************************************************************/

private:
	Ui::FAToolClass ui;

	QButtonGroup *bgRcv;
	QButtonGroup *bgSend;

	QSerialPort *serialPort;
	QTimer *timer;
	QTimer *mainTimer;
	QByteArray dataBuffer;
	QByteArray uartBuffer;
	QByteArray sendBufferHex;
	QString sendBufferASCII;

	//HMI_Printer_Driver *thermalPrinter;

	Plotter plotter;

	QVector<QPointF> plotData;
	QVector<QPointF> plotData2;
	QVector<QPointF> plotData3;
	QVector<qint32> adcData;
	QVector<qint32> filterData;
	QVector<qint32> adcData2;
	QVector<qint32> filterData2;
	QVector<qint32> adcData3;
	QVector<qint32> adcData4;//ÄâºÏ»ùÏß
	QVector<qint32> adcData5;
	qint32 adcDataIndex;

	//CFft1 t;
};
