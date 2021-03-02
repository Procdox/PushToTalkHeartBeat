#include "PushToTalkHeartBeat.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("BlackledgeBuilds");
    QCoreApplication::setApplicationName("PushToTalkHeartBeat");
    QApplication a(argc, argv);
    PushToTalkHeartBeat w;
    w.show();
    return a.exec();
}
