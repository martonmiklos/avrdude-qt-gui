#include <QtGui>
#include <QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
#endif
    
    QCoreApplication::setOrganizationName("MM");
    QCoreApplication::setApplicationName("QAvrdude-xml-converter");
    QCoreApplication::setApplicationVersion("0.1");
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
