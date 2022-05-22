#include "mygridlayout.h"
#include "cardbutton.h"
#include "mytimer.h"
#include "mytimer2.h"
#include "mytimer3.h"
#include <QPushButton>
#include <QTimer>
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <algorithm>
#include <random>

MyGridLayout::MyGridLayout(QTimer *timer, QTimer *timer2, QLabel *scorelabel):QGridLayout()
{
    this->timer180 = new MyTimer3();
    this->score=0;
    this->timer = timer;
    this->timer2 = timer2;
    this->scorelabel = scorelabel;
    this->card1=-1;// coordinates of the first clicked of the pair.
    this->card2=-1;// coordinates of the second clicked of the pair.
    this->checking = 0; // check if two cards are clicked right now.

    timer180->timer->start(1000);
    QObject::connect(timer180->timer, SIGNAL(timeout()), this, SLOT(check_lose_disable()));

    this->timer->start(1000);
    //std::vector<int> clicked_cards;
    int current_coordinate;
    //randomization seed generator.
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    //auto rng = std::default_random_engine {};
    std::vector<QString> cards_{"ali","veli","mehmet","osman","mustafa",
                                "canberk","alper","fatma","ece","necla",
                                "cansu","nalan","ibrahim","orhan","selim",
                                "ali","veli","mehmet","osman","mustafa",
                                "canberk","alper","fatma","ece","necla",
                                "cansu","nalan","ibrahim","orhan","selim"};
    std::shuffle(std::begin(cards_), std::end(cards_), e);//Randomized list of words.

    QString org_text;//text on the card.

    for(int row=0; row<5; row++){
        for(int col=0; col<6; col++){
            current_coordinate = row*6+col;
            QString org_text = cards_[current_coordinate];
            CardButton *cb = new CardButton(current_coordinate,org_text);

            this->addWidget(cb, row, col, 1, 1);
            QObject::connect(cb, SIGNAL(clicked()), this, SLOT(check_pairs()));
            QObject::connect(cb, SIGNAL(clicked()), this, SLOT(check_win()));
        }
    }

}

void MyGridLayout::check_lose_disable(){//if 180 seconds past. game is lost, disable all the cards.
    if(this->timer180->counter>=180){

        for(int i=0; i<30; i++){
            qobject_cast<CardButton*>(this->itemAt(i)->widget())->setDisabled(true);
            this->timer180->counter = 0;
        }
    }
}

void MyGridLayout::check_win(){//checks all the cards whether they are matched. if they are, game is won.

    bool all_matched = true;
    for(int i=0; i<30;i++){
        CardButton *cb = qobject_cast<CardButton*>(this->itemAt(i)->widget());
        if(!cb->isMatched){
            all_matched = false;
        }

    }
    if(all_matched){
        this->timer->stop();
        QMessageBox *qm = new QMessageBox();
        qm->setText("You won!");
        qm->setStandardButtons(QMessageBox::Cancel);
        qm->exec();
    }

}

void MyGridLayout::check_pairs(){//checks the clicked pair whether they are the same.
    //if they are, disable them, and close them. eliminate the cards.

    for(int i=0;i<30;i++){

        if(qobject_cast<CardButton*>(this->itemAt(i)->widget())->isClicked){

            if(this->card1 == -1)
                this->card1=i;
            else if(this->card2 == -1)
                this->card2=i;
        }
    }

    this->checking++;

    if(this->checking==2){//if clicked item is the second of the pair, show it for 1 second and check if matched.
        MyTimer2 *mt3 = new MyTimer2();
        mt3->timer->start(1000);
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->setDisabled(true);


        QObject::connect(mt3->timer, SIGNAL(timeout()), this, SLOT(check_pairs_continued()));

    }else if(this->checking == 1){ // if clicked item is the first of the pair, show it.
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->setDisabled(true);
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->isClicked = false;

    }
}
void MyGridLayout::check_pairs_continued(){//check whether the pair is matched
    if(qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->original_text ==
            qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->original_text){//if they are matched
        score++;
        scorelabel->setText("Score: "+ QString::number(score));

        //if pairs are matched. disable and close them. increment score.
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->setText("");
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->setText("");
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->setDisabled(true);
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->setDisabled(true);
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->isClicked = false;
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->isClicked = false;
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->isMatched = true;
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->isMatched = true;

        this->checking=0;
        this->card1 = -1;
        this->card2 = -1;
    }else{//if it is not a a match, close them and enable them.
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->setDisabled(false);
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->setText("?");
        qobject_cast<CardButton*>(this->itemAt(this->card1)->widget())->isClicked = false;

        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->setDisabled(false);
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->setText("?");
        qobject_cast<CardButton*>(this->itemAt(this->card2)->widget())->isClicked = false;
        //this->clicked_cards.clear();
        this->checking=0;
        this->card1 = -1;
        this->card2 = -1;

    }
}


void MyGridLayout::new_game_clicked(){//create a new game. Reset the buttons, time, and score.


    this->score=0;


    this->scorelabel->setText("Score: "+ QString::number(score));
    this->card1=-1;
    this->card2=-1;
    this->checking = 0;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);
    //auto rng = std::default_random_engine {};
    std::vector<QString> cards_{"ali","veli","mehmet","osman","mustafa",
                                "canberk","alper","fatma","ece","necla",
                                "cansu","nalan","ibrahim","orhan","selim",
                                "ali","veli","mehmet","osman","mustafa",
                                "canberk","alper","fatma","ece","necla",
                                "cansu","nalan","ibrahim","orhan","selim"};
    std::shuffle(std::begin(cards_), std::end(cards_), e);//RANDOMU DUZELT CANBERK

    QString org_text;

    for(int i=0; i<30; i++){


        QString org_text = cards_[i];
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->setDisabled(false);
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->original_text = org_text;
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->showed_text = "?";
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->isMatched = false;
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->isClicked = false;
        qobject_cast<CardButton*>(this->itemAt(i)->widget())->setText("?");


    }
}
