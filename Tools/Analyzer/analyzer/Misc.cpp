#include "Misc.h"

Misc::Misc(void)
{
	;
}

qint8 Misc::ConvertHexChar(qint8 ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}

//16进制转换函数
QByteArray Misc::QString2Hex(QString str)
{
	QByteArray senddata;
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.length();
	senddata.resize(len / 2);
	char lstr, hstr;
	for (int i = 0; i<len; )
	{
		hstr = str[i].toLatin1();
		if (hstr == ' ')
		{
			i++;
			continue;
		}
		i++;
		if (i >= len)
			break;
		lstr = str[i].toLatin1();
		hexdata = ConvertHexChar(hstr);
		lowhexdata = ConvertHexChar(lstr);
		if ((hexdata == 16) || (lowhexdata == 16))
			break;
		else
			hexdata = hexdata * 16 + lowhexdata;
		i++;
		senddata[hexdatalen] = (char)hexdata;
		hexdatalen++;
	}
	senddata.resize(hexdatalen);
	return senddata;
}

//最小二乘法基线拟合
void Misc::LeastSquare(QVector<qint32> &myList, double *a, double *b)
{
	qint32 len = myList.count();
	qint32 i;
	double xSum = 0, ySum = 0, xySum = 0, xxSum = 0;
	for (i = 0; i < len; i++)
	{
		xSum += i;
		ySum += myList.at(i);
		xySum += i *myList.at(i);
		xxSum += i*i;
	}

	*a = (xySum - xSum*ySum / len) / (xxSum - xSum*xSum / len);
	*b = (xxSum*ySum / len - xySum*xSum / len) / (xxSum - xSum*xSum / len);
	//*a = 0;
	//*b = ySum / len;
}

qint32 Misc::orderFilter(QVector<qint32> &myList, qint32 order)
{
	qint32 average = 0;
	qint32 averageCnt = 0;
	qint32 len = myList.count();
	for (int i = 0; i<len; i++)
	{
		average += (myList.at(i) * (i + 1));
		averageCnt += (i + 1);
	}
	average /= averageCnt;
	return average;
}

/*********************************************************************************
*
*
*
*
**********************************************************************************/


