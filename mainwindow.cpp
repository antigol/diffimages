#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openimagesdialog.h"
#include <QAction>
#include <QMessageBox>
#include <QFileDialog>
#include <QSplitter>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _glwidget = new GLWidget(this);
    _imageViewer = new ImageViewer(this);
    _backgroundViewer = new ImageViewer(this);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    QWidget *widget = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setMargin(0);
    layout->addWidget(_imageViewer);
    layout->addWidget(_backgroundViewer);
    splitter->addWidget(widget);
    splitter->addWidget(_glwidget);
    splitter->setSizes(QList<int>() << 0.3 * height() << 0.7 * height());
    setCentralWidget(splitter);

    QMenu *file = menuBar()->addMenu("File");

    QAction *open = new QAction("Open images...", this);
    open->setShortcuts(QKeySequence::Open);
    connect(open, SIGNAL(triggered()), this, SLOT(openGUI()));
    file->addAction(open);


    QAction *capture = new QAction("Save a capture...", this);
    capture->setShortcut(QKeySequence::Print);
    connect(capture, SIGNAL(triggered()), this, SLOT(saveCaptureGUI()));
    file->addAction(capture);

    QAction *exportData = new QAction("export brut data...", this);
    exportData->setShortcut(QKeySequence::Save);
    connect(exportData, SIGNAL(triggered()), this, SLOT(exportBrutDataGUI()));
    file->addAction(exportData);

    ui->multihistogram->addHistogram(QColor(0, 0, 255, 150));
    ui->multihistogram->addHistogram(QColor(255, 0, 0, 200));

    connect(ui->bmiRadioButton, SIGNAL(clicked()), this, SLOT(changeSubtractionModeGUI()));
    connect(ui->imbRadioButton, SIGNAL(clicked()), this, SLOT(changeSubtractionModeGUI()));

    connect(_glwidget, SIGNAL(dataChanged()), this, SLOT(updateHistogramData()));

    connect(_glwidget, SIGNAL(mousePressChangeBackgroundFactor()), this, SLOT(readBackgroundFactor()));
    connect(ui->backgroundFactorBfDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeBackgroundFactorGUI()));

    connect(ui->savitzkyGolayFilterCheckBox, SIGNAL(clicked(bool)), this, SLOT(changeSavitzkyGolayFilterGUI(bool)));

    connect(ui->colormapBeginDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeColormapBeginGUI()));
    connect(ui->colormapEndDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeColormapEndGUI()));

    connect(ui->_3DViewCheckBox, SIGNAL(clicked(bool)), this, SLOT(change3DViewGUI()));
    connect(ui->zScaleDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeZScaleGUI()));


    ui->backgroundFactorBfDoubleSpinBox->setValue(_glwidget->backgroundFactor());
    ui->savitzkyGolayFilterCheckBox->setChecked(_glwidget->isSGFilterEnabled());
    ui->colormapBeginDoubleSpinBox->setValue(_glwidget->colormapBegin());
    ui->colormapEndDoubleSpinBox->setValue(_glwidget->colormapEnd());
    ui->histogram->setColorRange(ui->colormapBeginDoubleSpinBox->value(), ui->colormapEndDoubleSpinBox->value());
    ui->_3DViewCheckBox->setChecked(_glwidget->is3DMode());
    ui->zScaleDoubleSpinBox->setValue(_glwidget->zScale());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openGUI()
{
    OpenImagesDialog d(this);
    if (d.exec()) {
        QImage img(d.imageFilepath());
        if (img.isNull()) {
            QMessageBox::warning(this, "error", "The image is null.");
            return;
        }
        QImage bck(d.backgroundFilepath());
        if (bck.isNull()) {
            QMessageBox::warning(this, "error", "The background is null.");
            return;
        }
        if (!_glwidget->openImages(bck, img)) {
            QMessageBox::warning(this, "error", "Cannot load thoses images. They must have the same size.");
            return;
        }
        _imageViewer->setModificator(1.0, 0.0);
        _imageViewer->setImage(img);
        _backgroundViewer->setModificator(ui->backgroundFactorBfDoubleSpinBox->value(), 0.0);
        _backgroundViewer->setImage(bck);

        ui->multihistogram->setData(0, _imageViewer->data());
        ui->multihistogram->setData(1, _backgroundViewer->data());
        ui->multihistogram->update();

        _glwidget->updateGL();
    }
}

void MainWindow::changeSubtractionModeGUI()
{
    _glwidget->setSubtractMode(ui->bmiRadioButton->isChecked());
    _glwidget->updateGL();
}

void MainWindow::changeBackgroundFactorGUI()
{
    _glwidget->setBackgroundFactor(ui->backgroundFactorBfDoubleSpinBox->value());
    _backgroundViewer->setModificator(ui->backgroundFactorBfDoubleSpinBox->value(), 0.0);
    ui->multihistogram->setData(0, _imageViewer->data());
    ui->multihistogram->setData(1, _backgroundViewer->data());
    ui->multihistogram->update();
    _backgroundViewer->updateGL();
    _glwidget->updateGL();
}

void MainWindow::readBackgroundFactor()
{
    disconnect(ui->backgroundFactorBfDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeBackgroundFactorGUI()));
    ui->backgroundFactorBfDoubleSpinBox->setValue(_glwidget->backgroundFactor());
    connect(ui->backgroundFactorBfDoubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeBackgroundFactorGUI()));

    _backgroundViewer->setModificator(ui->backgroundFactorBfDoubleSpinBox->value(), 0.0);

    ui->multihistogram->setData(0, _imageViewer->data());
    ui->multihistogram->setData(1, _backgroundViewer->data());
    ui->multihistogram->update();
}

void MainWindow::changeSavitzkyGolayFilterGUI(bool on)
{
    _glwidget->setSGFilter(on);
    _glwidget->updateGL();
}

void MainWindow::changeColormapBeginGUI()
{
    _glwidget->setColormapBegin(ui->colormapBeginDoubleSpinBox->value());
    ui->histogram->setColorRange(ui->colormapBeginDoubleSpinBox->value(), ui->colormapEndDoubleSpinBox->value());
    _glwidget->updateGL();
}

void MainWindow::changeColormapEndGUI()
{
    _glwidget->setColormapEnd(ui->colormapEndDoubleSpinBox->value());
    ui->histogram->setColorRange(ui->colormapBeginDoubleSpinBox->value(), ui->colormapEndDoubleSpinBox->value());
    _glwidget->updateGL();
}

void MainWindow::updateHistogramData()
{
    ui->histogram->setData(_glwidget->data());
}

void MainWindow::change3DViewGUI()
{
    _glwidget->set3DMode(ui->_3DViewCheckBox->isChecked());
    _glwidget->updateGL();
}

void MainWindow::changeZScaleGUI()
{
    _glwidget->setZScale(ui->zScaleDoubleSpinBox->value());
    _glwidget->updateGL();
}

void MainWindow::saveCaptureGUI()
{
    QString filepath = QFileDialog::getSaveFileName(this, "capture filepath", _set.value("capture_filepath").toString());
    if (!filepath.isEmpty()) {
        _set.setValue("capture_filepath", filepath);

        _glwidget->capture().save(filepath);
    }
}

void MainWindow::exportBrutDataGUI()
{
    QString filepath = QFileDialog::getSaveFileName(this, "export file text", _set.value("export_filepath").toString());
    if (!filepath.isEmpty()) {
        _set.setValue("export_filepath", filepath);

        QFile file(filepath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "error", file.errorString());
            return;
        }

        QTextStream out(&file);
        QMessageBox::information(this, "format", "Pixels are returned in row order from the lowest to the highest row, left to right in each row.");
        for (int row = 0; row < _glwidget->imagesSize().height(); ++row) {
            for (int px = 0; px < _glwidget->imagesSize().width(); ++px) {
                out << _glwidget->data().at(row * _glwidget->imagesSize().width() + px) << " ";
            }
            out << endl;
        }
    }
}

