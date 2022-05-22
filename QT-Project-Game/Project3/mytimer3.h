#ifndef MYTIMER3_H
#define MYTIMER3_H
#include <QTimer>
#include <QLabel>
#include <QMessageBox>


class MyTimer3 : public QObject
{

Q_OBJECT

public:
    MyTimer3();
    QTimer *timer;

    int counter;

public slots:
    void MyTimerSlot();


};

#endif // MYTIMER3_H
