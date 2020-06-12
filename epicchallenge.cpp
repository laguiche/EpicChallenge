#include "epicchallenge.h"
#include "ui_epicchallenge.h"

EpicChallenge::EpicChallenge(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EpicChallenge)
{
    ui->setupUi(this);
    init();
}

EpicChallenge::~EpicChallenge()
{
    delete ui;
}

void EpicChallenge::init()
{
    m_sportMode=true;

    setDataBase();

    loadChallenges();

    t_0=QTime::currentTime();

    connect(ui->challengeButton,SIGNAL(clicked(bool)),this,SLOT(on_challengeButton()));
}

void EpicChallenge::getChallenge(int id)
{
    QTime t_new=QTime::currentTime();
    QString tag;
    tag.setNum(id);

    //comparaison avec le dernier tour enregistré
    QTime t_last=t_0;
    int timeElapsed=0;
    QSqlQuery query;
    query.prepare("SELECT chrono,tag FROM loops WHERE tag=?");
    query.addBindValue(id);

    if(!query.exec())
      qDebug() << "ERROR: " << query.lastError().text();

    while(query.next())
    {
        QTime t_query=QTime::fromString(query.value(0).toString(),"hh_mm_ss");
        qDebug() << "t_query: "<<t_query;
        if(t_query>t_last)
            t_last=t_query;
    }

    timeElapsed=t_last.msecsTo(t_new);
    qDebug() << t_last << t_new << timeElapsed;

    if((timeElapsed/1000)>=10.)
    {
        //enregistrement du chrono
        QSqlQuery query1;
        QString query1_str="INSERT INTO loops(tag,chrono) VALUES(%1,'%2')";

        if(!query1.exec(query1_str.arg(id).arg(t_new.toString("hh_mm_ss"))))
          qDebug() << "ERROR: " << query1.lastError().text();

        int idxChallenge=std::rand() % m_challenges.size();
        QString msg;
        msg.setNum(id);
        msg=msg+" : "+m_challenges.at(idxChallenge);
        ui->challengeLabel->setText(msg);
    }
}

void EpicChallenge::on_challengeButton()
{
    getChallenge(21);
}

void EpicChallenge::loadChallenges()
{
    QString msg("Problème avec le fichier de challenge (vide, absent, ...)!");

    QFile file("./challenges/highChallenge.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_challenges << msg;
        ui->challengeLabel->setText(msg);
        return;
    }

    while (!file.atEnd())
    {
        QByteArray line = file.readLine();
        if (!line.isEmpty())
            m_challenges << QString(line);
    }

    if (m_challenges.isEmpty())
    {
        m_challenges << msg;
        ui->challengeLabel->setText(msg);
    }
}

void EpicChallenge::setDataBase()
{
    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
        m_db = QSqlDatabase::addDatabase(DRIVER);

    //creation de la database
    QString dbName("loops_");
    QString temps2=QTime::currentTime().toString("hh_mm");
    QString temps1 = QDate::currentDate().toString("yyyy_MM_dd_at_");
    dbName.append(temps1);
    dbName.append(temps2);
    dbName.append(".db");
    m_db.setDatabaseName(dbName);

    if(!m_db.open())
        qDebug() << "ERROR: " << m_db.lastError();
    QSqlQuery query("CREATE TABLE loops (id INTEGER PRIMARY KEY, tag INTEGER, chrono TEXT)");
    if(!query.isActive())
        qDebug() << "ERROR: " << query.lastError().text();
}
