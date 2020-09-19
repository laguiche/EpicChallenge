#ifndef EPICCHALLENGE_H
#define EPICCHALLENGE_H

#include <QMainWindow>
#include <stdlib.h>
#include <QStringList>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QDate>

#define OK 1
#define NOK 0
#define _HIGH_CHALLENGE_PATH_ "./challenges/highChallenge.txt"
#define _MEDIUM_CHALLENGE_PATH_ "./challenges/mediumChallenge.txt"
#define _LOW_CHALLENGE_PATH_ "./challenges/lowChallenge.txt"

#ifdef ON_PI
    #include <MFRC522.h>
#endif

namespace Ui {
class EpicChallenge;
}

class EpicChallenge : public QMainWindow
{
    Q_OBJECT

public:
    explicit EpicChallenge(QWidget *parent = 0);
    ~EpicChallenge();

private:
#ifdef ON_PI
    MFRC522 mfrc;
#endif
    Ui::EpicChallenge *ui;
    QStringList m_highChallenges;
    QStringList m_mediumChallenges;
    QStringList m_lowChallenges;
    QSqlDatabase m_db;
    bool m_sportMode;
    QTime t_0;
    QTimer * m_cadenceur;

    void init();
    unsigned char loadChallenges(QString filePath);
    unsigned char setDataBase();
    void getChallenge(int id, bool mode);

    unsigned char m_01[3]={0xDE,0xD9,0x91};
    unsigned char m_02[3]={0x39,0x59,0xF9};

private slots:
    void on_challengeButton(void);
};

#endif // EPICCHALLENGE_H
