#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include "glwidget.h"
#include "imageviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void openGUI();
    
private slots:
    void changeSubtractionModeGUI();
    void changeBackgroundFactorGUI();
    void readBackgroundFactor();
    void changeSavitzkyGolayFilterGUI(bool on);
    void changeColormapBeginGUI();
    void changeColormapEndGUI();
    void updateHistogramData();
    void change3DViewGUI();
    void changeZScaleGUI();
    void saveCaptureGUI();
    void exportBrutDataGUI();

private:
    Ui::MainWindow *ui;

    GLWidget *_glwidget;
    ImageViewer *_imageViewer;
    ImageViewer *_backgroundViewer;

    QSettings _set;
};

#endif // MAINWINDOW_H
