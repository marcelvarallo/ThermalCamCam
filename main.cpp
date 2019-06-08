#include <QApplication>
#include <QThread>
#include <QMutex>
#include <QMessageBox>

#include <QColor>
#include <QLabel>
#include <QtDebug>
#include <QString>
#include <QPushButton>

#include "LeptonThread.h"
#include "OutlineLabel.h"
#include "MyLabel.h"
int snapshotCount = 0;
int rawMin;
int rawMax;

int main( int argc, char **argv )
{
	//create the app
	QApplication a( argc, argv );
	
	QWidget *myWidget = new QWidget;
	myWidget->setGeometry(0, 0, 480, 350);

	//create an image placeholder for myLabel
	//fill the top left corner with red, just bcuz
	QImage myImage;
	myImage = QImage(400, 300, QImage::Format_RGB888);
	QRgb red = qRgb(255,0,0);
	for(int i=0;i<80;i++) {
		for(int j=0;j<60;j++) {
			myImage.setPixel(i, j, red);
		}
	}

	//create a label, and set it's image to the placeholder
	MyLabel myLabel(myWidget);
	myLabel.setGeometry(10, 10, 400, 300);
	myLabel.setPixmap(QPixmap::fromImage(myImage));
	
	QPushButton *button5 = new QPushButton("WebCam", myWidget);
	button5->setGeometry(410, 80, 80, 50);
		
	QPushButton *button4 = new QPushButton("Freeze", myWidget);
	button4->setGeometry(410, 128.5, 80, 50);

	//create a FFC button
	QPushButton *button1 = new QPushButton("FFC Cal", myWidget);
	button1->setGeometry(410, 270, 80, 50);

	//create a thread to gather SPI data
	//when the thread emits updateImage, the label should update its image accordingly
	LeptonThread *thread = new LeptonThread();
	QObject::connect(thread, SIGNAL(updateImage(QImage)), &myLabel, SLOT(setImage(QImage)));
	
	//Label for radiometric data
	OutlineLabel * radData = new OutlineLabel(myWidget);
	radData->setGeometry(320/2 - 15,290/2 - 5, 60, 20);
	QObject::connect(thread, SIGNAL(updateRadiometry(QString)), radData, SLOT(setText(QString)));

	//connect ffc button to the thread's ffc action
	QObject::connect(button1, SIGNAL(clicked()), thread, SLOT(performFFC()));
	QObject::connect(button4, SIGNAL(clicked()), thread, SLOT(rebootMe()));
	QObject::connect(button5, SIGNAL(clicked()), thread, SLOT(webcam()));
        QPushButton *snapshotButton = new QPushButton("LockRange", myWidget);
	snapshotButton->setGeometry(410, 222.5, 80, 50);
	QObject::connect(snapshotButton, SIGNAL(clicked()), thread, SLOT(saveSnapshot()));
	//connect(snapshotButton, SIGNAL(clicked()), this, SLOT(saveSnapshot()));

        QPushButton *ColorButton = new QPushButton("Color", myWidget);
        ColorButton->setGeometry(410, 175, 80, 50);
	QObject::connect(ColorButton, SIGNAL(clicked()), thread, SLOT(colorChange()));
	//rawMin->thread.minValue;
        //rawMax-> thread.maxValue;
	thread->start();
	
	myWidget->show();

	return a.exec();
}
