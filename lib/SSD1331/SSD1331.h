#pragma once

#define _GNU_SOURCE

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <spi.h>
#include <gpiohs.h>

#include <LCDBase.h>

#define SSD1331_SPI_FREQ 5000000UL
#define SSD1331_WIDTH 96
#define SSD1331_HEIGHT 64
#define COLOR888TO565(r, g, b) ((uint16_t) ((r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)))

class SSD1331 : public LCDBase {
public:
    SSD1331(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs);

    void initialize();
    void setRange(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
    void drawBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
    void drawFilledBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color, uint16_t fillColor);
    void copy(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3);
};
