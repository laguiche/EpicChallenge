#include "epicchallenge.h"
#include "ui_epicchallenge.h"
#include <QDebug>

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
    //if we are not in sport mode we turn on party mode which doesn't care high, medium or low challenge;
    //furthermore we have stats and other stuff in sport mode
    m_sportMode=true;

    //init database to save time of running loops in sport mode
    unsigned char m_statusDB=NOK;
    if (m_sportMode)
        m_statusDB=setDataBase();

    QString m_filePath_highChallenge=_HIGH_CHALLENGE_PATH_;
    unsigned char statusFile_1=loadChallenges(m_filePath_highChallenge);

    QString m_filePath_mediumChallenge=_MEDIUM_CHALLENGE_PATH_;
    unsigned char statusFile_2=loadChallenges(m_filePath_mediumChallenge);

    QString m_filePath_lowChallenge=_LOW_CHALLENGE_PATH_;
    unsigned char statusFile_3=loadChallenges(m_filePath_lowChallenge);

#ifdef ON_PI
    mfrc.PCD_Init();
#endif

    m_cadenceur=new QTimer();
    t_0=QTime::currentTime();

    /*QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, QColor(121,248,248,255));
    ui->challengeLabel->setAutoFillBackground(true);
    ui->challengeLabel->setPalette(sample_palette);*/


    // [1] The picture is not repeated, can be zoomed freely
    ui->challengeLabel->setStyleSheet("QLabel{"
                             "border-image:url(:/img/pictures/background.jpg) 4 4 4 4 stretch stretch;"
                            "color:blue;"
                             "}");
/*
         // [2] The picture is not repeated, the size is fixed
    ui->label_2->setStyleSheet("QLabel{"
                               "background-image:url(:/images/bd.png);"
                               "background-position:top right;"
                               "background-origin:content;"
                               "background-repeat:none;"
                               "}");*/
/*
         // [3] The picture is not repeated, the zoom ratio can be set, and the dynamic zoom is not allowed
    QImage* img = new QImage;
    img->load(QString(":/img/pictures/background.jpg"));
    QImage scaledimg;
    scaledimg = img->scaled(ui->challengeLabel->width(),ui->challengeLabel->height(),Qt::KeepAspectRatio);
    ui->challengeLabel->setPixmap(QPixmap::fromImage(scaledimg));
*/

    QString error_msg;
    if (statusFile_1*statusFile_2*statusFile_3==NOK)
    {
        error_msg="Problème avec le(s) fichier(s) de challenge (vide(s), absent(s), ...)!";
        ui->challengeLabel->setText(error_msg);
    }
    else if ((m_statusDB==NOK)&&(m_sportMode))
    {
        error_msg=error_msg+"\nImpossible d'enregistrer les résultats (mémoire pleine, erreur d'écriture,...).";
        ui->challengeLabel->setText(error_msg);
    }
    else
    {
        //connect(ui->challengeButton,SIGNAL(clicked(bool)),this,SLOT(on_challengeButton()));
        connect(m_cadenceur,SIGNAL(timeout()),this,SLOT(on_challengeButton()));
        //connect(ui->TagEdit,SIGNAL(textEdited(QString)),this,SLOT(on_challengeButton()));
        connect(ui->TagEdit,SIGNAL(returnPressed()),this,SLOT(on_challengeButton()));
        m_cadenceur->start(10);
    }
}

void EpicChallenge::getChallenge(int id, bool mode)
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

    if(mode && ((timeElapsed/1000)>=10.))
    {
        //enregistrement du chrono
        QSqlQuery query1;
        QString query1_str="INSERT INTO loops(tag,chrono) VALUES(%1,'%2')";

        if(!query1.exec(query1_str.arg(id).arg(t_new.toString("hh_mm_ss"))))
          qDebug() << "ERROR: " << query1.lastError().text();

        int idxChallenge=std::rand() % m_highChallenges.size();
        QString msg;
        msg.setNum(id);
        msg=msg+" : "+m_highChallenges.at(idxChallenge);
        ui->challengeLabel->setText(msg);
    }
    if(!mode && ((timeElapsed/1000)>=2.))
    {
        //enregistrement du chrono
        QSqlQuery query1;
        QString query1_str="INSERT INTO loops(tag,chrono) VALUES(%1,'%2')";

        if(!query1.exec(query1_str.arg(id).arg(t_new.toString("hh_mm_ss"))))
          qDebug() << "ERROR: " << query1.lastError().text();

        int idxChallenge=std::rand() % m_highChallenges.size();
        QString msg=m_highChallenges.at(idxChallenge);
        ui->challengeLabel->setText(msg);
    }
}

void EpicChallenge::on_challengeButton()
{

    unsigned char UID[3];

    //for debug purpose (without raspberry pi for example)
    QObject *obj=sender();
    int debug_tag=0;
    if(obj==ui->TagEdit)
    {
        QString m_string_uid= ui->TagEdit->text();
        debug_tag=m_string_uid.toInt();
        qDebug() << "[DEBUG] nouveau tag" << debug_tag;
        ui->TagEdit->clear();
    }

#ifdef ON_PI
    // Look for a card
    if(!mfrc.PICC_IsNewCardPresent())
    return;

    if( !mfrc.PICC_ReadCardSerial())
    return;

    // Print UID
    for(byte i = 0; i < mfrc.uid.size; ++i)
        UID[i]=mfrc.uid.uidByte[i];
    //qDebug() << QString::number(UID[0],16)<<QString::number(UID[1],16)<<QString::number(UID[2],16);
#endif

    //identification du tag
    unsigned char new_tag_id=0x00;
    if((UID[0]==m_01[0])&&(UID[1]==m_01[1])&&(UID[2]==m_01[2]))       new_tag_id=0x01;
    else if((UID[0]==m_02[0])&&(UID[1]==m_02[1])&&(UID[2]==m_02[2]))  new_tag_id=0x02;

    if(debug_tag!=0)
        getChallenge(debug_tag,m_sportMode);
    else {
        QSqlQuery query1;
        QString query1_str="SELECT tag FROM tagID WHERE (id1=%1 AND id2=%2 AND id3=%3)";

        if(!query1.exec(query1_str.arg(UID[0]).arg(UID[1]).arg(UID[2])))
          qDebug() << "ERROR: " << query1.lastError().text();
        else {
            int query_tag=0;
            while (query1.next()) {
                    query_tag = query1.value(0).toInt();
                    qDebug() << query_tag;
                }
            if(query_tag!=0)
            getChallenge(query_tag,m_sportMode);
        }
    }
    /*else if(new_tag_id!=0x00)
        getChallenge(new_tag_id,m_sportMode);*/
}

unsigned char EpicChallenge::loadChallenges(QString filePath)
{
    unsigned char status=OK;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        status=NOK;
    }
    else
    {
        while (!file.atEnd())
        {
            QByteArray line = file.readLine();
            if (!line.isEmpty())
            {
                m_highChallenges << QString(line);
            }
        }

        file.close();

        if (m_highChallenges.isEmpty())
        {
            status=NOK;
        }
    }

    return status;
}

unsigned char EpicChallenge::setDataBase()
{
    unsigned char status=OK;
    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
        m_db = QSqlDatabase::addDatabase(DRIVER);
    else
        status=NOK;

    //creation de la database
    QString dbName("loops_");
    QString temps2=QTime::currentTime().toString("hh_mm");
    QString temps1 = QDate::currentDate().toString("yyyy_MM_dd_at_");
    dbName.append(temps1);
    dbName.append(temps2);
    dbName.append(".db");
    m_db.setDatabaseName(dbName);

    if(!m_db.open())
    {
        qDebug() << "ERROR: " << m_db.lastError();
        status=NOK;
    }
    else{
        //create a storage table for loop time
        QSqlQuery query("CREATE TABLE loops (id INTEGER PRIMARY KEY, tag INTEGER, chrono TEXT)");

        if(!query.isActive())
        {
            qDebug() << "ERROR: " << query.lastError().text();
            status=NOK;
        }

        //create a storage table for predefined id tag or associated name
        QSqlQuery query2("CREATE TABLE tagID (id INTEGER PRIMARY KEY, id1 INT8, id2 INT8, id3 INT8, tag INTEGER, name TEXT)");

        if(!query2.isActive())
        {
            qDebug() << "ERROR: " << query2.lastError().text();
            status=NOK;
        }

        //filling the tageID table
        //it doesn't care editing the status while the app can run without that
        QFile file("runnerListing.csv");
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
        }
        else
        {
            qDebug() << "Lecture du fichier des noms de coureurs";
            while (!file.atEnd())
            {
                QString line = file.readLine();
                QStringList list1 = line.split(QLatin1Char(';'));
                if(list1.size()>=4)
                {
                    bool ok;
                    QString str_uid1=list1.at(0);
                    int uid1=str_uid1.toInt(&ok,16);
                    QString str_uid2=list1.at(1);
                    int uid2=str_uid2.toInt(&ok,16);
                    QString str_uid3=list1.at(2);
                    int uid3=str_uid3.toInt(&ok,16);
                    QString str_name=list1.at(4);
                    QString str_num=list1.at(3);
                    int num=str_num.toInt();

                    qDebug() << str_num << " : " <<str_name;
                    //enregistrement du chrono
                    QSqlQuery query1;
                    QString query1_str="INSERT INTO tagID(id1,id2,id3,tag,name) VALUES(%1,%2,%3,%4,'%5')";

                    if(!query1.exec(query1_str.arg(uid1).arg(uid2).arg(uid3).arg(num).arg(str_name)))
                      qDebug() << "ERROR: " << query1.lastError().text();
                }
            }
            file.close();
        }
    }

    return status;
}
