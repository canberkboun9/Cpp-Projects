#ifndef MYTIMER2_H
#define MYTIMER2_H
#include <QTimer>


class MyTimer2 : public QObject
{

Q_OBJECT

public:
    MyTimer2();
    QTimer *timer;


public slots:

    void oneSecondPast();

};

#endif // MYTIMER2_H
