#include "ILI9341.h"

// https://github.com/adafruit/Adafruit_ILI9341
// BSD licensed

static uint8_t ILI9341_INITIALIZATION[] = {
    ILI9341_SWRESET, 0x80,
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
    ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
    ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
    ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
    ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
    ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
    ILI9341_PIXFMT  , 1, 0x55,
    ILI9341_FRMCTR1 , 2, 0x00, 0x18,
    ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
    0xF2, 1, 0x00,                         // 3Gamma Function Disable
    ILI9341_GAMMASET , 1, 0x01,            // Gamma curve selected
    ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    ILI9341_SLPOUT  , 0x80,                // Exit Sleep
    ILI9341_DISPON  , 0x80,                // Display on
    0x00                                   // End of list
};

ILI9341::ILI9341(spi_device_num_t spi, uint8_t rst, uint8_t dc, uint8_t cs) : LCDBase(spi, ILI9341_SPI_FREQUENCY, rst, dc, cs) {
}

void ILI9341::initialize() {
    sendReset();
    uint8_t command, length;
    const uint8_t *current = ILI9341_INITIALIZATION;
    while ((command = *current++) > 0) {
        length = (*current++) & 0x7f;
        sendCommandData(command, current, length);
        current += length;
        if (length == 0) usleep(150000);
    }
}

void ILI9341::setRange(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
    uint16_t left = x;
    uint16_t right = x + width - 1;
    uint16_t top = y;
    uint16_t bottom = y + height - 1;
    uint8_t data[4];

    data[0] = left >> 8;
    data[1] = left & 0xff;
    data[2] = right >> 8;
    data[3] = right & 0xff;
    sendCommandData(ILI9341_CASET, data, 4);

    data[0] = top >> 8;
    data[1] = top & 0xff;
    data[2] = bottom >> 8;
    data[3] = bottom & 0xff;
    sendCommandData(ILI9341_PASET, data, 4);

    data[0] = ILI9341_RAMWR;
    sendCommand(data, 1);
}

void ILI9341::setRotation(uint8_t rotation) {
    uint8_t target = 0;

    switch (rotation % 4) {
        case 0:
            target = 0b01001000;
            break;
        case 1:
            target = 0b00101000;
            break;
        case 2:
            target = 0b10001000;
            break;
        case 3:
            target = 0b11101000;
            break;
    }
    sendCommandData(ILI9341_MADCTL, &target, 1);
}

void ILI9341::fillBox(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    uint8_t buffer[256];
    uint32_t rest = w * h * 2;
    for (int i= 0; i < 128; ++i) {
        buffer[i * 2] = color >> 8;
        buffer[i * 2 + 1] = color & 0xff;
    }

    setRange(x, y, w, h);
    while (rest > 0) {
        size_t sending = rest < 256 ? rest : 256;
        sendData(buffer, sending);
        rest -= sending;
    }
}
