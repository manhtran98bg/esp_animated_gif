#include "esp_animated_gif.h"
#include "gif.inl"
#include "esp_log.h"

static const char *TAG = "esp_gif";
static GIFIMAGE s_gif;
static int maxW, maxH;
static uint16_t usTemp[2][BUFFER_SIZE];
static bool dmaBuf = 0;

static uint16_t buffer[BUFFER_SIZE * 10];

static esp_port_draw_callback *esp_draw = NULL;
static uint8_t startX = 0;
static uint8_t startY = 0;
static void flush_block(int x, int y, int w, int h)
{
    if (esp_draw)
        esp_draw(x, y, w, h, &buffer[0]);
}

static void gif_draw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette;
    int x, y, iWidth, iCount;

    // Display bounds chech and cropping
    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > maxW)
        iWidth = maxW - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    if (y >= maxH || pDraw->iX >= maxW || iWidth < 1)
        return;

    // Old image disposal
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        dmaBuf = !dmaBuf;
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        pEnd = s + iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < iWidth)
        {
            c = ucTransparent - 1;
            d = &usTemp[dmaBuf][0];
            while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE)
            {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                }
                else // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            } // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                // DMA would degrtade performance here due to short line segments
                if (esp_draw)
                    esp_draw(pDraw->iX + x, y, iCount, 1, &usTemp[dmaBuf][0]);
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent)
                    x++;
                else
                    s--;
            }
        }
    }
    else
    {
        s = pDraw->pPixels;

        // Unroll the first pass to boost DMA performance
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        if (iWidth <= BUFFER_SIZE)
            for (iCount = 0; iCount < iWidth; iCount++)
                usTemp[dmaBuf][iCount] = usPalette[*s++];
        else
            for (iCount = 0; iCount < BUFFER_SIZE; iCount++)
                usTemp[dmaBuf][iCount] = usPalette[*s++];
        esp_draw(pDraw->iX, y, iCount, 1, &usTemp[dmaBuf][0]);
        dmaBuf = !dmaBuf;
        iWidth -= iCount;
        // Loop if pixel buffer smaller than width
        while (iWidth > 0)
        {
            // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
            if (iWidth <= BUFFER_SIZE)
                for (iCount = 0; iCount < iWidth; iCount++)
                    usTemp[dmaBuf][iCount] = usPalette[*s++];
            else
                for (iCount = 0; iCount < BUFFER_SIZE; iCount++)
                    usTemp[dmaBuf][iCount] = usPalette[*s++];
            esp_draw(pDraw->iX, y, iCount, 1, &usTemp[dmaBuf][0]);
            dmaBuf = !dmaBuf;
            iWidth -= iCount;
        }
    }
}

void esp_GIF_begin(unsigned char ucPaletteType, int maxWidth, int maxHeight, esp_port_draw_callback *pfnDraw)
{
    if (maxWidth <= 0 || maxWidth > 256)
        maxWidth = 240;
    if (maxHeight <= 0 || maxHeight > 256)
        maxHeight = 240;
    maxW = maxWidth;
    maxH = maxHeight;
    if (pfnDraw)
        esp_draw = pfnDraw;
    GIF_begin(&s_gif, ucPaletteType);
}

int esp_GIF_openFile(const char *path)
{
    return GIF_openFile(&s_gif, path, gif_draw);
}

int esp_GIF_openRAM(uint8_t *pData, int iDataSize)
{
    return GIF_openRAM(&s_gif, pData, iDataSize, gif_draw);
}

void esp_GIF_close()
{
    GIF_close(&s_gif);
}

void esp_GIF_reset()
{
    GIF_reset(&s_gif);
}

int esp_GIF_playFrame(int *delayMilliseconds, void *pUser)
{
    return GIF_playFrame(&s_gif, delayMilliseconds, pUser);
}