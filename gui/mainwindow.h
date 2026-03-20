#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include "configmanager.h"
#include "settingsdialog.h"
// Предварительное объявление классов
class BlockWidget;
class BKDClient;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDriveDataUpdated(const QVector<double> &angles);
    void onPyroDataUpdated(const QVector<quint16> &masks);
    void onConnectionStatusChanged(bool connected);
    void onErrorOccurred(const QString &error);
    void onDataSent();
    void onDataReceived();
   void on_settingsButton_clicked();
    void onSettingsActionTriggered();

private:
    void setupStatusBar();

    void createMenus();

    ConfigManager m_configManager;
    QAction *m_settingsAction;
    QAction *m_reconnectAction;


    Ui::MainWindow *ui;
    QVector<BlockWidget*> m_blockWidgets;
    BKDClient *m_bkdClient;

    // Элементы статусной строки
    QLabel *m_statusLabel;
    QLabel *m_connectionLabel;
    QLabel *m_statsLabel;

    // Статистика
    int m_sentCount;
    int m_receivedCount;
};

#endif // MAINWINDOW_H
