#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ff.h>
#include <lodepng.h>

#define FONT_HASH_SIZE 1024

struct GlyphInfo {
    uint32_t codepoint;
    uint8_t x;
    uint8_t y;
    uint16_t nextChain;
};

class TextRenderer {
private:
    uint32_t fontImageWidth = 0;
    uint32_t fontImageHeight = 0;
    uint8_t fontWidth = 0;
    uint8_t fontHeight = 0;
    uint8_t *fontImage = nullptr;
    uint32_t glyphCount = 0;
    GlyphInfo *glyphs = nullptr;

    bool searchGlyph(uint32_t codepoint, uint8_t *x, uint8_t *y);

public:
    TextRenderer(uint8_t fw, uint8_t fh);
    ~TextRenderer();
    int loadFont(const char *imageFile, const char *tableFile);
    void render(const char *text, uint32_t x, uint32_t y, uint16_t color, uint8_t *buffer, uint32_t bufferWidth, uint32_t bufferHeight);
};
