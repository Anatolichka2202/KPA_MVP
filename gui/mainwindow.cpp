#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blockwidget.h"
#include "BKDClient.h"
#include <QMessageBox>
#include <QDebug>
#include <QScrollArea>
#include "configmanager.h"
#include <QSettings>
#include <QFile>
#include <settingsdialog.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_sentCount(0)
    , m_receivedCount(0)
{
    qDebug() << "MainWindow constructor started";
    ui->setupUi(this);




     m_bkdClient = new BKDClient(this);
     qDebug() << "BKDClient created";

    // Создаем 12 виджетов блоков и добавляем в gridLayout из UI
    m_blockWidgets.resize(12);
    for (int i = 0; i < 12; ++i) {
        m_blockWidgets[i] = new BlockWidget(i, this);
        // Распределяем по 4 столбцам (3 строки × 4 столбца)
        ui->gridLayout->addWidget(m_blockWidgets[i], i / 4, i % 4);
        connect(m_blockWidgets[i], &BlockWidget::pyroMaskChanged,m_bkdClient, &BKDClient::setPyroMask);
    }
    qDebug() << "All block widgets created";

    setupStatusBar();
    qDebug() << "Status bar setup";

    // Сигналы BKDClient

    connect(m_bkdClient, &BKDClient::driveDataUpdated,
            this, &MainWindow::onDriveDataUpdated);
    connect(m_bkdClient, &BKDClient::pyroDataUpdated,
            this, &MainWindow::onPyroDataUpdated);
    connect(m_bkdClient, &BKDClient::connectionStatusChanged,
            this, &MainWindow::onConnectionStatusChanged);
    connect(m_bkdClient, &BKDClient::errorOccurred,
            this, &MainWindow::onErrorOccurred);
    connect(m_bkdClient, &BKDClient::dataSent,
            this, &MainWindow::onDataSent);
    connect(m_bkdClient, &BKDClient::dataReceived,
            this, &MainWindow::onDataReceived);

      qDebug() << "Starting BKDClient";

    ConfigManager config;
    config.load();

    if (!m_bkdClient->start(config.serverIP, config.serverPort, 20)) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не удалось запустить BKD клиент\nПроверьте настройки в config.ini\nIP: %1\nPort: %2")
                                  .arg(config.serverIP)
                                  .arg(config.serverPort));
    }

    qDebug() << "MainWindow constructor completed";
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Готов к работе");
    m_connectionLabel = new QLabel("Подключение...");
    m_statsLabel = new QLabel("Отправлено: 0 | Получено: 0");

    statusBar()->addWidget(m_statusLabel);
    statusBar()->addWidget(m_connectionLabel);
    statusBar()->addPermanentWidget(m_statsLabel);
}

void MainWindow::onDriveDataUpdated(const QVector<double> &angles)
{
    // angles содержит 36 значений (12 блоков × 3 привода)
    for (int block = 0; block < 12; ++block) {
        QVector<double> blockAngles;
        for (int drive = 0; drive < 3; ++drive) {
            int index = block * 3 + drive;
            if (index < angles.size()) {
                blockAngles.append(angles[index]);
            }
        }
        // Обновляем данные приводов
        if (block < m_blockWidgets.size()) {
            m_blockWidgets[block]->updateData(blockAngles, 0);
        }
    }
    m_statusLabel->setText("Данные углов обновлены");
}

void MainWindow::onPyroDataUpdated(const QVector<quint16> &masks)
{
    // masks содержит 12 значений (по одному на блок)
    for (int block = 0; block < 12 && block < masks.size(); ++block) {
        if (block < m_blockWidgets.size()) {
            // Передаем маску от сервера (только ненулевую)
            QVector<double> emptyAngles;
            m_blockWidgets[block]->updateData(emptyAngles, static_cast<quint8>(masks[block]));
        }
    }
    m_statusLabel->setText("Данные пироотсеков обновлены");
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
    if (connected) {
        m_connectionLabel->setText("✓ Подключено к БКД");
        statusBar()->setStyleSheet("QStatusBar { background-color: #90EE90; }");
    } else {
        m_connectionLabel->setText("✗ Отключено от БКД");
        statusBar()->setStyleSheet("QStatusBar { background-color: #FFB6C1; }");
    }
}

void MainWindow::onErrorOccurred(const QString &error)
{
    m_statusLabel->setText("Ошибка: " + error);
    statusBar()->setStyleSheet("QStatusBar { background-color: #FFB6C1; }");
    qDebug() << "BKDClient error:" << error;
}

void MainWindow::onDataSent()
{
    m_sentCount++;
    m_statsLabel->setText(QString("Отправлено: %1 | Получено: %2").arg(m_sentCount).arg(m_receivedCount));
}

void MainWindow::onDataReceived()
{
    m_receivedCount++;
    m_statsLabel->setText(QString("Отправлено: %1 | Получено: %2").arg(m_sentCount).arg(m_receivedCount));
}

void MainWindow::onSettingsActionTriggered() {
    ConfigManager config;
    config.load();

    SettingsDialog dialog(this);
    dialog.setConfig(config);

    if (dialog.exec() == QDialog::Accepted) {
        ConfigManager newConfig = dialog.getConfig();
        newConfig.save();

        // Переподключаемся
        m_bkdClient->stop();
        m_bkdClient->start(newConfig.serverIP, newConfig.serverPort, 20);
    }
}

void MainWindow::on_settingsButton_clicked()
{
    ConfigManager config;
    config.load();

    SettingsDialog dialog(this);
    dialog.setConfig(config);

    if (dialog.exec() == QDialog::Accepted) {
        ConfigManager newConfig = dialog.getConfig();
        newConfig.save();

        // Перезапускаем подключение
        m_bkdClient->stop();
        if (!m_bkdClient->start(newConfig.serverIP, newConfig.serverPort, 20)) {
            QMessageBox::critical(this, "Ошибка",
                                  "Не удалось переподключиться с новыми настройками");
        }
    }
}

/*void MainWindow::createMenus()
{
    QMenuBar *menuBar = new QMenuBar(this);
    setMenuBar(menuBar);

    QMenu *connectionMenu = menuBar->addMenu("Подключение");

    m_settingsAction = new QAction("Настройки", this);
    m_reconnectAction = new QAction("Переподключиться", this);

    connectionMenu->addAction(m_settingsAction);
    connectionMenu->addAction(m_reconnectAction);

    connect(m_settingsAction, &QAction::triggered,
            this, &MainWindow::onSettingsActionTriggered);
    connect(m_reconnectAction, &QAction::triggered,
            this, &MainWindow::onReconnectActionTriggered);
}
*/
