#pragma once

#define _GNU_SOURCE
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fpioa.h>
#include <gpiohs.h>
#include <spi.h>

#include "Maixduino.h"
#include <SSD1331.h>
#include <BME280.h>
#include <TextRenderer.h>
#include <ff.h>
#include <sdcard.h>
#include <tjpgd.h>


typedef struct {
    FIL *file;
    uint8_t *buffer;
    size_t bufferWidth;
    size_t bufferHeight;
} JpegIOData;

int main();
void initializeFPIOA();
void initializeGPIOHS();
int initializeFatFs();
int initializeFont();
int showImage();
int showAtmosphere();
uint16_t jpegio_input(JDEC *jdec, uint8_t *buff, uint16_t ndata);
uint16_t jpegio_output(JDEC *jdec, void *bitmap, JRECT *rect);
