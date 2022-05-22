#ifndef CARDBUTTON_H
#define CARDBUTTON_H
#include <QPushButton>
#include <QTimer>


class CardButton : public QPushButton
{
    Q_OBJECT
public:

    CardButton(int coordinate, const QString& text,QWidget* parent=0);
    QString original_text;
    QString showed_text;
    QString when_opened;
    int coordinate;
    bool isClicked;
    bool isMatched;




public slots:
    void change_text();

};

#endif // CARDBUTTON_H
