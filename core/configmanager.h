#pragma once
#include <QString>

class ConfigManager {
public:
    QString serverIP = "127.0.0.1";
    quint16 serverPort = 15000;

    void load(const QString &filename = "config.ini");
    void save(const QString &filename = "config.ini");
};
