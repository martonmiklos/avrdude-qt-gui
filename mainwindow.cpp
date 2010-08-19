#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QtMultimedia>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    statusBarLabel = new QLabel(this);
    ui->setupUi(this);
    ui->statusbar->addWidget(statusBarLabel);

    // settings to gui
    settings = new Settings(this);
    ui->lineEditAvrDudePath->setText(settings->dudePath);
    ui->lineEditXmlsPath->setText(settings->xmlsPath);
    ui->checkBoxOverrideProgrammersOptions->setChecked(settings->particularProgOptions);
    on_checkBoxOverrideProgrammersOptions_toggled(settings->particularProgOptions);
    ui->lineEditProgOptions->setText(settings->programmerOptions);
    ui->checkBoxShowAvrDudeOutput->setChecked(settings->showAvrDudeOutPut);
    ui->lineEditProgrammerPort->setText(settings->programmerPort);
    ui->lineEditFlashHex->setText(settings->flashHexFile);
    ui->lineEditEEPROMHex->setText(settings->eepromHexFile);

    connect(ui->lineEditFlashHex, SIGNAL(clicked()), this, SLOT(showFlashHexFileBrowse()));
    connect(ui->lineEditEEPROMHex, SIGNAL(clicked()),this, SLOT(showEEPROMHexFileBrowse()));

    textOutPutMenu = ui->textEditMessages->createStandardContextMenu();
    QAction *clearAction = textOutPutMenu->addAction(tr("Clear all"));
    connect(clearAction, SIGNAL(triggered()), this, SLOT(clearMessages()));


    fillDeviceList();
    fillProgrammersList();
    calculateArgumentsLabelText();

    fuseModel = new FuseModel(this);

    avrPart = new AvrPart(settings, fuseModel, this);
    avrPart->init(ui->comboBoxDevice->currentText());

    // the avrpart object fills the fuseModel
    ui->tableViewFuses->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableViewFuses->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    ui->tableViewFuses->setEditTriggers(QTableView::QAbstractItemView::CurrentChanged);
    ui->tableViewFuses->setModel(fuseModel);
    fuseDelegate = new FuseDelegate(this);
    ui->tableViewFuses->setItemDelegateForColumn(1, fuseDelegate);
    connect(avrPart, SIGNAL(reloadFuseView()), this, SLOT(reloadFuseView()));
    reloadFuseView();

    statusBarLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    statusBarLabel->setTextFormat(Qt::RichText);
    statusBarLabel->setText(QString("<b>%1 %2</b>").arg(avrPart->getPartName()).arg(avrPart->getSignature()));

    avrProgrammer = new AvrProgrammer(settings, this);
    connect(avrProgrammer, SIGNAL(avrDudeOut(QString)), this, SLOT(avrDudeOut(QString)));
    connect(avrProgrammer, SIGNAL(signatureReadSignal(quint8,quint8,quint8)), this, SLOT(signatureRead(quint8,quint8,quint8)));
    connect(avrProgrammer, SIGNAL(taskFailed(QString)), this, SLOT(logMessage(QString)));
    connect(avrProgrammer, SIGNAL(taskFinishedOk(QString)), this,SLOT(logMessage(QString)));
    connect(avrProgrammer, SIGNAL(progressStep()), this, SLOT(progressStep())); // this will output nice dots to the messages view during the long operations
    connect(avrProgrammer, SIGNAL(verifyMismatch(QString,int,int,int)), this, SLOT(verifyFailed(QString,int,int,int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::fillProgrammersList()
{
    ui->comboBoxProgrammer->addItem("STK200","stk200");
    ui->comboBoxProgrammer->addItem("Atmel STK500","stk500");
    ui->comboBoxProgrammer->addItem("C2N232I","c2n232i");
    ui->comboBoxProgrammer->addItem("Direct AVR Parallel Access cable","dapa");
    ui->comboBoxProgrammer->addItem("serial port banging   reset=rts sck=dtr mosi=txd miso=cts","dasa");
    ui->comboBoxProgrammer->addItem("serial port banging   reset=!dtr sck=rts mosi=txd miso=cts","dasa3");
    ui->comboBoxProgrammer->addItem("AVR Dragon in debugWire mode","dragon_dw");
    ui->comboBoxProgrammer->addItem("AVR Dragon in HVSP mode","dragon_hvsp");
    ui->comboBoxProgrammer->addItem("AVR Dragon in ISP mode","dragon_isp");
    ui->comboBoxProgrammer->addItem("AVR Dragon in JTAG mode","dragon_jtag");
    ui->comboBoxProgrammer->addItem("AVR Dragon in PDI mode","dragon_pdi");
    ui->comboBoxProgrammer->addItem("AVR Dragon in HVP mode","dragon_pp");
    ui->comboBoxProgrammer->addItem("Dontronics DT006","dt006");
    ui->comboBoxProgrammer->addItem("ERE ISP-AVR","ere-isp-avr");
    ui->comboBoxProgrammer->addItem("Frank-STK200","frank-stk200");
    ui->comboBoxProgrammer->addItem("Futurlec.com programming cable","futurlec");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkI   running at 115200 Bd","jtag1");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkI   running at 19200 Bd","jtag1slow");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII   running at 115200 Bd","jtag2");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII in AVR32 mode.","jtag2avr32");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII in debugWire mode.","jtag2dw");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII   running at 115200 Bd","jtag2fast");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII in ISP mode.","jtag2isp");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII in PDI mode.","jtag2pdi");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII (19200 Bd)","jtag2slow");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkI   running at 115200 Bd","jtagmkI");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII (19200 Bd)","jtagmkII");
    ui->comboBoxProgrammer->addItem("Atmel JTAG ICE mkII in AVR32 mode.","jtagmkII_avr32");
    ui->comboBoxProgrammer->addItem("Crossbow MIB510 programming board","mib510");
    ui->comboBoxProgrammer->addItem("Jason Kyle’s pAVR Serial Programmer","pavr");
    ui->comboBoxProgrammer->addItem("Picoweb Programming Cable","picoweb");
    ui->comboBoxProgrammer->addItem("Pony Prog STK200","pony-stk200");
    ui->comboBoxProgrammer->addItem("Pony Prog serial","ponyser");
    ui->comboBoxProgrammer->addItem("Lancos SI-Prog","siprog");
    ui->comboBoxProgrammer->addItem("Steve Bolt’s Programmer","sp12");
    ui->comboBoxProgrammer->addItem("Atmel STK500 in HVSP mode(2.x firmware)","stk500hvsp");
    ui->comboBoxProgrammer->addItem("Atmel STK500 in parallel programming mode (2.x firmware)","stk500pp");
    ui->comboBoxProgrammer->addItem("Atmel STK500   1.x firmware","stk500v1");
    ui->comboBoxProgrammer->addItem("Atmel STK500   2.x firmware","stk500v2");
    ui->comboBoxProgrammer->addItem("Atmel STK600 in ISP mode","stk600");
    ui->comboBoxProgrammer->addItem("Atmel STK600 in HVSP mode","stk600hvsp");
    ui->comboBoxProgrammer->addItem("Atmel STK600 in Ppmode","stk600pp");
    ui->comboBoxProgrammer->addItem("USBasp","usbasp");
    ui->comboBoxProgrammer->addItem("USBtiny","usbtiny");
    ui->comboBoxProgrammer->addItem("Xilinx JTAG cable","xil");
    ui->comboBoxProgrammer->setCurrentIndex(ui->comboBoxProgrammer->findData(settings->programmerName));
}

void MainWindow::calculateArgumentsLabelText()
{
    QString args = QString("-c %1 -P %2").arg(settings->programmerName).arg(settings->programmerPort);
    ui->labelCurrentArgumentsValue->setText(args);
}

void MainWindow::reloadFuseView()
{
    for (int i = 0; i<fuseModel->rowCount(fuseModel->index(-1,-1)) ; i++) {
        ui->tableViewFuses->openPersistentEditor(fuseModel->index(i, 1));
    }
}

void MainWindow::progressStep()
{
    ui->textEditMessages->insertPlainText(".");
}

void MainWindow::verifyFailed(QString what, int offset, int value_read, int value_waited)
{
    QMessageBox msgBox(QMessageBox::Critical,
                       tr("Verify failed"),
                       QString(tr("Verifying the %1 failed!\nByte mismatch at %2\n%3 != %4"))
                       .arg(what).arg(offset).arg(value_read).arg(value_waited));
    msgBox.exec();
}

void MainWindow::fillDeviceList()
{
    ui->comboBoxDevice->addItem("AT90S1200", "1200");
    ui->comboBoxDevice->addItem("AT90S2313", "2313");
    ui->comboBoxDevice->addItem("AT90S2333", "2333");
    ui->comboBoxDevice->addItem("AT90S2343", "2343");
    ui->comboBoxDevice->addItem("ATtiny22", "2343");
    ui->comboBoxDevice->addItem("AT90S4414", "4414");
    ui->comboBoxDevice->addItem("AT90S4433", "4433");
    ui->comboBoxDevice->addItem("AT90S4434", "4434");
    ui->comboBoxDevice->addItem("AT90S8515", "8515");
    ui->comboBoxDevice->addItem("AT90S8535", "8535");
    ui->comboBoxDevice->addItem("AT90CAN128", "c128");
    ui->comboBoxDevice->addItem("AT90CAN32", "c32");
    ui->comboBoxDevice->addItem("AT90CAN64", "c64");
    ui->comboBoxDevice->addItem("ATmega103", "m103");
    ui->comboBoxDevice->addItem("ATmega128", "m128");
    ui->comboBoxDevice->addItem("ATmega1280", "m1280");
    ui->comboBoxDevice->addItem("ATmega1281", "m1281");
    ui->comboBoxDevice->addItem("ATmega1284P", "m1284p");
    ui->comboBoxDevice->addItem("ATmega128RFA1", "m128rfa1");
    ui->comboBoxDevice->addItem("ATmega16", "m16");
    ui->comboBoxDevice->addItem("ATmega161", "m161");
    ui->comboBoxDevice->addItem("ATmega162", "m162");
    ui->comboBoxDevice->addItem("ATmega163", "m163");
    ui->comboBoxDevice->addItem("ATmega164", "m164");
    ui->comboBoxDevice->addItem("ATmega164P", "m164p");
    ui->comboBoxDevice->addItem("ATmega168", "m168");
    ui->comboBoxDevice->addItem("ATmega169", "m169");
    ui->comboBoxDevice->addItem("ATmega2560", "m2560");
    ui->comboBoxDevice->addItem("ATmega2561", "m2561");
    ui->comboBoxDevice->addItem("ATmega32", "m32");
    ui->comboBoxDevice->addItem("ATmega324P", "m324p");
    ui->comboBoxDevice->addItem("ATmega325", "m325");
    ui->comboBoxDevice->addItem("ATmega3250", "m3250");
    ui->comboBoxDevice->addItem("ATmega328P", "m328p");
    ui->comboBoxDevice->addItem("ATmega329", "m329");
    ui->comboBoxDevice->addItem("ATmega3290", "m3290");
    ui->comboBoxDevice->addItem("ATmega329P", "m329p");
    ui->comboBoxDevice->addItem("ATmega3290P", "m3290p");
    ui->comboBoxDevice->addItem("ATmega32U4", "m32u4");
    ui->comboBoxDevice->addItem("ATmega48", "m48");
    ui->comboBoxDevice->addItem("ATmega64", "m64");
    ui->comboBoxDevice->addItem("ATmega640", "m640");
    ui->comboBoxDevice->addItem("ATmega644P", "m644p");
    ui->comboBoxDevice->addItem("ATmega644", "m644");
    ui->comboBoxDevice->addItem("ATmega645", "m645");
    ui->comboBoxDevice->addItem("ATmega6450", "m6450");
    ui->comboBoxDevice->addItem("ATmega649", "m649");
    ui->comboBoxDevice->addItem("ATmega6490", "m6490");
    ui->comboBoxDevice->addItem("ATmega8", "m8");
    ui->comboBoxDevice->addItem("ATmega8515", "m8515");
    ui->comboBoxDevice->addItem("ATmega8535", "m8535");
    ui->comboBoxDevice->addItem("ATmega88", "m88");
    ui->comboBoxDevice->addItem("AT90PWM2", "pwm2");
    ui->comboBoxDevice->addItem("AT90PWM2B", "pwm2b");
    ui->comboBoxDevice->addItem("AT90PWM3", "pwm3");
    ui->comboBoxDevice->addItem("AT90PWM3B", "pwm3b");
    ui->comboBoxDevice->addItem("ATtiny10", "t10");
    ui->comboBoxDevice->addItem("ATtiny11", "t12");
    ui->comboBoxDevice->addItem("ATtiny12", "t12");
    ui->comboBoxDevice->addItem("ATtiny13", "t13");
    ui->comboBoxDevice->addItem("ATtiny15", "t15");
    ui->comboBoxDevice->addItem("ATtiny2313", "t2313");
    ui->comboBoxDevice->addItem("ATtiny25", "t25");
    ui->comboBoxDevice->addItem("ATtiny26", "t26");
    ui->comboBoxDevice->addItem("ATtiny261", "t261");
    ui->comboBoxDevice->addItem("ATtiny4", "t4");
    ui->comboBoxDevice->addItem("ATtiny44", "t44");
    ui->comboBoxDevice->addItem("ATtiny45", "t45");
    ui->comboBoxDevice->addItem("ATtiny461", "t461");
    ui->comboBoxDevice->addItem("ATtiny5", "t5");
    ui->comboBoxDevice->addItem("ATtiny84", "t84");
    ui->comboBoxDevice->addItem("ATtiny85", "t85");
    ui->comboBoxDevice->addItem("ATtiny861", "t861");
    ui->comboBoxDevice->addItem("ATtiny88", "t88");
    ui->comboBoxDevice->addItem("ATtiny9", "t9");
    ui->comboBoxDevice->addItem("AT32UCA0512", "ucr2");
    ui->comboBoxDevice->addItem("ATmega1286", "usb1286");
    ui->comboBoxDevice->addItem("ATmega1287", "usb1287");
    ui->comboBoxDevice->addItem("ATmega162", "usb162");
    ui->comboBoxDevice->addItem("ATmega647", "usb646");
    ui->comboBoxDevice->addItem("ATmega647", "usb647");
    ui->comboBoxDevice->addItem("ATmega82", "usb82");
    ui->comboBoxDevice->addItem("ATxmega128A1", "x128a1");
    ui->comboBoxDevice->addItem("ATxmega128A1revD", "x128a1d");
    ui->comboBoxDevice->addItem("ATxmega128A3", "x128a3");
    ui->comboBoxDevice->addItem("ATxmega128A4", "x128a4");
    ui->comboBoxDevice->addItem("ATxmega16A4", "x16a4");
    ui->comboBoxDevice->addItem("ATxmega192A1", "x192a1");
    ui->comboBoxDevice->addItem("ATxmega192A3", "x192a3");
    ui->comboBoxDevice->addItem("ATxmega256A1", "x256a1");
    ui->comboBoxDevice->addItem("ATxmega256A3", "x256a3");
    ui->comboBoxDevice->addItem("ATxmega256A3B", "x256a3b");
    ui->comboBoxDevice->addItem("ATxmega32A4", "x32a4");
    ui->comboBoxDevice->addItem("ATxmega64A1", "x64a1");
    ui->comboBoxDevice->addItem("ATxmega64A3", "x64a3");
    ui->comboBoxDevice->addItem("ATxmega64A4", "x64a4");
    ui->comboBoxDevice->setCurrentIndex(ui->comboBoxDevice->findData(settings->partName));
}

void MainWindow::on_pushButtonClearAvrDudeOutput_clicked()
{
    ui->textEditAvrDudeOutPut->clear();
}

void MainWindow::on_checkBoxShowAvrDudeOutput_toggled(bool checked)
{
    settings->showAvrDudeOutPut = checked;
}

void MainWindow::on_pushButtonBrowseXmls_clicked()
{
    QString str = QFileDialog::getExistingDirectory(this, tr("Select the partdescfiles directory"), settings->xmlsPath, QFileDialog::ShowDirsOnly);
    if (!str.isEmpty()) {
        settings->xmlsPath = str;
        ui->lineEditXmlsPath->setText(str);
    }
}

void MainWindow::on_lineEditXmlsPath_textEdited(QString str)
{
    settings->xmlsPath = str;
}

void MainWindow::on_lineEditAvrDudePath_textEdited(QString str)
{
    settings->dudePath = str;
}

void MainWindow::on_pushButtonAvrDude_clicked()
{
    QString str = QFileDialog::getOpenFileName(this,tr("Select the AVRDude binary"), settings->dudePath);
    if (!str.isEmpty()) {
        ui->lineEditAvrDudePath->setText(str);
        settings->dudePath = str;
    }
}

void MainWindow::on_pushButtonReadSignature_clicked()
{
    avrProgrammer->readSignature();
    ui->labelSignatureMatch->setText("");
    logMessage("Reading signature");
}

void MainWindow::avrDudeOut(QString str)
{
    if (settings->showAvrDudeOutPut) {
        if (str.size() > 1) {
            ui->textEditAvrDudeOutPut->append(str);
        } else {
            ui->textEditAvrDudeOutPut->insertPlainText(str);
        }
    }
}

void MainWindow::on_checkBoxOverrideProgrammersOptions_toggled(bool checked)
{
    ui->comboBoxProgrammer->setEnabled(!checked);
    ui->lineEditProgrammerPort->setEnabled(!checked);
    ui->lineEditProgOptions->setEnabled(checked);
    settings->particularProgOptions = checked;
}

void MainWindow::on_lineEditProgOptions_textEdited(QString str)
{
    settings->programmerOptions = str;
}

void MainWindow::on_lineEditProgrammerPort_textEdited(QString str)
{
    settings->programmerPort = str;
    calculateArgumentsLabelText();
}

void MainWindow::on_comboBoxProgrammer_activated(int index)
{
    settings->programmerName = ui->comboBoxProgrammer->itemData(index).toString();
    calculateArgumentsLabelText();
}

void MainWindow::on_comboBoxDevice_activated(int index)
{
    settings->partName = ui->comboBoxDevice->itemData(index).toString();
    if (!avrPart->setPartName(ui->comboBoxDevice->currentText())) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error happend");
        msgBox.setText(QString("Error while loading part data:\n%1").arg(avrPart->error()));
        msgBox.exec();
    }
    statusBarLabel->setText(QString("<b>%1 %2</b>").arg(avrPart->getPartName()).arg(avrPart->getSignature()));
}


void MainWindow::signatureRead(quint8 s0, quint8 s1, quint8 s2)
{
    QString signature;
    signature = "0x"+QString::number(s0, 16).rightJustified(2, '0').toUpper()+":"+
                "0x"+QString::number(s1, 16).rightJustified(2, '0').toUpper()+":"+
                "0x"+QString::number(s2, 16).rightJustified(2, '0').toUpper();
    ui->lineEditDevSignature->setText(signature);

    bool ok = true;
    if ((avrPart->sign0 != s0) ||
        (avrPart->sign1 != s1) ||
        (avrPart->sign2 != s2))
        ok = false;

    if (!ok) {
        QMessageBox error(this);
        error.setWindowTitle("Signature mismatch");
        error.setText("The signature does not matches the selected device\n"
                      "Would you like to find it which part is it?");
        error.setStandardButtons(QMessageBox::Yes| QMessageBox::No);
        if (error.exec() == QMessageBox::Yes) {
            QString partIs = avrPart->findDeviceWithSignature(s0, s1, s2);
            if (!partIs.isEmpty()) {
                QMessageBox okBox;
                okBox.setWindowTitle("Found part");
                okBox.setText(QString("The part is seems to be %1\n"
                                      "Change the current device to it?").arg(partIs));
                okBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                if (okBox.exec() == QMessageBox::Yes) {
                    ui->labelSignatureMatch->setText("<b>The signature matches the selected device</b>");
                    ui->comboBoxDevice->setCurrentIndex(ui->comboBoxDevice->findText(partIs));
                    avrPart->setPartName(partIs);
                    settings->partName = ui->comboBoxDevice->itemData(ui->comboBoxDevice->currentIndex()).toString();
                    statusBarLabel->setText(QString("<b>%1 %2</b>").arg(avrPart->getPartName()).arg(avrPart->getSignature()));
                } else {
                    ui->labelSignatureMatch->setText("<b>The signature does not matches the selected device</b>");
                }
            } else {
                QMessageBox notFound;
                notFound.setWindowTitle("Part not found");
                notFound.setText("Could not found a part with this signature in the database");
                notFound.exec();
            }
        } else {
            ui->labelSignatureMatch->setText("<b>The signature does not matches the selected device</b>");
        }
    } else {
       ui->labelSignatureMatch->setText("<b>The signature matches the selected device</b>");
    }
}

void MainWindow::on_pushButtonErase_clicked()
{
    QMessageBox question(QMessageBox::Question,
                        "Are you sure?",
                        "Do you really want to erase\n"
                        "the device flash and EEPROM memory?",
                        (QMessageBox::Yes | QMessageBox::No));
    if (question.exec() == QMessageBox::Yes) {
        avrProgrammer->eraseDevice();
        logMessage("Erasing device");
    }
}

void MainWindow::logMessage(QString msg)
{
    ui->textEditMessages->append(msg);
}

void MainWindow::showFlashHexFileBrowse()
{
    QString path;
    if (settings->flashHexFile.isEmpty()) {
        path = QDir::homePath();
    } else {
        QFileInfo info(settings->flashHexFile);
        path = info.absolutePath();
    }

    QString fn = QFileDialog::QFileDialog::getSaveFileName(this, tr("Select the hex file location"), path, tr("HEX files (*.hex)"));
    if (fn != "") {
        ui->lineEditFlashHex->setText(fn);
        settings->flashHexFile = fn;
    }
}

void MainWindow::on_lineEditFlashHex_textEdited(QString str)
{
    settings->flashHexFile = str;
}

void MainWindow::showEEPROMHexFileBrowse()
{
    QString path;
    if (settings->eepromHexFile.isEmpty()) {
        path = QDir::homePath();
    } else {
        QFileInfo info(settings->eepromHexFile);
        path = info.absolutePath();
    }

    QString fn = QFileDialog::getOpenFileName(this, tr("Open hex file"), path, tr("HEX files (*.hex)"));
    ui->lineEditEEPROMHex->setText(fn);
    settings->eepromHexFile = fn;
}

void MainWindow::clearMessages()
{
    ui->textEditMessages->clear();
}

void MainWindow::on_lineEditEEPROMHex_textEdited(QString str)
{
    settings->eepromHexFile = str;
}

void MainWindow::on_pushButtonProgramFlash_clicked()
{
    if (QFile::exists(ui->lineEditFlashHex->text())){
        avrProgrammer->programFlash(ui->lineEditFlashHex->text(),
                                    ui->checkBoxVerifyAfterProg->isChecked(),
                                    ui->checkBoxEraseBeforeProg->isChecked());
        logMessage(tr("Programming the flash memory"));
    } else {
        logMessage(tr("Cannot program, because the input hex file not exists"));
    }
}

void MainWindow::on_pushButtonVerifyFlash_clicked()
{
    if (QFile::exists(ui->lineEditFlashHex->text())){
        logMessage(tr("Verfying the flash memory"));
        avrProgrammer->verifyFlash(ui->lineEditFlashHex->text());
    } else {
        logMessage(tr("Cannot verify because the etalon hex file does not exists"));
    }

}

void MainWindow::on_pushButtonReadReadFlash_clicked()
{
    if (QFile::exists(ui->lineEditFlashHex->text())) {
        QMessageBox exists(QMessageBox::Warning	,
                           tr("File already exists!"),
                           tr("The output file is already exists.\nThe reading will overwrite it.\nAre you sure?"),
                            (QMessageBox::Yes | QMessageBox::No));
        if (exists.exec() == QMessageBox::Yes) {
            logMessage(tr("Reading the flash memory"));
            avrProgrammer->readFlash(ui->lineEditFlashHex->text());
        }
    } else {
        logMessage(tr("Reading the flash memory"));
        avrProgrammer->readFlash(ui->lineEditFlashHex->text());
    }
}

void MainWindow::on_pushButtonProgramFuses_clicked()
{

}

void MainWindow::on_pushButtonReadFuses_clicked()
{
    logMessage("Reading fuse bits");
    avrProgrammer->readFuse();
}

void MainWindow::on_pushButton_2_clicked()
{
    QApplication::aboutQt();
}
