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

//#define SOH 0x01 //���⿪ʼ
//#define STX 0x02 //���Ŀ�ʼ
//#define US	',' //��Ԫ�ָ���
//#define ETX 0x03 //���Ľ���
//#define EOT 0x04 //�������

#define SOH	'!' //���⿪ʼ
#define STX '@' //���Ŀ�ʼ
#define US	',' //��Ԫ�ָ���
#define ETX '#' //���Ľ���
#define EOT '$' //�������

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

	str.append(SOH);//��ʼ

	//��Ŀ���ơ�����
	str.append(STX).append(PROJECT_NAME).append(US).append(ui.leProjectName->text()).append(ETX);
	str.append(STX).append(BATCH_NUMBER).append(US).append(ui.leBatchNumber->text()).append(ETX);

	//ʧЧ����
	str.append(STX).append(INVALIDE_DATA).append(US).append(ui.leInvalidData->text()).append(ETX);

	//ɨ������
	index = ui.cbScanLineNum->currentIndex() + 0x30;
	str.append(STX).append(SCANLINE_NUM).append(US).append(index).append(ETX);
	//str.append(STX).append(ui.cbScanLineNum->currentText()).append(ETX);

	//C�ߡ�T��λ��
	str.append(STX).append(CLINE_POSITION).append(US).append(ui.leCLinePosition->text()).append(ETX);
	str.append(STX).append(TLINE_POSITION).append(US).append(ui.leTLinePosition->text()).append(ETX);

	//��ⴰ��
	str.append(STX).append(CHECK_WINDOW).append(US).append(ui.leCheckWindow->text()).append(ETX);

	//��������
	index = ui.cbSampleType->currentIndex() + 0x30;
	str.append(STX).append(SAMPLE_TYPE).append(US).append(index).append(ETX);

	//Ũ�ȵ�λ
	str.append(STX).append(CONCEN_UNIT).append(US).append(ui.leConcentrationUnit->text()).append(ETX);

	//��ⷶΧ
	str.append(STX).append(DETECT_RANGE_L).append(US).append(ui.leDetectionRangeLow->text()).append(ETX);
	str.append(STX).append(DETECT_RANGE_H).append(US).append(ui.leDetectionRangeHigh->text()).append(ETX);

	//�ο���Χ
	str.append(STX).append(REFER_RANGE_L).append(US).append(ui.leReferenceRangeLow->text()).append(ETX);
	str.append(STX).append(REFER_RANGE_H).append(US).append(ui.leReferenceRangeHigh->text()).append(ETX);

	//�궨��ʽ
	index = ui.cbFittingType->currentIndex() + 0x30;
	str.append(STX).append(FITTING_TYPE).append(US).append(index).append(ETX);

	//���ģ��
	index = ui.cbFittingModel->currentIndex() + 0x30;
	str.append(STX).append(FITTING_MODEL).append(US).append(index).append(ETX);

	//��ϲ���
	switch (ui.cbFittingModel->currentIndex())
	{
	case 0://ֱ��
	case 3://logit-logֱ�߻ع�
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		break;
	case 1://���ζ���ʽ
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		str.append(STX).append(FITTING_PARA3).append(US).append(ui.leParameter3->text()).append(ETX);
		break;
	case 2://���ζ���ʽ
	case 4://�Ĳ�����ϻع�
		str.append(STX).append(FITTING_PARA1).append(US).append(ui.leParameter1->text()).append(ETX);
		str.append(STX).append(FITTING_PARA2).append(US).append(ui.leParameter2->text()).append(ETX);
		str.append(STX).append(FITTING_PARA3).append(US).append(ui.leParameter3->text()).append(ETX);
		str.append(STX).append(FITTING_PARA4).append(US).append(ui.leParameter4->text()).append(ETX);
		break;
	default:
		break;
	}

	str.append(EOT);//����

	qDebug() << str;

	qrImage->setString(str);
	qrImage->show();
}

void ParaQRGenerator::ComboBoxIndexChanged(int index)
{
	switch (index)
	{
	case 0://ֱ��
		ui.lbFormula->setText("y = A*x + B");
		ui.lbParameter3->setVisible(0);
		ui.leParameter3->setVisible(0);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 1://���ζ���ʽ
		ui.lbFormula->setText("y = A*x^2 + B*x + C");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 2://���ζ���ʽ
		ui.lbFormula->setText("y = A*x^3 + B*x^2 + C*x + D");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(1);
		ui.leParameter4->setVisible(1);
		break;
	case 3://logit-logֱ�߻ع�
		ui.lbFormula->setText("A*lg(y) + B = ln((x/C)/(1-x/C))");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 4://�Ĳ�����ϻع�
		ui.lbFormula->setText("y = (A - D)/(1 + (x/C)^B) + D");
		ui.lbParameter3->setVisible(1);
		ui.leParameter3->setVisible(1);
		ui.lbParameter4->setVisible(1);
		ui.leParameter4->setVisible(1);
		break;
	case 5://����������ֵ
		ui.lbFormula->setText("");
		ui.lbParameter3->setVisible(0);
		ui.leParameter3->setVisible(0);
		ui.lbParameter4->setVisible(0);
		ui.leParameter4->setVisible(0);
		break;
	case 6://��Ե����
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


