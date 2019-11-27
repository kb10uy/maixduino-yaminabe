#pragma once

#include <stdint.h>
#include <stddef.h>

#include <gpiohs.h>
#include <spi.h>

#define BME280_SPI_FREQ 10000000UL

class BME280 {
private:
    spi_device_num_t spiNumber = SPI_DEVICE_0;
    uint8_t pinChipSelect = 0;

    uint16_t digT1 = 0;
    int16_t digT2 = 0;
    int16_t digT3 = 0;
    uint16_t digP1 = 0;
    int16_t digP2 = 0;
    int16_t digP3 = 0;
    int16_t digP4 = 0;
    int16_t digP5 = 0;
    int16_t digP6 = 0;
    int16_t digP7 = 0;
    int16_t digP8 = 0;
    int16_t digP9 = 0;
    uint8_t digH1 = 0;
    int16_t digH2 = 0;
    uint8_t digH3 = 0;
    int16_t digH4 = 0;
    int16_t digH5 = 0;
    int8_t digH6 = 0;

    int32_t fineTemperature = 0;

    void writeRegister(uint8_t address, uint8_t value);
    void readRegister(uint8_t address, uint8_t *buffer, size_t size);
    void readDig();

public:
    BME280(spi_device_num_t spi, uint8_t cs);

    void initialize();
    double temperature();
    double humidity();
    double pressure();
};
