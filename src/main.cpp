#include "main.h"

void *__dso_handle=0;

uint8_t buffer[SSD1331_WIDTH * SSD1331_HEIGHT * 2] = {0};
SSD1331 oled(SPI_DEVICE_1, 13, 12, 11);
TextRenderer font(8, 12);
FATFS fs;

int main() {
    initializeFPIOA();
    initializeGPIOHS();
    printf("GPIOHS OK\n");

    oled.initialize();
    printf("OLED OK\n");

    initializeFatFs();
    initializeFont();
    printf("Filesystem OK\n");

    while (true) showImage();

    return 0;
}

void initializeFPIOA() {
    // SD
    fpioa_set_function(MAIXDUINO_SD_SCLK_PIN, FUNC_SPI0_SCLK);
    fpioa_set_function(MAIXDUINO_SD_MOSI_PIN, FUNC_SPI0_D0);
    fpioa_set_function(MAIXDUINO_SD_MISO_PIN, FUNC_SPI0_D1);
	fpioa_set_function(MAIXDUINO_SD_CS_PIN, FUNC_GPIOHS7);

    // SSD1331
    fpioa_set_function(MAIXDUINO_D11_PIN, FUNC_GPIOHS11);
    fpioa_set_function(MAIXDUINO_D12_PIN, FUNC_GPIOHS12);
    fpioa_set_function(MAIXDUINO_D13_PIN, FUNC_GPIOHS13);
    fpioa_set_function(MAIXDUINO_SDA_PIN, FUNC_SPI1_D0);
    fpioa_set_function(MAIXDUINO_SCL_PIN, FUNC_SPI1_SCLK);
}

void initializeGPIOHS() {
    gpiohs_set_drive_mode(11, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(12, GPIO_DM_OUTPUT);
    gpiohs_set_drive_mode(13, GPIO_DM_OUTPUT);
}

int initializeFatFs() {
    FRESULT status;

    status = f_mount(&fs, "", 1);
    if (status != FR_OK) {
        printf("Failed to mount the filesystem!\n");
        return status;
    }
    printf("Mounted the filesystem.\n");
    return FR_OK;
}

int initializeFont() {
    if (font.loadFont("k8x12.png", "sjtable.dat") != 0) {
        printf("Failed to load font!\n");
        return 1;
    }

    return 0;
}

int showImage() {
    JDEC decoder;
    JpegIOData data;
    uint8_t work[3100];
    data.buffer = buffer;
    data.bufferWidth = SSD1331_WIDTH;
    data.bufferHeight = SSD1331_HEIGHT;

    FRESULT status;
    DIR root;
    FILINFO fileInfo;
    FIL file;

    status = f_findfirst(&root, &fileInfo, "", "*.jpg");

    while (status == FR_OK && fileInfo.fname[0]) {
        if (f_open(&file, fileInfo.fname, FA_READ) != FR_OK) {
            printf("Failed to open 'ongeki.jpg'!\n");
            break;
        }

        data.file = &file;
        if (jd_prepare(&decoder, jpegio_input, work, 3100, &data) != JDR_OK) {
            printf("Failed to initialize a decoder!\n");
            break;
        }

        memset(buffer, 0, SSD1331_WIDTH * SSD1331_HEIGHT * 2);
        jd_decomp(&decoder, jpegio_output, 0);
        font.render(fileInfo.fname, 0, 0, COLOR888TO565(0, 255, 255), buffer, SSD1331_WIDTH, SSD1331_HEIGHT);
        oled.setRange(0, 0, 96, 64);
        oled.sendData(buffer, SSD1331_WIDTH * SSD1331_HEIGHT * 2);

        f_close(&file);
        usleep(1000000);
        status = f_findnext(&root, &fileInfo);
    }

    return status;
}

uint16_t jpegio_input(JDEC *jdec, uint8_t *buff, uint16_t ndata) {
    FRESULT status;
    JpegIOData *data = (JpegIOData *) jdec->device;

    if (buff) {
        UINT read;
        status = f_read(data->file, buff, ndata, &read);
        return status == FR_OK ? (uint16_t) read : 0;
    } else {
        status = f_lseek(data->file, f_tell(data->file) + ndata);
        return status == FR_OK ? ndata : 0;
    }
}

uint16_t jpegio_output(JDEC *jdec, void *bitmap, JRECT *rect) {
    JpegIOData *data = (JpegIOData *) jdec->device;
    WORD *source = (WORD *) bitmap;

    if (rect->left >= data->bufferWidth) {
        return 1;
    }

    int sourceWidth = rect->right - rect->left + 1;
    int copyWidth = sourceWidth;
    if (copyWidth > data->bufferWidth - rect->left) {
        copyWidth = data->bufferWidth - rect->left;
    }
    for (int sy = rect->top; sy <= rect->bottom && sy < data->bufferHeight; ++sy) {
        for (int sx = rect->left; sx <= rect->right && sx < data->bufferWidth; ++sx) {
            WORD color = source[(sy - rect->top) * sourceWidth + (sx - rect->left)];
            data->buffer[(sy * data->bufferWidth + sx) * 2] = color >> 8;
            data->buffer[(sy * data->bufferWidth + sx) * 2 + 1] = color & 0xff;
        }
    }

    return 1;
}
