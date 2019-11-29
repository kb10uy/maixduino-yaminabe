#include "ILI9341.h"

ILI9341::ILI9341(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs) : LCDBase(spi, ILI9341_SPI_FREQUENCY, rst, dc, cs) {
}

void ILI9341::initialize() {

}
