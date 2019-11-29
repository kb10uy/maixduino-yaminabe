#include "LCDBase.h"

LCDBase::LCDBase(spi_device_num_t spi, uint32_t freq, uint8_t rst, uint8_t dc, uint8_t cs) {
    spiNumber = spi;
    spiFrequency = freq;
    pinReset = rst;
    pinDataCommand = dc;
    pinChipSelect = cs;
}

/**
 * リセットをかける。
 */
void LCDBase::sendReset() {
    gpiohs_set_pin(pinReset, GPIO_PV_HIGH);
    gpiohs_set_pin(pinReset, GPIO_PV_LOW);
    usleep(50000);
    gpiohs_set_pin(pinReset, GPIO_PV_HIGH);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
    gpiohs_set_pin(pinDataCommand, GPIO_PV_HIGH);
    usleep(50000);
}

/**
 * コマンドとデータをそれぞれ送信する。
 */
void LCDBase::sendCommandData(uint8_t command, const uint8_t *data, size_t length) {
    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, spiFrequency);

    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    gpiohs_set_pin(pinDataCommand, GPIO_PV_LOW);
    spi_send_data_normal_dma(DMAC_CHANNEL0, spiNumber, SPI_CHIP_SELECT_0, &command, 1, SPI_TRANS_CHAR);
    if (data != nullptr && length != 0) {
        gpiohs_set_pin(pinDataCommand, GPIO_PV_HIGH);
        spi_send_data_normal_dma(DMAC_CHANNEL0, spiNumber, SPI_CHIP_SELECT_0, data, length, SPI_TRANS_CHAR);
    }
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}

/**
 * コマンドを送信する。
 */
void LCDBase::sendCommand(const uint8_t *command, size_t length) {
    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, spiFrequency);

    gpiohs_set_pin(pinDataCommand, GPIO_PV_LOW);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    spi_send_data_normal_dma(DMAC_CHANNEL0, spiNumber, SPI_CHIP_SELECT_0, command, length, SPI_TRANS_CHAR);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}

/**
 * データ(バッファデータ) を送信する。
 */
void LCDBase::sendData(const uint8_t *data, size_t length) {
    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, spiFrequency);

    gpiohs_set_pin(pinDataCommand, GPIO_PV_HIGH);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    spi_send_data_normal_dma(DMAC_CHANNEL0, spiNumber, SPI_CHIP_SELECT_0, data, length, SPI_TRANS_CHAR);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}
