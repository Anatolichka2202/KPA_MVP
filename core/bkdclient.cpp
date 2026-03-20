  // /core/include/bkd/protocol/unified_protocol.h
#pragma once
#include <cstdint>
#include <array>
#include <vector>
#include <optional>
#include <cstring>
#include <algorithm>

namespace BKD {
namespace Protocol {

// ==================== БАЗОВЫЕ КОНСТАНТЫ ====================
constexpr int NUM_BLOCKS = 12;              //12 блоков управления
constexpr int NUM_DRIVES_PER_BLOCK = 3;     // по 3 привода на блок
constexpr int NUM_PYRO_PER_BLOCK = 8;       // по 8 пиросредств на блок

constexpr size_t BKD_REQUEST_SIZE = 9;      // ЯЛС→БКД
constexpr size_t BKD_RESPONSE_SIZE = 95;    // БКД→ЯЛС
constexpr size_t YV_TO_YLS_SIZE = 128;      // ЯВ→ЯЛС
constexpr size_t YLS_TO_YV_SIZE = 8192;     // ЯЛС→ЯВ

// Команды
enum Command : uint8_t {
    CMD_REQUEST = 1,    // Запрос от ЯВ к БКД
    CMD_RESPONSE = 2,   // Ответ от БКД к ЯВ
    // ... другие команды по мере необходимости
};


// ==================== ПАКЕТ ЯЛС→БКД (9 байт) ====================
#pragma pack(push, 1)
struct BKDRequest {
    uint8_t block_number;           // [0] Номер блока (0-11)
    uint8_t command;                // [1] Команда (CMD_REQUEST)
    uint16_t drive_angles[3];       // [2-7] Углы 3 приводов
    uint8_t pyro_mask;              // [8] Маска пиро

    bool isValid() const {
        return block_number < NUM_BLOCKS &&
               command == CMD_REQUEST;
    }

    //                      МЕТОДЫ

    uint16_t getDriveAngle(int drive) const {
        if (drive >= 0 && drive < NUM_DRIVES_PER_BLOCK) {
            return drive_angles[drive];
        }
        return 0;
    }

    void setDriveAngle( int drive, uint16_t angle) {
        if (drive >= 0 && drive < NUM_DRIVES_PER_BLOCK) {
            drive_angles[drive] = angle;
        }
    }

    bool getPyroStatus(int pyro_index) const {
        if (pyro_index >= 0 && pyro_index < NUM_PYRO_PER_BLOCK) {
            return (pyro_mask >> pyro_index) & 1;
        }
        return false;
    }

    void setPyroStatus(int pyro_index, bool status) {
        if (pyro_index >= 0 && pyro_index < NUM_PYRO_PER_BLOCK) {
            if (status) {
                pyro_mask |= (1 << pyro_index);
            }
            else {
                pyro_mask &= ~(1 << pyro_index);
            }
        }
    }

};
#pragma pack(pop)
static_assert(sizeof(BKDRequest) == BKD_REQUEST_SIZE, "BKDRequest size mismatch");

// ==================== ПАКЕТ БКД→ЯЛС (95 байт) ====================
#pragma pack(push, 1)
struct BKDResponse {
    uint8_t block_number;           // [0] Номер блока
    uint8_t command;                // [1] Команда (CMD_RESPONSE)
    uint16_t drive_angles[3];       // [2-7] Углы приводов
    uint8_t yaz_data[16];           // [8-23] Данные ЯАЗ
    uint8_t pyro_mask;              // [24] Маска пиро
    uint8_t ylk_data[16];           // [25-40] Данные ЯЛК
    uint8_t yvp_data[54];           // [41-94] Данные ЯВП

    bool isValid() const {
        return block_number < NUM_BLOCKS &&
               command == CMD_RESPONSE;
    }

    //                      МЕТОДЫ

    uint16_t getDriveAngle(int drive) const {
        if (drive >= 0 && drive < NUM_DRIVES_PER_BLOCK) {
            return drive_angles[drive];
        }
        return 0;
    }

    void setDriveAngle(int drive, uint16_t angle) {
        if (drive >= 0 && drive < NUM_DRIVES_PER_BLOCK) {
            drive_angles[drive] = angle;
        }
    }

    bool getPyroStatus(int pyro_index) const {
        if (pyro_index >= 0 && pyro_index < NUM_PYRO_PER_BLOCK) {
            return (pyro_mask >> pyro_index) & 1;
        }
        return false;
    }

    void setPyroStatus(int pyro_index, bool status) {
        if (pyro_index >= 0 && pyro_index < NUM_PYRO_PER_BLOCK) {
            if (status) {
                pyro_mask |= (1 << pyro_index);
            }
            else {
                pyro_mask &= ~(1 << pyro_index);
            }
        }
    }

};
#pragma pack(pop)
static_assert(sizeof(BKDResponse) == BKD_RESPONSE_SIZE, "BKDResponse size mismatch");




// ==================== ПАКЕТ ЯВ→ЯЛС (128 байт) ====================
#pragma pack(push, 1)
struct YVToYLSPacket {
    uint8_t address_mask;           // [0] Битовая маска адресации (12 бит)
    uint8_t command;                // [1] Команда для всех блоков
    uint16_t drives[NUM_BLOCKS][NUM_DRIVES_PER_BLOCK];  // [2-73] Углы для 12 блоков
    uint8_t pyro_masks[NUM_BLOCKS]; // [74-85] Маски пиро для 12 блоков
    uint8_t reserved[42];           // [86-127] Резерв

    bool isBlockActive(uint8_t block) const {
        return block < NUM_BLOCKS && (address_mask & (1 << block));
    }

    // Получить запрос для конкретного блока
    BKDRequest getBlockRequest(uint8_t block) const {
        BKDRequest req{};
        if (isBlockActive(block)) {
            req.block_number = block;
            req.command = command;
            req.drive_angles[0] = drives[block][0];
            req.drive_angles[1] = drives[block][1];
            req.drive_angles[2] = drives[block][2];
            req.pyro_mask = pyro_masks[block];
        }
        return req;
    }

    // Установить данные для блока
    void setBlockData(uint8_t block, const BKDRequest& request) {
        if (block < NUM_BLOCKS) {
            address_mask |= (1 << block);
            drives[block][0] = request.drive_angles[0];
            drives[block][1] = request.drive_angles[1];
            drives[block][2] = request.drive_angles[2];
            pyro_masks[block] = request.pyro_mask;
        }
    }
};
#pragma pack(pop)
static_assert(sizeof(YVToYLSPacket) == YV_TO_YLS_SIZE, "YVToYLSPacket size mismatch");

// ==================== ПАКЕТ ЯЛС→ЯВ (8192 байт) ====================
#pragma pack(push, 1)
struct YLSToYVPacket {
    uint8_t address_mask;           // [0] Маска ответивших блоков
    uint8_t command;                // [1] Команда (CMD_RESPONSE)
    uint16_t drives[NUM_BLOCKS][NUM_DRIVES_PER_BLOCK];  // [2-73] Углы от блоков
    uint8_t yaz_data[192];          // [74-265] Данные ЯАЗ (12×16)
    uint8_t pyro_masks[NUM_BLOCKS]; // [266-277] Маски пиро
    uint8_t ylk_data[192];          // [278-469] Данные ЯЛК (12×16)
    uint8_t yvp_data[648];          // [470-1117] Данные ЯВП (12×54)
    uint8_t reserved[7074];         // [1118-8191] Резерв

    // Установить ответ от блока
    void setBlockResponse(uint8_t block, const BKDResponse& response) {
        if (block < NUM_BLOCKS) {
            address_mask |= (1 << block);

            drives[block][0] = response.drive_angles[0];
            drives[block][1] = response.drive_angles[1];
            drives[block][2] = response.drive_angles[2];

            // Копируем данные подсистем
            std::memcpy(&yaz_data[block * 16], response.yaz_data, 16);
            pyro_masks[block] = response.pyro_mask;
            std::memcpy(&ylk_data[block * 16], response.ylk_data, 16);
            std::memcpy(&yvp_data[block * 54], response.yvp_data, 54);
        }
    }

    // Получить ответ блока
    BKDResponse getBlockResponse(uint8_t block) const {
        BKDResponse resp{};
        if (block < NUM_BLOCKS && (address_mask & (1 << block))) {
            resp.block_number = block;
            resp.command = command;
            resp.drive_angles[0] = drives[block][0];
            resp.drive_angles[1] = drives[block][1];
            resp.drive_angles[2] = drives[block][2];

            std::memcpy(resp.yaz_data, &yaz_data[block * 16], 16);
            resp.pyro_mask = pyro_masks[block];
            std::memcpy(resp.ylk_data, &ylk_data[block * 16], 16);
            std::memcpy(resp.yvp_data, &yvp_data[block * 54], 54);
        }
        return resp;
    }
};
#pragma pack(pop)


// Преобразование сырого значения в угол в градусах (-8.0 до +8.0) - позже будет доработано и добавлено
inline double rawToAngle(uint16_t raw) {
    // Предполагаю, что значение представляет собой фиксированную точку
    // или использует определенный масштаб
    return (static_cast<double>(raw) / 4096.0 - 8.0);
}

// Преобразование угла в градусах в сырое значение
inline uint16_t angleToRaw(double angle) {

    // Ограничиваю угол диапазоном -8.0 до +8.0
    angle = (std::max)(-8.0, (std::min)(8.0, angle));
    return static_cast<uint16_t>((angle + 8.0) * 4096.0);
}


} // namespace Protocol
} // namespace BKD
