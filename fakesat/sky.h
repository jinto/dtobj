/**
 * Fake Sat
 */

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

class Sky : public QWidget
{
	Q_OBJECT

public:
	Sky(char* imgfile,QWidget *parent = 0) ;

protected:
	void keyPressEvent(QKeyEvent * event);
	void paintEvent(QPaintEvent *event);

private:
	int pos_x;
	int pos_y;

private slots:
	 void moveSat();
};

