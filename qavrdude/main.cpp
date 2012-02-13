#include <QtGui/QApplication>
#include <QtCore/QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    
    QCoreApplication::setOrganizationName("MM");
    QCoreApplication::setApplicationName("QAvrdude");
    QCoreApplication::setApplicationVersion("0.1");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
