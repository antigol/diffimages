#ifndef MULTIHISTOGRAM_H
#define MULTIHISTOGRAM_H

#include <QWidget>
#include <QList>
#include <QPair>
#include <QColor>
#include <QVector>

class MultiHistogram : public QWidget
{
    Q_OBJECT
public:
    explicit MultiHistogram(QWidget *parent = 0);
    int addHistogram(const QColor &color);
    void setData(int id, const QVector<float> &data);
    
protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QList<QPair<QColor, QVector<float> > > _datas;
};

#endif // MULTIHISTOGRAM_H
