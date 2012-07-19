#include "multihistogram.h"
#include <QPainter>

MultiHistogram::MultiHistogram(QWidget *parent) :
    QWidget(parent)
{
}

int MultiHistogram::addHistogram(const QColor &color)
{
    _datas.append(QPair<QColor, QVector<float> >(color, QVector<float>()));
    return _datas.size() - 1;
}

void MultiHistogram::setData(int id, const QVector<float> &data)
{
    _datas[id].second = data;
    qSort(_datas[id].second);
}

void MultiHistogram::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(rect(), Qt::white);

    bool empty = true;
    for (int i = 0; i < _datas.size(); ++i)
        if (!_datas[i].second.isEmpty())
            empty = false;

    if (empty) {
        painter.drawText(rect(), Qt::AlignCenter, "no data");
        return;
    }

    float beg = 0.0;
    float end = 0.0;

    for (int i = 0; i < _datas.size(); ++i)
        if (!_datas[i].second.isEmpty()) {
            beg = _datas[i].second.first();
            end = _datas[i].second.last();
        }

    for (int i = 0; i < _datas.size(); ++i)
        if (!_datas[i].second.isEmpty()) {
            beg = qMin(beg, _datas[i].second.first());
            end = qMax(end, _datas[i].second.last());
        }

    if (beg >= end) {
        painter.drawText(rect(), Qt::AlignCenter, "data error");
        return;
    }

    float tot = end - beg;
    float sub = tot / float(width());

    QList<QVector<float> > hists;
    float cmax = 0;

    for (int i = 0; i < _datas.size(); ++i) {
        QVector<float> hist;
        int jj = 0;
        for (int x = 0; x < width(); ++x) {
            float c = 0.0;
            for (int j = jj; j < _datas[i].second.size(); ++j) {
                if (_datas[i].second[j] < ((x+1) * sub + beg)) {
                    if (_datas[i].second[j] >= (x * sub + beg))
                        c++;
                } else {
                    jj = j;
                    break;
                }
            }
            cmax = qMax(cmax, c);

            hist << c;
        }
        hists << hist;
    }

    float k = float(height()) / cmax;
    for (int i = 0; i < _datas.size(); ++i) {
        painter.setPen(_datas[i].first);
        for (int x = 0; x < width(); ++x) {
            painter.drawLine(x, height(), x, height() - hists[i][x] * k);
        }
    }
}
