#include "configmanager.h"
#include <QSettings>

void ConfigManager::load(const QString &filename) {
    QSettings settings(filename, QSettings::IniFormat);
    serverIP = settings.value("serverIP", "127.0.0.1").toString();
    serverPort = settings.value("serverPort", 15000).toInt();
}

void ConfigManager::save(const QString &filename) {
    QSettings settings(filename, QSettings::IniFormat);
    settings.setValue("serverIP", serverIP);
    settings.setValue("serverPort", serverPort);
    settings.sync();
}
