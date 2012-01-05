#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "xmlconverter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButtonGo_clicked();

    void on_pushButtonBrowseSQLITE_clicked();

    void on_pushButtonBrowseXML_clicked();

    void logMessage(QString str);

private:
    Ui::MainWindow *ui;
    QSettings settings;
    XMLConverter converter;
};

#endif // MAINWINDOW_H
