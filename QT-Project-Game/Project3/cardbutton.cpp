#include "cardbutton.h"

CardButton::CardButton(int coordinate, const QString& text, QWidget* parent):QPushButton(text, parent)
{
    this->original_text = text;//the text card hiding.
    this->showed_text = "?";//the card is closed
    this->when_opened = "";//card has been eliminated
    this->coordinate = coordinate;
    this->setText(showed_text);
    this->isClicked = false; // it is used to check if the card has been clicked.
    this->isMatched = false; // it is used to check if the card has been matched.



    QObject::connect(this, SIGNAL(clicked()), this, SLOT(change_text()));
}
void CardButton::change_text(){//when a card is clicked, turn it over and show the text underneath.
    if(this->showed_text=="?")//a useless check.
        this->showed_text = original_text;

    this->isClicked = true;

    this->setText(showed_text);//change the showed text to the player so it can be seen.

}

