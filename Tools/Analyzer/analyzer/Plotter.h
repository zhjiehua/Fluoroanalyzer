#ifndef PLOTTER_H
#define PLOTTER_H 
#include <QMap>//������Qt��ͷ�ļ�
#include <QPixmap>
#include <QVector>
#include <QWidget> 
class QToolButton; //����ǰ������
class PlotSettings;  
class Plotter : public QWidget
{
	Q_OBJECT
public:
	Plotter(QWidget *parent = 0);
	void setPlotSettings(const PlotSettings &settings);
	void setCurveData(int id, const QVector<QPointF> &data);//QPointF�����Ǹ������͵�QPoint
	void clearCurve(int id);
	QSize minimumSizeHint() const; //��дQWidget::minimumSizeHint()
	QSize sizeHint() const;        //��дQWidget::sizeHint()
public slots:
	void zoomIn();   //�Ŵ�����
	void zoomOut();   //��С��ʾ����
protected:  //����ʵ�ֵ�QWidget���¼�������
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *event);
private:
	void updateRubberBandRegion();
	void refreshPixmap();
	void drawGrid(QPainter *painter);
	void drawCurves(QPainter *painter);
	enum { Margin = 50 };//����ͼ����Χ�Ŀռ�
	QToolButton *zoomInButton;
	QToolButton *zoomOutButton;
	QMap<int, QVector<QPointF> > curveMap;  //��������
	QVector<PlotSettings> zoomStack;   //PlotSettingsջ����
	int curZoom;
	bool rubberBandIsShown;
	QRect rubberBandRect;
	QPixmap pixmap; //��ʾ����Ļ�Ŀؼ���һ���������κλ��������Ȼ���������pixmap�ϣ�Ȼ�󿽱����ؼ���
};
//PlotSettingsȷ��x��y��ķ�Χ���Ϳ̶ȵĸ���
class PlotSettings
{
public:
	PlotSettings();

	void scroll(int dx, int dy);
	void adjust();
	double spanX() const { return maxX - minX; }
	double spanY() const { return maxY - minY; }

	double minX;
	double maxX;
	int numXTicks;
	double minY;
	double maxY;
	int numYTicks;

private:
	static void adjustAxis(double &min, double &max, int &numTicks);
};

#endif
