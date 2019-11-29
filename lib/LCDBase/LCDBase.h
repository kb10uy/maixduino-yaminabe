#pragma once

#define _GNU_SOURCE
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <spi.h>
#include <gpiohs.h>

class LCDBase {
protected:
    spi_device_num_t spiNumber = SPI_DEVICE_0;
    uint32_t spiFrequency = 0;
    uint8_t pinReset = 0;
    uint8_t pinDataCommand = 0;
    uint8_t pinChipSelect = 0;

public:
    LCDBase(spi_device_num_t spi, uint32_t freq, uint8_t rst, uint8_t dc, uint8_t cs);
    void sendReset();
    void sendCommand(const uint8_t *command, size_t length);
    void sendData(const uint8_t *data, size_t length);
};