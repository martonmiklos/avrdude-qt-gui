#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    statusBarLabel = new QLabel(this);
    statusBarLabel->setText("asd");
    ui->setupUi(this);

    // TODO implement it
    ui->tabWidgetMain->removeTab(5);
    ui->tabWidgetMain->removeTab(8);

    ui->statusbar->addWidget(statusBarLabel);

    // settings to gui
    dataSourceButtonGroup = new QButtonGroup(this);
    dataSourceButtonGroup->addButton(ui->radioButtonUseSqlite);
    dataSourceButtonGroup->addButton(ui->radioButtonUseXML);

    settings = new Settings(this);

    switch (settings->deviceData) {
    case Settings::DeviceDb_SQLite:
        ui->radioButtonUseSqlite->setChecked(true);
        break;
    case Settings::DeviceDb_XML:
        ui->radioButtonUseXML->setChecked(true);
        break;
    }

    programmerSelected();
    ui->checkBoxNoIcons->hide();// FIXME workaround
    ui->lineEditAvrDudePath->setText(settings->dudePath);
    ui->lineEditXmlsPath->setText(settings->xmlsPath);
    ui->lineEditSQLitePath->setText(settings->sqlitePath);
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
    fillDefaultTabComboBox();


    avrPart = new AvrPart(settings, ui->comboBoxDevice->currentText(), this);

    // fuse tables setup
    ui->tableViewFuses->setModel(avrPart->fusesModel());
    connect(avrPart->fusesModel(), SIGNAL(changed()), this, SLOT(deviceChanged()));
    ui->tableViewFuseFields->setModel(avrPart->fuseFieldsModel());

    fuseValueDelegate = new RegisterValueDelegate(this);
    ui->tableViewFuses->setItemDelegateForColumn(1, fuseValueDelegate);
    ui->tableViewFuses->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    fuseFieldDelegate = new BitFieldDelegate(this);
    ui->tableViewFuseFields->setItemDelegateForColumn(1, fuseFieldDelegate);
    ui->tableViewFuseFields->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewFuseFields->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

    // setupt the lockbits gui stuff
    ui->tableViewLockBitFields->setModel(avrPart->lockByteModel());
    connect(avrPart->lockByteModel(), SIGNAL(changed()), this, SLOT(deviceChanged()));
    ui->tableViewLockBitFields->setModel(avrPart->lockByteFieldsModel());

    lockFieldDelegate = new BitFieldDelegate(this);
    ui->tableViewLockBitFields->setItemDelegateForColumn(1, lockFieldDelegate);
    ui->tableViewLockBitFields->horizontalHeader()->setStretchLastSection(true);

    deviceChanged(); // this function opens all delegate on fusefields tableview

    statusBarLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    statusBarLabel->setTextFormat(Qt::RichText);
    statusBarLabel->setText(QString("<b>%1 %2</b>").arg(avrPart->getPartName()).arg(avrPart->getSignature()));

    avrProgrammer = new AvrProgrammer(settings, avrPart, this);
    connect(avrProgrammer, SIGNAL(avrDudeOut(QString)), this, SLOT(avrDudeOut(QString)));
    connect(avrProgrammer, SIGNAL(signatureReadSignal(quint8,quint8,quint8)), this, SLOT(signatureRead(quint8,quint8,quint8)));
    connect(avrProgrammer, SIGNAL(taskFailed(QString)), this, SLOT(logError(QString)));
    connect(avrProgrammer, SIGNAL(taskFinishedOk(QString)), this,SLOT(logMessage(QString)));
    connect(avrProgrammer, SIGNAL(progressStep()), this, SLOT(progressStep())); // this will output nice dots to the messages view during the long operations
    connect(avrProgrammer, SIGNAL(verifyMismatch(QString,int,int,int)), this, SLOT(verifyFailed(QString,int,int,int)));

    ui->checkBoxLastTabRemember->setChecked(settings->rememberLastTab);
    if (settings->rememberLastTab == true) {
        ui->tabWidgetMain->setCurrentIndex(settings->lastTabIndex);
    } else {
        ui->tabWidgetMain->setCurrentIndex(settings->defaultTabIndex);
    }

    QDir xmlDir(settings->xmlsPath);
    if (((!xmlDir.exists()) || (xmlDir.entryList(QStringList("*.xml")).isEmpty())) && settings->deviceData == Settings::DeviceDb_XML) {
        QMessageBox xmlMsg(QMessageBox::Critical,
                           tr("The xml files directory does not exists, or it does not contains XML files"),
                           tr("The program is using AVR description XML files.\n"
                              "These files are provided with the Avrstudio.\n"
                              "If you have Avrstudio installed go to the Settings tab\n"
                              "and browse the PartDescriptionFiles directory from the Avrstudio's install dir.\n"
                              "If you do not have Avrstudio you can obtain it from the "
                              "<a href='http://www.atmel.com/dyn/products/tools_card.asp?tool_id=2725'>Atmel's website</a> for free.\n"
                              "If you can't or don't want to download it do not worry, the basic functions will work without it."
                              "It is possible to get the device data from an SQLite database, which is provided by the qavrdude-data package.\n"
                              "Check your settings tab about this mode!"));
        xmlMsg.setTextFormat(Qt::RichText);
        xmlMsg.exec();
        ui->tabWidgetMain->setCurrentWidget(ui->tabSettings);
        ui->lineEditXmlsPath->setFocus(Qt::TabFocusReason);
    }
}

MainWindow::~MainWindow()
{
    settings->lastTabIndex = ui->tabWidgetMain->currentIndex();
    if (avrProgrammer->isWorking()) {

    }
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

void MainWindow::fillDefaultTabComboBox()
{
    for (int i = 0; i<ui->tabWidgetMain->count(); i++) {
        ui->comboBoxDefaultTab->addItem(ui->tabWidgetMain->tabIcon(i),
                                        ui->tabWidgetMain->tabText(i),
                                        i);
    }
    ui->comboBoxDefaultTab->setCurrentIndex(settings->defaultTabIndex);
}

void MainWindow::programmerSelected()
{
    bool hwVisible = false;
    if (settings->programmerName.contains("stk500")) {
        ui->horizontalSliderVTarget->setMaximum(60);
        ui->horizontalSliderVTarget->setSingleStep(1);
        ui->doubleSpinBoxVTarget->setSingleStep(0.1);
        ui->doubleSpinBoxVTarget->setMaximum(6.0);
        ui->horizontalSliderAREF0->setMaximum(60);
        hwVisible = true;
        ui->horizontalSliderAREF1->setEnabled(false);
        ui->labelAREF1->setEnabled(false);
        ui->doubleSpinBoxAREF1->setEnabled(false);
    } else if (settings->programmerName.contains("stk600")) {
        ui->horizontalSliderVTarget->setMaximum(55);
        ui->doubleSpinBoxVTarget->setMaximum(5.5);
        ui->horizontalSliderAREF0->setMaximum(550);
        ui->horizontalSliderAREF1->setMaximum(550);
        hwVisible = true;
        ui->horizontalSliderAREF1->setEnabled(true);
        ui->labelAREF1->setEnabled(true);
        ui->doubleSpinBoxAREF1->setEnabled(true);
    }
    ui->tabWidgetMain->setTabEnabled(5, hwVisible);
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
    ui->comboBoxDevice->addItem("Attiny22", "2343");
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
    programmerSelected();
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
    deviceChanged();
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
                okBox.setWindowTitle("Part found");
                okBox.setText(QString("The part is seems to be %1\n"
                                      "Change the current device to it?").arg(partIs));
                okBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                if (okBox.exec() == QMessageBox::Yes) {
                    ui->labelSignatureMatch->setText("<b>The signature matches the selected device</b>");
                    ui->comboBoxDevice->setCurrentIndex(ui->comboBoxDevice->findText(partIs));
                    on_comboBoxDevice_activated(ui->comboBoxDevice->currentIndex());
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

void MainWindow::logError(QString msg)
{
    msg.prepend("<p style=\"color:red;\">");
    msg.append("</p>");
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

    QString fn = QFileDialog::getOpenFileName(this,
                                              tr("Select the flash image"),
                                              path,
                                              tr("HEX files (*.hex);;RAW image (*.bin *.raw);;Motorla S Records (*.s)"));
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
    QString fuseNames;
    for (int i = 0; i< avrPart->fuseRegs.count(); i++) {
        fuseNames.append(avrPart->fuseRegs[i]->name());
        if (i != (avrPart->fuseRegs.count()-1))
            fuseNames.append(", ");
        for (int j = 0; j<avrPart->fuseRegs[i]->bitFields.count(); j++) {
            if (avrPart->fuseRegs[i]->bitFields[j].text().contains("clock", Qt::CaseInsensitive) &&
                avrPart->fuseRegs[i]->bitFields[j].text().contains("source", Qt::CaseInsensitive)) {
                if (avrPart->fuseRegs[i]->bitFields[j].enumValues.value(avrPart->fuseRegs[i]->bitFields[j].value())
                    .contains("Ext", Qt::CaseInsensitive)) {
                    QMessageBox msgBox(QMessageBox::Warning, tr("Are you sure"), tr("It seems to be that you have selected external clock\n"
                                                                                    "for clock source.\n"
                                                                                    "You may brick you AVR if you do not\n"
                                                                                    "have the external clock!\n"
                                                                                    "Do you really want to program these fuses?"));
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::No);
                    if (msgBox.exec() != QMessageBox::Yes) {
                        return;
                    }
                    goto program;
                }
            }
        }
    }
    program:
    logMessage(tr("Programming the %1 fuse bits").arg(fuseNames));
    avrProgrammer->programFuses();
}

void MainWindow::on_pushButtonReadFuses_clicked()
{
    logMessage("Reading fuse bits");
    avrProgrammer->readFuses(avrPart->getSupportedFuses());
}

void MainWindow::on_pushButtonAboutQt_clicked()
{
    QApplication::aboutQt();
}

void MainWindow::on_checkBoxLastTabRemember_toggled(bool checked)
{
    settings->rememberLastTab = checked;
    ui->comboBoxDefaultTab->setEnabled(!checked);
}

void MainWindow::on_comboBoxDefaultTab_activated(int index)
{
    settings->defaultTabIndex = index;
}

void MainWindow::on_pushButtonProgramEEPROM_clicked()
{
    if (QFile::exists(ui->lineEditEEPROMHex->text())){
        avrProgrammer->programEEPROM(ui->lineEditEEPROMHex->text());
        logMessage(tr("Programming the EEPROM memory"));
    } else {
        logMessage(tr("Cannot write the EEPROM, because the input hex file not exists"));
    }
}

void MainWindow::on_pushButtonVerifyEEPROM_clicked()
{
    if (QFile::exists(ui->lineEditEEPROMHex->text())){
        logMessage(tr("Verfying the EEPROM memory"));
        avrProgrammer->verifyEEPROM(ui->lineEditEEPROMHex->text());
    } else {
        logMessage(tr("Cannot verify because the etalon hex file does not exists"));
    }
}

void MainWindow::on_pushButtonReadReadEEPROM_clicked()
{
    if (QFile::exists(ui->lineEditEEPROMHex->text())) {
        QMessageBox exists(QMessageBox::Warning	,
                           tr("File already exists!"),
                           tr("The output file is already exists.\nThe reading will overwrite it.\nAre you sure?"),
                            (QMessageBox::Yes | QMessageBox::No));
        if (exists.exec() == QMessageBox::Yes) {
            logMessage(tr("Reading the EEPROM memory"));
            avrProgrammer->readEEPROM(ui->lineEditEEPROMHex->text());
        }
    } else {
        logMessage(tr("Reading the EEPROM memory"));
        avrProgrammer->readEEPROM(ui->lineEditEEPROMHex->text());
    }
}

void MainWindow::on_textEditMessages_anchorClicked(QUrl link)
{
    if (link.toString() == "tab_dudeout") {
        ui->tabWidgetMain->setCurrentWidget(ui->tabAVRDudeOutput);
    }
}

void MainWindow::on_comboBoxFuseDisplaymode_activated(int index)
{
    for (int i = 0; i<ui->tableViewFuses->model()->rowCount(); i++) {
        ui->tableViewFuses->closePersistentEditor(avrPart->fuseFieldsModel()->index(i, 1));
    }

    switch (index) {
    case 0:
        fuseValueDelegate->setInputType(RegisterValueSpinBox::HexaDecimal);
        break;
    case 1:
        fuseValueDelegate->setInputType(RegisterValueSpinBox::Binary);
        break;
    case 2:
        fuseValueDelegate->setInputType(RegisterValueSpinBox::Decimal);
        break;
    }

    for (int i = 0; i<ui->tableViewFuses->model()->rowCount(); i++) {
        ui->tableViewFuses->openPersistentEditor(avrPart->fuseFieldsModel()->index(i, 1));
    }
}

void MainWindow::on_checkBoxNoIcons_toggled(bool checked)
{
    settings->noicons = checked;
    QMessageBox::warning(this, tr("Warning"), tr("This change only takes effect after restarting the program"));
}

void MainWindow::on_pushButtonReadLockbits_clicked()
{
    logMessage(tr("Reading the lockbyte"));
    avrProgrammer->readLockByte();
}

void MainWindow::on_pushButtonProgramLockbits_clicked()
{
    if (QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Writing the lockbits is dangerous!\n"
                                "You may make your device unusable\n"
                                "if you are not careful!\n"
                                "Do you really want to continue?"),
                             QMessageBox::Yes,
                             QMessageBox::No) == QMessageBox::Yes) {
        logMessage(tr("Writing the lockbyte (value: 0x%2)")
                   .arg(QString::number(avrPart->lockBytes.first()->value(), 16).rightJustified(2, '0'))); // FIXME if we find AVR with multiple lockbytesx
        avrProgrammer->programLockByte();
    }
}

void MainWindow::on_horizontalSliderVTarget_sliderMoved(int position)
{
    double voltage = (double)position/10;
    ui->doubleSpinBoxVTarget->setValue(voltage);
}


void MainWindow::deviceChanged()
{
    avrPart->fuseFieldsModel()->refresh();
    avrPart->fusesModel()->refresh();

    avrPart->lockByteFieldsModel()->refresh();
    avrPart->lockByteModel()->refresh();

    //return;
    for (int i = 0; i<ui->tableViewFuseFields->model()->rowCount(); i++)
        ui->tableViewFuseFields->openPersistentEditor(avrPart->fuseFieldsModel()->index(i, 1));

    for (int i = 0; i<ui->tableViewLockBitFields->model()->rowCount(); i++)
        ui->tableViewLockBitFields->openPersistentEditor(avrPart->lockByteFieldsModel()->index(i, 1));

    on_comboBoxFuseDisplaymode_activated(ui->comboBoxFuseDisplaymode->currentIndex());
}

void MainWindow::on_pushButtonVerifyFuses_clicked()
{
    logMessage(tr("Verifying fuses"));
    avrProgrammer->verifyFuses();
}

void MainWindow::on_pushButtonReadVoltages_clicked()
{
    logMessage(tr("Reading programming voltages"));
    avrProgrammer->getVoltagesSTK500();
}

void MainWindow::on_pushButtonWriteVoltages_clicked()
{
    logMessage(tr("Setting Vtarget to: %1, AREF0 to: %2 AREF1 to: %3")
               .arg(ui->doubleSpinBoxVTarget->value())
               .arg(ui->doubleSpinBoxAREF0->value())
               .arg(ui->doubleSpinBoxAREF1->value()));
    avrProgrammer->setVoltagesSTK500(ui->doubleSpinBoxVTarget->value(),
                                     ui->doubleSpinBoxAREF0->value(),
                                     ui->doubleSpinBoxAREF1->value());
}


void MainWindow::on_radioButtonUseXML_clicked()
{
    settings->deviceData = Settings::DeviceDb_XML;
}

void MainWindow::on_radioButtonUseSqlite_clicked()
{
    settings->deviceData = Settings::DeviceDb_SQLite;
}

void MainWindow::on_pushButtonBrowseSqlite_clicked()
{
    QFileInfo fi(settings->sqlitePath);
    QString sqliteDir = fi.path();
    if (sqliteDir == "")
        sqliteDir = QDir::homePath();
    QString str = QFileDialog::getOpenFileName(this, tr("Select the SQLite database"), sqliteDir, tr("SQLite databases(*.sqlite)"));
    if (!str.isEmpty()) {
        settings->sqlitePath = str;
        ui->lineEditSQLitePath->setText(str);
    }
}

void MainWindow::on_lineEditProgOptions_editingFinished()
{

}
