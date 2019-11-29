#pragma once

#include <stddef.h>
#include <stdint.h>

#include <gpiohs.h>
#include <spi.h>

#include <LCDBase.h>

#define ILI9341_SPI_FREQUENCY 5000000UL

class ILI9341 : public LCDBase {
public:
    ILI9341(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs);

    void initialize();
};
