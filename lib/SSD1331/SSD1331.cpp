#include "SSD1331.h"

const uint8_t INITIALIZE_COMMAND[] = {
    0xae,
    0xa0, 0x72,
    0xa1, 0x00,
    0xa2, 0x00,
    0xa4,
    0xa8, 0x3f,
    0xad, 0x8e,
    0xb0, 0x0b,
    0xb1, 0x31,
    0xb3, 0xf0,
    0x8a, 0x64,
    0x8b, 0x78,
    0x8c, 0x64,
    0xbb, 0x3a,
    0xbe, 0x3e,
    0x87, 0x06,
    0x81, 0x91,
    0x82, 0x50,
    0x83, 0x7d,
    0xaf
};

/**
 * FPIOA, GPIOHS は正しく設定されていること
 */
SSD1331::SSD1331(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs): LCDBase(spi, SSD1331_SPI_FREQ, rst, dc, cs) {
}

/**
 * OLED を初期化する。
 */
void SSD1331::initialize() {
    sendReset();
    sendCommand(INITIALIZE_COMMAND, 37);
    usleep(110000);
}

/**
 * 描画範囲を設定する。
 */
void SSD1331::setRange(uint8_t x, uint8_t y, uint8_t width, uint8_t height) {
    uint8_t left = x;
    uint8_t right = x + width - 1;
    if (right >= SSD1331_WIDTH) {
        right = SSD1331_WIDTH - 1;
    }

    uint8_t top = y;
    uint8_t bottom = y + height - 1;
    if (bottom >= SSD1331_HEIGHT) {
        bottom = SSD1331_HEIGHT - 1;
    }

    uint8_t column[] = { 0x15, left, right };
    uint8_t row[] = { 0x75, top, bottom };
    sendCommand(column, 3);
    sendCommand(row, 3);
}

/**
 * GAC で直線を描画する。
 */
void SSD1331::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) {
    uint8_t r = (static_cast<uint8_t>(color >> 11) & 0b00011111) << 1;
    uint8_t g = (static_cast<uint8_t>(color >> 5) & 0b00111111);
    uint8_t b = (static_cast<uint8_t>(color) & 0b00011111) << 1;
    uint8_t command[] = {
        0x21,
        x1, y1,
        x2, y2,
        r, g, b
    };
    sendCommand(command, 8);
}

/**
 * GAC で矩形を描画する。
 */
void SSD1331::drawBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color) {
uint8_t r = (static_cast<uint8_t>(color >> 11) & 0b00011111) << 1;
    uint8_t g = (static_cast<uint8_t>(color >> 5) & 0b00111111);
    uint8_t b = (static_cast<uint8_t>(color) & 0b00011111) << 1;
    uint8_t command[] = {
        0x26, 0x00, // Fill disable
        0x22,
        x1, y1,
        x2, y2,
        r, g, b,
        0, 0, 0
    };
    sendCommand(command, 13);
}

/**
 * GAC で塗り潰し矩形を描画する。
 */
void SSD1331::drawFilledBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color, uint16_t fillColor) {
    uint8_t r = (static_cast<uint8_t>(color >> 11) & 0b00011111) << 1;
    uint8_t g = (static_cast<uint8_t>(color >> 5) & 0b00111111);
    uint8_t b = (static_cast<uint8_t>(color) & 0b00011111) << 1;
    uint8_t fr = (static_cast<uint8_t>(fillColor >> 11) & 0b00011111) << 1;
    uint8_t fg = (static_cast<uint8_t>(fillColor >> 5) & 0b00111111);
    uint8_t fb = (static_cast<uint8_t>(fillColor) & 0b00011111) << 1;
    uint8_t command[] = {
        0x26, 0x01, // Fill enable
        0x22,
        x1, y1,
        x2, y2,
        r, g, b,
        fr, fg, fb
    };
    sendCommand(command, 13);
}

/**
 * GAC で画面領域をコピーする。
 */
void SSD1331::copy(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3) {
    uint8_t command[] = {
        0x23,
        x1, y1,
        x2, y2,
        x3, y3
    };
    sendCommand(command, 7);
}
