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
#include <QDate>

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
    Ui::EpicChallenge *ui;
    void init();
    QStringList m_challenges;
    QSqlDatabase m_db;
    void loadChallenges();
    bool m_sportMode;
    void setDataBase();
    QTime t_0;
        void getChallenge(int id);
private slots:
    void on_challengeButton(void);
};

#endif // EPICCHALLENGE_H
