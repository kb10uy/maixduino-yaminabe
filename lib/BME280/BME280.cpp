#include "BME280.h"

BME280::BME280(spi_device_num_t spi, uint8_t cs) {
    spiNumber = spi;
    pinChipSelect = cs;
}

/**
 * 初期化する。
 */
void BME280::initialize() {
    uint8_t humidityOversampling = 0b001;       // x1 oversampling
    uint8_t temperatureOversampling = 0b001;    // x1 oversampling
    uint8_t pressureOversampling = 0b001;       // x1 oversampling
    uint8_t mode = 0b11;                        // normal mode
    uint8_t standbyTime = 0b011;                // t_{sb} 250ms
    uint8_t filter = 0b000;                     // filter disabled
    uint8_t spi3wire = 0b0;                     // 3-wire SPI disabled

    uint8_t ctrl_hum = humidityOversampling;
    uint8_t ctrl_meas = temperatureOversampling << 5 | pressureOversampling << 2 | mode;
    uint8_t config = standbyTime << 5 | filter << 2 | spi3wire;

    writeRegister(0xf2, ctrl_hum);
    writeRegister(0xf4, ctrl_meas);
    writeRegister(0xf5, config);
    readDig();
}

/**
 * 気温を読み出す。
 */
double BME280::temperature() {
    uint8_t buffer[3];
    uint32_t rawValue;

    readRegister(0xfa, buffer, 3);
    rawValue = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

    int32_t var1, var2, T;
    var1 = ((((rawValue >> 3) - ((int32_t) digT1 << 1))) * ((int32_t) digT2)) >> 11;
    var2 = (((((rawValue >> 4) - ((int32_t) digT1)) * ((rawValue >> 4) - ((int32_t) digT1))) >> 12) * ((int32_t) digT3)) >> 14;
    fineTemperature = var1 + var2;
    T = (fineTemperature * 5 + 128) >> 8;

    return T / 100.0;
}

/**
 * 湿度を読み出す。
 */
double BME280::humidity() {
    uint8_t buffer[2];
    uint32_t rawValue;

    readRegister(0xfd, buffer, 2);
    rawValue = (buffer[0] << 8) | buffer[1];

    int32_t v_x1_u32r;
    v_x1_u32r = (fineTemperature - ((int32_t) 76800));
    v_x1_u32r =
        (((((rawValue << 14) - (((int32_t) digH4) << 20) - (((int32_t) digH5) * v_x1_u32r)) +
        ((int32_t) 16384)) >> 15) * (((((((v_x1_u32r * ((int32_t) digH6)) >> 10) * (((v_x1_u32r *
        ((int32_t) digH3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) *
        ((int32_t) digH2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)digH1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return ((uint32_t) (v_x1_u32r >> 12)) / 1024.0;
}

/**
 * 気圧を読み出す。
 */
double BME280::pressure() {
    uint8_t buffer[3];
    uint32_t rawValue;

    readRegister(0xf7, buffer, 3);
    rawValue = (buffer[0] << 12) | (buffer[1] << 4) | (buffer[2] >> 4);

    int64_t var1, var2, p;
    var1 = ((int64_t) fineTemperature) - 128000;
    var2 = var1 * var1 * (int64_t) digP6;
    var2 = var2 + ((var1 * (int64_t) digP5) << 17);
    var2 = var2 + (((int64_t) digP4) << 35);
    var1 = ((var1 * var1 * (int64_t) digP3) >> 8) + ((var1 * (int64_t) digP2) << 12);
    var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) digP1) >> 33;
    if (var1 == 0) return 0.0; // avoid exception caused by division by zero

    p = 1048576 - rawValue;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t) digP9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t) digP8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t) digP7) << 4);

    return p / 256.0;
}

void BME280::writeRegister(uint8_t address, uint8_t value) {
    uint8_t command[] = { (uint8_t) (address & 0x7f), value };

    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, BME280_SPI_FREQ);

    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    spi_send_data_standard(spiNumber, SPI_CHIP_SELECT_0, nullptr, 0, command, 2);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}

void BME280::readRegister(uint8_t address, uint8_t *buffer, size_t size) {
    uint8_t command = address | 0x80;

    spi_init(spiNumber, SPI_WORK_MODE_0, SPI_FF_STANDARD, 8, 0);
    spi_set_clk_rate(spiNumber, BME280_SPI_FREQ);

    gpiohs_set_pin(pinChipSelect, GPIO_PV_LOW);
    spi_send_data_standard(spiNumber, SPI_CHIP_SELECT_0, nullptr, 0, &command, 1);
    spi_receive_data_standard(spiNumber, SPI_CHIP_SELECT_0, nullptr, 0, buffer, size);
    gpiohs_set_pin(pinChipSelect, GPIO_PV_HIGH);
}

void BME280::readDig() {
    uint8_t data[32];
    readRegister(0x88, data, 24);
    readRegister(0xa1, data + 24, 1);
    readRegister(0xe1, data + 25, 7);

    // https://github.com/TakehikoShimojima/BME280_SPI
    digT1 = (data[1] << 8) | data[0];
    digT2 = (data[3] << 8) | data[2];
    digT3 = (data[5] << 8) | data[4];
    digP1 = (data[7] << 8) | data[6];
    digP2 = (data[9] << 8) | data[8];
    digP3 = (data[11] << 8) | data[10];
    digP4 = (data[13] << 8) | data[12];
    digP5 = (data[15] << 8) | data[14];
    digP6 = (data[17] << 8) | data[16];
    digP7 = (data[19] << 8) | data[18];
    digP8 = (data[21] << 8) | data[20];
    digP9 = (data[23] << 8) | data[22];
    digH1 = data[24];
    digH2 = (data[26] << 8) | data[25];
    digH3 = data[27];
    digH4 = (data[28] << 4) | (data[29] & 0x0f);
    digH5 = (data[30] << 4) | ((data[29] >> 4) & 0x0f);
    digH6 = data[31];
}
