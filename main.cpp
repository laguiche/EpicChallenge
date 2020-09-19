#include "epicchallenge.h"
#include <QApplication>

#ifdef ON_PI
    //#include <wiringPi.h>
    //#include <wiringPiI2C.h>
#endif

int main(int argc, char *argv[])
{
    QApplication m_application(argc, argv);
    EpicChallenge m_challenge;
    m_challenge.showMaximized();

    return m_application.exec();
}
