#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QCoreApplication::setApplicationName("QAvrdude-xml-converter");
    QCoreApplication::setApplicationVersion("0.1");
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
