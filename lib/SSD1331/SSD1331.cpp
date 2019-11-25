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
SSD1331::SSD1331(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs) {
    spiNumber = spi;
    pinReset = rst;
    pinDataCommand = dc;
    pinChipSelect = cs;
}

/**
 * OLED を初期化する。
 */
void SSD1331::initialize() {
    gpiohs_set_pin(pinReset, GPIO_PV_HIGH);
    gpiohs_set_pin(pinReset, GPIO_PV_LOW);
    usleep(1000);
    gpiohs_set_pin(pinReset, GPIO_PV_HIGH);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
    gpiohs_set_pin(pinDataCommand, GPIO_PV_HIGH);

    sendCommand(INITIALIZE_COMMAND, 37);
    usleep(110000);
}

/**
 * コマンドを送信する。
 */
void SSD1331::sendCommand(const uint8_t *command, size_t length) {
    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, SSD1331_SPI_FREQ);

    gpiohs_set_pin(pinDataCommand, GPIO_PV_LOW);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    spi_send_data_standard(spiNumber, SPI_CHIP_SELECT_0, NULL, 0, command, length);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}

/**
 * データ(バッファデータ) を送信する。
 */
void SSD1331::sendData(const uint8_t *data, size_t length) {
    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, SSD1331_SPI_FREQ);
    gpiohs_set_pin(pinDataCommand, GPIO_PV_HIGH);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);

    spi_send_data_standard(spiNumber, SPI_CHIP_SELECT_0, NULL, 0, data, length);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
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