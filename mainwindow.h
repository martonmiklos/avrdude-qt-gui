#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QCompleter>
#include <QLabel>
#include <QMenu>
#include <QFile>
#include <QUrl>

#include "avrprogrammer.h"
#include "settings.h"
#include "avrpart.h"
#include "fusemodel.h"
#include "fusedelegate.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    AvrPart *avrPart;
    AvrProgrammer *avrProgrammer;
    QStringList deviceNames;
    QCompleter *nameCompleter;
    Settings *settings;

    QLabel *statusBarLabel;
    QMenu *textOutPutMenu;

    FuseModelCute *fuseModel;
    FuseDelegate *fuseDelegate;

    LockBitsModel *locksModel;
    LockDelegate *lockDelegate;

    FuseValuesModel *fuseValuesModel;

    void fillDeviceList();
    void fillProgrammersList();
    void calculateArgumentsLabelText();
    void fillDefaultTabComboBox();
    void programmerSelected();

private slots:
    void on_horizontalSliderVTarget_sliderMoved(int position);
    void on_pushButtonProgramLockbits_clicked();
    void on_pushButtonReadLockbits_clicked();
    void on_checkBoxNoIcons_toggled(bool checked);
    void on_comboBoxFuseDisplaymode_activated(int index);
    void on_textEditMessages_anchorClicked(QUrl );
    void on_pushButtonReadReadEEPROM_clicked();
    void on_pushButtonVerifyEEPROM_clicked();
    void on_pushButtonProgramEEPROM_clicked();
    void on_comboBoxDefaultTab_activated(int index);
    void on_checkBoxLastTabRemember_toggled(bool checked);
    void on_pushButtonAboutQt_clicked();
    void on_pushButtonReadFuses_clicked();
    void on_pushButtonProgramFuses_clicked();
    void on_pushButtonReadReadFlash_clicked();
    void on_pushButtonVerifyFlash_clicked();
    void on_pushButtonProgramFlash_clicked();
    void on_lineEditEEPROMHex_textEdited(QString );
    void on_lineEditFlashHex_textEdited(QString );
    void on_pushButtonErase_clicked();
    void on_comboBoxDevice_activated(int index);
    void on_comboBoxProgrammer_activated(int index);
    void on_lineEditProgrammerPort_textEdited(QString );
    void on_lineEditProgOptions_textEdited(QString );
    void on_checkBoxOverrideProgrammersOptions_toggled(bool checked);
    void on_pushButtonReadSignature_clicked();
    void on_pushButtonAvrDude_clicked();
    void on_lineEditAvrDudePath_textEdited(QString );
    void on_lineEditXmlsPath_textEdited(QString );
    void on_pushButtonBrowseXmls_clicked();
    void on_checkBoxShowAvrDudeOutput_toggled(bool checked);
    void on_pushButtonClearAvrDudeOutput_clicked();
    void showFlashHexFileBrowse();
    void showEEPROMHexFileBrowse();
    void clearMessages();
    void avrDudeOut(QString str);
    void signatureRead(quint8 s0, quint8 s1, quint8 s2);
    void logMessage(QString msg);
    void logError(QString msg);
    void reloadFuseView();
    void reloadLocksView();
    void progressStep();
    void verifyFailed(QString what, int offset, int value_read, int value_waited);
};

#endif // MAINWINDOW_H
