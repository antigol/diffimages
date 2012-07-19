#include "histogram.h"
#include <QPainter>
#include <QTime>

Histogram::Histogram(QWidget *parent) :
    QWidget(parent)
{
}

void Histogram::setData(const QVector<float> &data)
{
    _data = data;
    qSort(_data);

    update();
}

void Histogram::setColorRange(float begin, float end)
{
    _colorbegin = begin;
    _colorEnd = end;
}

void Histogram::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    if (_data.isEmpty()) {
        painter.drawText(rect(), Qt::AlignCenter, "no data");
        return;
    }

    float beg = _data.first();
    float end = _data.last();

    if (beg >= end) {
        painter.drawText(rect(), Qt::AlignCenter, "data error");
        return;
    }

    float tot = end - beg;
    float sub = tot / float(width());

    QVector<float> hist;
    float cmax = 0;
    int ii = 0;
    for (int x = 0; x < width(); ++x) {
        float c = 0.0;
        for (int i = ii; i < _data.size(); ++i) {
            if (_data[i] < ((x+1) * sub + beg)) {
                if (_data[i] >= (x * sub + beg))
                    c++;
            } else {
                ii = i;
                break;
            }
        }
        cmax = qMax(cmax, c);

        hist << c;
    }

    float k = float(height()) / cmax;
    for (int x = 0; x < width(); ++x) {
        float median = (x+0.5) * sub + beg;
        if (median >= _colorbegin && median <= _colorEnd)
            painter.setPen(Qt::red);
        else
            painter.setPen(Qt::black);
        painter.drawLine(x, height(), x, height() - hist[x] * k);
    }
}
