#include "TextRenderer.h"

TextRenderer::TextRenderer(uint8_t fw, uint8_t fh) {
    fontWidth = fw;
    fontHeight = fh;
}

TextRenderer::~TextRenderer() {
    if (fontImage) free(fontImage);
    if (glyphs) free(glyphs);
}

int TextRenderer::loadFont(const char *imageFile, const char *tableFile) {
    // PNG 画像データを読み込む
    FIL image;
    if (f_open(&image, imageFile, FA_READ) != FR_OK) {
        printf("Failed to open '%s'!\n", imageFile);
        return 1;
    }
    size_t imageSize = f_size(&image);
    if (imageSize > 256 * 1024) {
        printf("Too large font image file!\n");
        return 1;
    }
    UINT readSize;
    void *fontFileData = malloc(imageSize);
    if (f_read(&image, fontImage, imageSize, &readSize) != FR_OK) {
        printf("Failed to read font image file!\n");
        return 1;
    }
    f_close(&image);

    // PNG デコード
    unsigned char *decoded;
    unsigned int w, h;
    if (lodepng_decode_memory(
        &decoded, &w, &h,
        (unsigned char *) fontFileData, imageSize,
        LodePNGColorType::LCT_GREY, 8) != 0
    ) {
        printf("Failed to decode PNG image!\n");
        return 1;
    }
    fontImage = (uint8_t *) decoded;
    fontImageWidth = w;
    fontImageHeight = h;

    // 配置テーブル
    FIL table;
    if (f_open(&table, tableFile, FA_READ) != FR_OK) {
        printf("Failed to open '%s'!\n", tableFile);
        return 1;
    }
    if (f_read(&table, &glyphCount, sizeof(uint32_t), &readSize) != FR_OK) {
        printf("Invalid font table file!\n");
        return 1;
    }
    glyphs = (GlyphInfo *) malloc(sizeof(GlyphInfo) * glyphCount);
    if (f_read(&table, &glyphs, sizeof(GlyphInfo) * glyphCount, &readSize) != FR_OK) {
        printf("Invalid font table file!\n");
        return 1;
    }

    return 0;
}

void TextRenderer::render(const char *text, uint32_t x, uint32_t y, uint16_t color, uint8_t *buffer, uint32_t bufferWidth, uint32_t bufferHeight) {
    size_t length = strlen(text);
    // TODO: Unicode 対応する
    uint32_t cx = x;
    uint32_t cy = y;
    for (int i = 0; i < length; ++i) {
        uint32_t cp = text[i];
        if (cp == '\n') {
            cx = x;
            cy += fontHeight;
            continue;
        }

        uint8_t gx, gy;
        if (!searchGlyph(cp, &gx, &gy)) {
            continue;
        }

        for (int h = 0; h < fontHeight && cy + h < bufferHeight; ++h) {
            for (int w = 0; w < fontWidth && cx + w < bufferWidth; ++w) {
                uint32_t px = gx * fontWidth + w, py = gy * fontHeight + h;
                uint32_t tx = cx + w, ty = cy + h;
                uint8_t orig = fontImage[fontImageWidth * py + px];
                if (orig == 0) continue;
                buffer[(ty * bufferWidth + tx) * 2] = color >> 8;
                buffer[(ty * bufferWidth + tx) * 2  + 1] = color & 0xff;
            }
        }
        cx += fontWidth;
    }
}

bool TextRenderer::searchGlyph(uint32_t codepoint, uint8_t *x, uint8_t *y) {
    uint32_t chain = codepoint % FONT_HASH_SIZE;
    GlyphInfo *target = &glyphs[chain];
    bool found = false;

    while (target->nextChain != 0xffff) {
        if (target->codepoint == codepoint) {
            *x = target->x;
            *y = target->y;
            found = true;
            break;
        }
        target = &glyphs[target->nextChain];
    }

    return found;
}
