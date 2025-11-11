#pragma once
#include <stdint.h>
#include "../src/AnimatedGIF/src/AnimatedGIF.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_SIZE 256

typedef void (esp_port_draw_callback)(int x, int y, int w, int h, const void *data);

void esp_GIF_begin(unsigned char ucPaletteType, int maxWidth, int maxHeight, esp_port_draw_callback *pfnDraw);

int esp_GIF_openFile(const char *path);

int esp_GIF_openRAM(uint8_t *pData, int iDataSize);

void esp_GIF_close();

void esp_GIF_reset();

int esp_GIF_playFrame(int *delayMilliseconds, void *pUser);

#ifdef __cplusplus
}
#endif
