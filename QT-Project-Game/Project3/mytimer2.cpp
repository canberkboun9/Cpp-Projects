#include "mytimer2.h"

MyTimer2::MyTimer2() : QObject()
{//this is the 1 second timer to show the pairs during that time.

    this->timer = new QTimer();
    QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(oneSecondPast()));
}
void MyTimer2::oneSecondPast(){
    this->timer->stop();
}
