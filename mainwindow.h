#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFont>
#include <QGraphicsTextItem>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextCursor>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QKeyEvent>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHBoxLayout>
#include <QRectF>
#include <QComboBox>
#include <QStatusBar>
#include <QTimer>
#include <QTime>
#include <QLabel>
#include <QString>
#include <xkbcommon/xkbcommon.h>
#include <QGraphicsRectItem>
#include "keybutton.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createStatusBar();

    void nextLetter(QKeyEvent *key);
    void keyPressEvent(QKeyEvent *event) override;
    void generateText();
    void updateText();
    void updateTimeLabel();
    void startCursor();

    void createdefaultTexts();

    void createKeyboard(std::string lan);


private:
    QList<KeyButton*> buttons;

    QGraphicsView *view;
    QGraphicsScene *scene;
    QStringList defaultTexts;

    QComboBox *languages;
    QPushButton *restartButton;
    QLabel *timeLabel;
    QLabel *charPerSecond;
    QLabel *wordPerSecond;
    int countWords;
    int countChars;

    QTimer *timer;
    QTime deltaTime;
    bool statedWrite = 0;

    QTextCursor *cursor;

    QNetworkReply* reply;
    QGraphicsTextItem *text;
    QNetworkAccessManager manager;
    bool completeReply = 1;
};
#endif // MAINWINDOW_H
