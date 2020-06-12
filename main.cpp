#include "epicchallenge.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication m_application(argc, argv);
    EpicChallenge m_challenge;
    m_challenge.show();

    return m_application.exec();
}
