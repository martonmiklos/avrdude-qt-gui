#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEditSQLITE->setText(settings.value("sqlitepath").toString());
    ui->lineEditXMLDir->setText(settings.value("xmlspath").toString());
    connect(&converter,SIGNAL(logMessage(QString)), this, SLOT(logMessage(QString)));
}

MainWindow::~MainWindow()
{
    settings.setValue("sqlitepath", ui->lineEditSQLITE->text());
    settings.setValue("xmlspath", ui->lineEditXMLDir->text());
    delete ui;
}

void MainWindow::on_pushButtonGo_clicked()
{
    converter.convert(ui->lineEditSQLITE->text(), ui->lineEditXMLDir->text());
    qWarning()  << "done";
}

void MainWindow::on_pushButtonBrowseSQLITE_clicked()
{
    QString ret = QFileDialog::getOpenFileName(this,
                                               tr("Select the SQLITE database"),
                                               settings.value("sqlitepath").toString(),
                                               "*.*");
    if (ret != "") {
        ui->lineEditSQLITE->setText(ret);
        settings.setValue("sqlitepath", ui->lineEditSQLITE->text());
    }
}

void MainWindow::on_pushButtonBrowseXML_clicked()
{
    QString ret = QFileDialog::getExistingDirectory(this,
                                                    tr("Select the directory containing avrstudio xmls"),
                                                    settings.value("xmlspath").toString());
    if (ret != "") {
        QDir dir(ret);
        if (dir.entryList(QStringList("*.xml")).isEmpty()) {
            QMessageBox::critical(this,
                                  tr("Could not find any XMLs"),
                                  tr("I cannot find any XMLs in the specified directory\n"));
        } else {
            ui->lineEditXMLDir->setText(ret);
            settings.setValue("xmlspath", ui->lineEditXMLDir->text());
        }
    }
}

void MainWindow::logMessage(QString str)
{
    ui->textEdit->append(str);
}
