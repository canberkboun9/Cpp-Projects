#include "mytimer3.h"

MyTimer3::MyTimer3() : QObject()
{//this timer is only created to disable all the cards after the game finished.
    this->counter = 0;

    this->timer = new QTimer();

    QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(MyTimerSlot()));



}


void MyTimer3::MyTimerSlot(){
    this->counter+=1;

    if(this->counter >=180){
        this->timer->stop();

    }
}

