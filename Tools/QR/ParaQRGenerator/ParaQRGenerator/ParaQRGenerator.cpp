#include "ParaQRGenerator.h"
#include "qdebug.h"

ParaQRGenerator::ParaQRGenerator(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	qrImage = new QRImage();
	qrImage->resize(200, 200);

	connect(ui.cbFittingModel, SIGNAL(currentIndexChanged(int)), this, SLOT(ComboBoxIndexChanged(int)));
}

//#define SOH 0x01 //标题开始
//#define STX 0x02 //正文开始
//#define US	',' //单元分隔符
//#define ETX 0x03 //正文结束
//#define EOT 0x04 //传输结束

#define SOH	'!' //标题开始
#define STX '@' //正文开始
#define US	',' //单元分隔符
#define ETX '#' //正文结束
#define EOT '$' //传输结束

#define PROJECT_NAME	"01"
#define BATCH_NUMBER	"02"
#define INVALIDE_DATA	"03"
#define SCANLINE_NUM	"04"
#define CLINE_POSITION	"05"
#define TLINE_POSITION	"06"
#define CHECK_WINDOW	"07"
#define SAMPLE_TYPE		"08"
#define CONCEN_UNIT		"09"
#define DETECT_RANGE_L	"10"
#define DETECT_RANGE_H	"11"
#define REFER_RANGE_L	"12"
#define REFER_RANGE_H	"13"
#define FITTING_TYPE	"14"
#define FITTING_MODEL	"15"
#define FITTING_PARA1	"16"
#define FITTING_PARA2	"17"
#define FITTING_PARA3	"18"
#define FITTING_PARA4	"19"


void ParaQRGenerator::on_btnGen_clicked()
{
	qrImage->close();
	QString str;
	uint8_t index;

	str.append(SOH);//开始

	//项目名称、批号
	str.append(STX).append(PROJECT_NAME).append(US).append(ui.leProjectName->text()).append(ETX);
	str.append(STX).append(BATCH_NUMBER).append(US).append(ui.leBatchNumber->text()).append(ETX);

	//失效日期
	str.append(STX).append(INVALIDE_DATA).append(US).append(ui.leInvalidData->text()).append(ETX);

	//扫描条数
	index = ui.cbScanLineNum->currentIndex() + 0x30;
	str.append(STX).append(SCANLINE_NUM).append(US).append(index).append(ETX);
	//str.append(STX).append(ui.cbScanLineNum->currentText()).append(ETX);

	//C线、T线位置
	str.append(STX).append(CLINE_POSITION).append(US).append(ui.leCLinePosition->text()).append(ETX);
	str.append(STX).append(TLINE_POSITION).append(US).append(ui.leTLinePosition->text()).append(ETX);

	//检测窗口
	str.append(STX).append(CHECK_WINDOW).append(US).append(ui.leCheckWindow->text()).append(ETX);

	//样本类型
	index = ui.cbSampleType->currentIndex() + 0x30;
	str.append(STX).append(SAMPLE_TYPE).append(US).append(index).append(ETX);

	//浓度单位
	str.append(STX).append(CONCEN_UNIT).append(US).append(ui.leConcentrationUnit->text()).append(ETX);

	//检测范围
	str.append(STX).append(DETECT_RANGE_L).append(US).append(ui.leDetectionRangeLow->text()).append(ETX);
	str.append(STX).append(DETECT_RANGE_H).append(US).append(ui.leDetectionRangeHigh->text()).append(ETX);

	//参考范围
	str.append(STX).append(REFER_RANGE_L).append(US).append(ui.leReferenceRangeLow->text()).append(ETX);
	str.append(STX).append(REFER_RANGE_H).append(US).append(ui.leReferenceRangeHigh->text()).append(ETX);

	//标定方式
	index = ui.cbFittingType->currentIndex() + 0x30;
	str.append(STX).append(FITTING_TYPE).append(US).append(index).append(ETX);

	//拟合模型
	index = ui.cbFittingModel->currentIndex() + 0x30;
	str.append(STX).append(FITTING_MODEL).append(US).append(index).append(ETX);

	//拟合参数
	switch (ui.cbFittingModel->currentIndex())
	{
	case 0://直线
	case 3://logit-log直线回归
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		break;
	case 1://二次多项式
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		str.append(STX).append(FITTING_PARA3).append(US).append(ui.leParameter3->text()).append(ETX);
		break;
	case 2://三次多项式
	case 4://四参数拟合回归
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		str.append(STX).append(FITTING_PARA3).append(US).append(ui.leParameter3->text()).append(ETX);
		str.append(STX).append(FITTING_PARA4).append(US).append(ui.leParameter4->text()).append(ETX);
		break;
	default:
		break;
	}

	str.append(EOT);//结束

	qDebug() << str;

	qrImage->setString(str);
	qrImage->show();
}

void ParaQRGenerator::ComboBoxIndexChanged(int index)
{
	switch (index)
	{
	case 0://直线
		ui.lbFormula->setText("y = A*x + B");
		ui.lbParameter3->setVisible(0);
		ui.leParameter3->setVisible(0);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 1://二次多项式
		ui.lbFormula->setText("y = A*x^2 + B*x + C");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 2://三次多项式
		ui.lbFormula->setText("y = A*x^3 + B*x^2 + C*x + D");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(1);
		ui.leParameter4->setVisible(1);
		break;
	case 3://logit-log直线回归
		ui.lbFormula->setText("A*lg(y) + B = ln((x/C)/(1-x/C))");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 4://四参数拟合回归
		ui.lbFormula->setText("y = (A - D)/(1 + (x/C)^B) + D");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(1);
		ui.leParameter4->setVisible(1);
		break;
	case 5://三次样条插值
		ui.lbFormula->setText("");
		ui.lbParameter3->setVisible(0);
		ui.leParameter3->setVisible(0);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 6://点对点计算
		ui.lbFormula->setText("");
		ui.lbParameter3->setVisible(0);
		ui.leParameter3->setVisible(0);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	default:
		break;
	}
}


