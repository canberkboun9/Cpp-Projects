#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <algorithm>
#include <random>
#include "mytimer.h"
#include "mytimer2.h"
#include "mygridlayout.h"
#include "cardbutton.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);

    QWidget *cw = new QWidget();
    MyTimer *mt = new MyTimer();
    MyTimer2 *mt2 = new MyTimer2();

    QLabel *scorelabel = new QLabel("Score: 0");
    QPushButton *newGameButton = new QPushButton("New Game");
    QPushButton *quitButton = new QPushButton("Quit");

    MyGridLayout *ml = new MyGridLayout(mt->timer, mt2->timer,scorelabel);

    QObject::connect(newGameButton, SIGNAL(clicked()), mt, SLOT(new_game()));

    QObject::connect(newGameButton, SIGNAL(clicked()), ml, SLOT(new_game_clicked()));
    QObject::connect(quitButton, SIGNAL(clicked()), &app, SLOT(quit()));

    QVBoxLayout *vb = new QVBoxLayout();
    QHBoxLayout *top_shelf = new QHBoxLayout();
    QSpacerItem *qs = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSpacerItem *qs2 = new QSpacerItem(0, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);

    top_shelf->addWidget(mt->label);
    top_shelf->addWidget(scorelabel);
    top_shelf->addSpacerItem(qs);
    top_shelf->addWidget(newGameButton);
    top_shelf->addWidget(quitButton);

    vb->addLayout(top_shelf);
    vb->addLayout(ml);
    vb->addSpacerItem(qs2);


    cw->setLayout(vb);
    cw->setWindowTitle("Card Match Game");
    cw->resize(800, 600);

    cw->show();

    return app.exec();
}
