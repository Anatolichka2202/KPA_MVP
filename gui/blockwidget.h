#ifndef BLOCKWIDGET_H
#define BLOCKWIDGET_H

#include <QWidget>
#include <QtCharts>

namespace Ui {
class BlockWidget;
}

class BlockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BlockWidget(int blockIndex, QWidget *parent = nullptr);
    ~BlockWidget();

    void updateData(const QVector<double> &driveAngles, quint8 pyroMask);

signals:
    void pyroMaskChanged(int blockIndex, quint8 mask);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;  // Добавляем эту строку

private:
    void setupChart();
    void updateCircles(quint8 mask);
    void setupPyroClickHandlers();

    Ui::BlockWidget *ui;
    int m_blockIndex;
    QBarSeries *m_series;
    QBarSet *m_barSet;

    QVector<QWidget*> m_pyroCircles; // Для удобного доступа к кружкам
    quint8 m_currentPyroMask = 0; // Текущая маска блока
};

#endif // BLOCKWIDGET_H
