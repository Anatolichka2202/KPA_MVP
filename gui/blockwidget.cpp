#include "blockwidget.h"
#include "ui_blockwidget.h"
#include <QChart>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

BlockWidget::BlockWidget(int blockIndex, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BlockWidget)
    , m_blockIndex(blockIndex)
{
    qDebug() << "Creating BlockWidget" << blockIndex;
    ui->setupUi(this);

    // Устанавливаем заголовок блока
    ui->blockLabel->setText(QString("БЛОК %1").arg(m_blockIndex + 1));

    setupChart();
    setupPyroClickHandlers();
    qDebug() << "BlockWidget" << blockIndex << "created successfully";
}

BlockWidget::~BlockWidget()
{
    delete ui;
}

void BlockWidget::setupPyroClickHandlers()
{
    qDebug() << "Setting up pyro click handlers for block" << m_blockIndex;
    // Собираем все кружки в вектор
    m_pyroCircles = {
        ui->pyro1, ui->pyro2, ui->pyro3, ui->pyro4,
        ui->pyro5, ui->pyro6, ui->pyro7, ui->pyro8
    };
    //  обработчик кликов для каждого кружка
    for (int i = 0; i < m_pyroCircles.size(); ++i) {
        //  обработку событий мыши
        m_pyroCircles[i]->setAttribute(Qt::WA_TransparentForMouseEvents, false);

        //  фильтр событий
         qDebug() << "Setting up event filter for pyro" << i;
        m_pyroCircles[i]->installEventFilter(this);
    }
      qDebug() << "Pyro click handlers setup completed for block" << m_blockIndex;
}

bool BlockWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // Проверяем, какой кружок был нажат
        for (int i = 0; i < m_pyroCircles.size(); ++i) {
            if (obj == m_pyroCircles[i]) {
                // Инвертируем соответствующий бит
                m_currentPyroMask ^= (1 << i);

                // Обновляем отображение
                updateCircles(m_currentPyroMask);

                // Отправляем сигнал с новой маской
                emit pyroMaskChanged(m_blockIndex, m_currentPyroMask);

                return true; // Событие обработано
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}


void BlockWidget::setupChart()
{
    // Создаем и настраиваем chart
    QChart *chart = new QChart();

    // Настройка данных для гистограммы
    m_series = new QBarSeries();
    m_barSet = new QBarSet("");

    // Начальные значения для трех приводов
    *m_barSet << 0 << 0 << 0;
    m_series->append(m_barSet);
    chart->addSeries(m_series);
    chart->setTitle("");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setBackgroundRoundness(0);
    chart->setMargins(QMargins(2, 2, 2, 2)); // Уменьшаем отступы

    // Ось X
    QStringList categories {"Пр.1", "Пр.2", "Пр.3"};
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisX, Qt::AlignBottom);
    m_series->attachAxis(axisX);

    // Ось Y
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 70000);
    axisY->setTitleText("Угол");
    axisY->setTitleFont(QFont("Arial", 8));
    axisY->setLabelsFont(QFont("Arial", 7));
    axisY->setLabelFormat("%.0f");
    chart->addAxis(axisY, Qt::AlignLeft);
    m_series->attachAxis(axisY);

    // Легенда - убираем, чтобы сэкономить место
    chart->legend()->setVisible(false);

    // Устанавливаем chart в chartView
    ui->chartView->setChart(chart);
    ui->chartView->setRenderHint(QPainter::Antialiasing);
}

void BlockWidget::updateData(const QVector<double> &driveAngles, quint8 pyroMask)
{
    // Обновляем гистограмму только если переданы новые данные углов
    if (!driveAngles.isEmpty()) {
        for (int i = 0; i < 3 && i < driveAngles.size(); ++i) {
            m_barSet->replace(i, driveAngles[i]);
        }
    }

    // Обновляем кружки пиросредств
    if (pyroMask != 0) { // 0 означает, что маска не обновлялась
        m_currentPyroMask = pyroMask;
        updateCircles(pyroMask);
    }
}
void BlockWidget::updateCircles(quint8 mask)
{
    // Явно перечисляем все кружки
    QVector<QWidget*> circles = {
        ui->pyro1, ui->pyro2, ui->pyro3, ui->pyro4, ui->pyro5, ui->pyro6, ui->pyro7, ui->pyro8
    };

    for (int i = 0; i < circles.size() && i < 8; ++i) {
        bool isActive = (mask >> i) & 1;
        QString color = isActive ? "#FF4444" : "#CCCCCC";
        QString borderColor = isActive ? "#AA0000" : "#666666";
        circles[i]->setStyleSheet(
            QString("background-color: %1; border: 2px solid %2; border-radius: 10px;")
                .arg(color).arg(borderColor)
            );
    }
}

