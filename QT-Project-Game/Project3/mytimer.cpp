#include "mytimer.h"

MyTimer::MyTimer() : QObject()
{//this is the main timer.
    this->counter = 0;
    this->label = new QLabel("Time(secs): 0");
    this->timer = new QTimer();

    QObject::connect(this->timer, SIGNAL(timeout()), this, SLOT(MyTimerSlot()));



}

void MyTimer::MyTimerSlot(){
    this->counter+=1;
    this->label->setText("Time(secs): "+ QString::number(this->counter));
    if(this->counter >=180){
        this->timer->stop();
        QMessageBox *qm = new QMessageBox();
        qm->setText("You failed!");
        qm->setStandardButtons(QMessageBox::Cancel);
        qm->exec();
    }
}

void MyTimer::new_game(){
    this->timer->stop();
    this->counter = 0;
    this->label->setText("Time(secs):"+ QString::number(this->counter));
    this->timer->start(1000);
}
