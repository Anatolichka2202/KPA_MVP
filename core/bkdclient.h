#ifndef BKDCLIENT_H
#define BKDCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QVector>
#include "Protocol.h"

class BKDClient : public QObject
{
    Q_OBJECT

public:
    explicit BKDClient(QObject *parent = nullptr);
    ~BKDClient();

    bool start(const QString &serverAddress, quint16 serverPort, int updateIntervalMs = 1000);
    void stop();
    bool isRunning() const { return m_timer->isActive(); }
    void setPyroMask(int blockIndex, quint8 mask); //метод установки маски
signals:
    void driveDataUpdated(const QVector<double> &angles);
    void pyroDataUpdated(const QVector<quint16> &masks);
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString &error);
    void dataSent();
    void dataReceived();

private slots:
    void onReadyRead();
    void onTimeout();

private:
    void generateTestData();
    void sendRequest();
    void processResponse(const YVProtocol::ResponsePacket &response);

    quint8 m_pyroMasks[YVProtocol::NUM_BLOCKS] = {0}; //маски
    QUdpSocket *m_socket;
    QTimer *m_timer;

    YVProtocol::RequestPacket m_requestPacket;
    YVProtocol::ResponsePacket m_responsePacket;

    QHostAddress m_serverAddress;
    quint16 m_serverPort;
    bool m_connected;
};

#endif // BKDCLIENT_H
