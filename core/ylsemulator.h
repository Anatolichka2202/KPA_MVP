#ifndef YLSEMULATOR_H
#define YLSEMULATOR_H

#pragma once
#include "Protocol.h"
#include <vector>
#include <memory>

class YLSEmulator {
public:
    enum class Mode {
        EMULATE,    // Разбиваем на маленькие пакеты
        PASSTHROUGH // Проксируем на реальный ЯЛС
    };

    YLSEmulator(Mode mode = Mode::EMULATE);

    // Принимает большой пакет от ЯВ, возвращает большой пакет от ЯЛС
    BKD::Protocol::YLSToYVPacket process(
        const BKD::Protocol::YVToYLSPacket& yv_packet);

    void setMode(Mode mode) { mode_ = mode; }
    Mode getMode() const { return mode_; }

private:
    Mode mode_;

    // Для эмуляции: адреса БКД
    struct BKDEndpoint {
        std::string ip;
        uint16_t port;
    };
    std::vector<BKDEndpoint> bkd_endpoints_;

    // Отправка маленького пакета на БКД (реальную или эмулированную)
    BKD::Protocol::BKDResponse sendToBKD(
        uint8_t block_num,
        const BKD::Protocol::BKDRequest& request);
};

#endif // YLSEMULATOR_H
