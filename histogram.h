#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QWidget>
#include <QVector>
#include <QMap>

class Histogram : public QWidget
{
    Q_OBJECT
public:
    explicit Histogram(QWidget *parent = 0);
    void setData(const QVector<float> &data);
    void setColorRange(float begin, float end);
    
protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QVector<float> _data;
    float _colorbegin, _colorEnd;
};

#endif // HISTOGRAM_H
