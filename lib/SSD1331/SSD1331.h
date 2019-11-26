#pragma once

#define _GNU_SOURCE

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <spi.h>
#include <gpiohs.h>

#define SSD1331_SPI_FREQ 5000000UL
#define SSD1331_WIDTH 96
#define SSD1331_HEIGHT 64

class SSD1331 {
private:
    spi_device_num_t spiNumber = SPI_DEVICE_0;
    uint8_t pinReset = 0;
    uint8_t pinDataCommand = 0;
    uint8_t pinChipSelect = 0;

public:
    SSD1331(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs);

    void initialize();
    void sendCommand(const uint8_t *command, size_t length);
    void sendData(const uint8_t *data, size_t length);
    void setRange(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
};
