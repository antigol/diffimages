#ifndef OPENIMAGESDIALOG_H
#define OPENIMAGESDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QSettings>

namespace Ui {
class OpenImagesDialog;
}

class OpenImagesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OpenImagesDialog(QWidget *parent = 0);
    ~OpenImagesDialog();

    QString imageFilepath() const;
    QString backgroundFilepath() const;

private slots:
    void verifyFilepath();
    void on_imageToolButton_clicked();
    void on_backgroundToolButton_clicked();
    void on_alignPushButton_clicked();
    void finishedProcess(int exitCode, QProcess::ExitStatus status);
    void saveSettings();

private:
    Ui::OpenImagesDialog *ui;
    QProcess *_process;
    QString _outputFilePath;
    QSettings _set;
};

#endif // OPENIMAGESDIALOG_H
