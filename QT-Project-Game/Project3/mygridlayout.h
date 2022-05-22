#ifndef MYGRIDLAYOUT_H
#define MYGRIDLAYOUT_H
#include <QGridLayout>
#include <QTimer>
#include <QMessageBox>
#include <vector>
#include <QWidget>
#include <QVBoxLayout>
#include <QSignalMapper>
#include <QSpacerItem>
#include <algorithm>
#include <random>
#include "mytimer.h"
#include "mytimer2.h"
#include "mytimer3.h"
#include "cardbutton.h"


class MyGridLayout : public QGridLayout
{
    Q_OBJECT
public:
    MyGridLayout(QTimer *timer, QTimer *timer2,QLabel *scorelabel);

    QTimer *timer;
    QTimer *timer2;

    MyTimer3 *timer180;
    //std::vector<int> clicked_cards;
    //QSignalMapper *signalMapper;
    int current_coordinate;
    int score;
    int card1;
    int card2;
    int checking;
    QLabel *scorelabel;
    //std::vector <QPushButton*> cardsCreated;

public slots:
    void check_lose_disable();
    void check_win();
    void check_pairs();
    void check_pairs_continued();
    void new_game_clicked();
};

#endif // MYGRIDLAYOUT_H
