/* * Projeto 1 (Proj1) - Processamento de Imagens
 * Disciplina: Computação Visual
 * Integrantes: Felipe Carvalho RA: 10409804, Gabriel Rodrigues RA: 10409071, Giulia Araki RA: 10408954
 */

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define HIST_W 400
#define HIST_H 300
#define BTN_W 120
#define BTN_H 40

typedef struct { SDL_FRect rect; bool hovered; bool clicked; const char *label; } Button;

// Função de conversão ajustada para evitar listras no Mac (Item 2 do Proj1)
void convertToGrayscale(SDL_Surface *surface) {
    if (!surface) return;
    SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    if (!temp) return;

    SDL_LockSurface(temp);
    Uint32 *pixels = (Uint32 *)temp->pixels;
    int pitch = temp->pitch / 4;
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(temp->format);

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            Uint8 r, g, b, a;
            SDL_GetRGBA(pixels[y * pitch + x], fmt, NULL, &r, &g, &b, &a);
            // Fórmula oficial: Y = 0.2125*R + 0.7154*G + 0.0721*B
            Uint8 gray = (Uint8)(0.2125 * r + 0.7154 * g + 0.0721 * b);
            pixels[y * pitch + x] = SDL_MapRGBA(fmt, NULL, gray, gray, gray, a);
        }
    }
    SDL_UnlockSurface(temp);
    SDL_BlitSurface(temp, NULL, surface, NULL);
    SDL_DestroySurface(temp);
}

void computeHistogram(SDL_Surface *surface, int hist[256], double *media, double *desvio) {
    for (int i = 0; i < 256; i++) hist[i] = 0;
    SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    Uint32 *pixels = (Uint32 *)temp->pixels;
    int total = temp->w * temp->h;
    int pitch = temp->pitch / 4;
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(temp->format);

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            Uint8 r, g, b; SDL_GetRGB(pixels[y * pitch + x], fmt, NULL, &r, &g, &b);
            hist[r]++;
        }
    }
    double soma = 0; for (int i = 0; i < 256; i++) soma += i * hist[i];
    *media = soma / total;
    double soma2 = 0; for (int i = 0; i < 256; i++) soma2 += (i - *media) * (i - *media) * hist[i];
    *desvio = sqrt(soma2 / total);
    SDL_DestroySurface(temp);
}

SDL_Surface* equalizeHistogram(SDL_Surface *src) {
    int hist[256] = {0}, cdf[256];
    SDL_Surface *temp = SDL_ConvertSurface(src, SDL_PIXELFORMAT_RGBA32);
    Uint32 *pixels = (Uint32 *)temp->pixels;
    int total = temp->w * temp->h;
    int pitch = temp->pitch / 4;
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(temp->format);

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            Uint8 r, g, b; SDL_GetRGB(pixels[y * pitch + x], fmt, NULL, &r, &g, &b);
            hist[r]++;
        }
    }
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) cdf[i] = cdf[i - 1] + hist[i];
    
    SDL_Surface *dst = SDL_ConvertSurface(temp, temp->format);
    Uint32 *pixOut = (Uint32 *)dst->pixels;
    int cdf_min = 0;
    for (int i = 0; i < 256; i++) if (cdf[i] > 0) { cdf_min = cdf[i]; break; }

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            Uint8 r, g, b, a; SDL_GetRGBA(pixels[y * pitch + x], fmt, NULL, &r, &g, &b, &a);
            int newVal = round(((double)(cdf[r] - cdf_min) / (total - cdf_min)) * 255.0);
            if (newVal < 0) newVal = 0; if (newVal > 255) newVal = 255;
            pixOut[y * pitch + x] = SDL_MapRGBA(fmt, NULL, newVal, newVal, newVal, a);
        }
    }
    SDL_DestroySurface(temp);
    return dst;
}

void drawHistogram(SDL_Renderer *ren, int hist[256]) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); SDL_RenderClear(ren);
    int maxVal = 1; for (int i = 0; i < 256; i++) if (hist[i] > maxVal) maxVal = hist[i];
    for (int i = 0; i < 256; i++) {
        int h = (hist[i] * (HIST_H - 120)) / maxVal;
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderLine(ren, (float)i * HIST_W / 256, (float)HIST_H, (float)i * HIST_W / 256, (float)HIST_H - (float)h);
    }
}

void renderButton(SDL_Renderer *ren, Button *btn, TTF_Font *font) {
    if (btn->clicked) SDL_SetRenderDrawColor(ren, 0, 0, 139, 255);
    else if (btn->hovered) SDL_SetRenderDrawColor(ren, 173, 216, 230, 255);
    else SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
    SDL_RenderFillRect(ren, &btn->rect);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurf = TTF_RenderText_Blended(font, btn->label, 0, white);
    if (textSurf) {
        SDL_Texture *textTex = SDL_CreateTextureFromSurface(ren, textSurf);
        SDL_FRect dst = {btn->rect.x + (btn->rect.w - (float)textSurf->w)/2.0f, btn->rect.y + (btn->rect.h - (float)textSurf->h)/2.0f, (float)textSurf->w, (float)textSurf->h};
        SDL_RenderTexture(ren, textTex, NULL, &dst);
        SDL_DestroyTexture(textTex); SDL_DestroySurface(textSurf);
    }
}

void renderClassification(SDL_Renderer *ren, TTF_Font *font, double media, double desvio) {
    const char *lum = (media < 85) ? "Imagem escura" : (media < 170) ? "Imagem media" : "Imagem clara";
    const char *con = (desvio < 50) ? "Contraste baixo" : (desvio < 100) ? "Contraste medio" : "Contraste alto";
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Surface *s1 = TTF_RenderText_Blended(font, lum, 0, yellow);
    SDL_Texture *t1 = SDL_CreateTextureFromSurface(ren, s1);
    SDL_FRect d1 = {10, 60, (float)s1->w, (float)s1->h}; SDL_RenderTexture(ren, t1, NULL, &d1);
    SDL_Surface *s2 = TTF_RenderText_Blended(font, con, 0, yellow);
    SDL_Texture *t2 = SDL_CreateTextureFromSurface(ren, s2);
    SDL_FRect d2 = {10, 90, (float)s2->w, (float)s2->h}; SDL_RenderTexture(ren, t2, NULL, &d2);
    SDL_DestroyTexture(t1); SDL_DestroySurface(s1); SDL_DestroyTexture(t2); SDL_DestroySurface(s2);
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printf("Uso: %s imagem.jpg\n", argv[0]); return 1; }
    SDL_Init(SDL_INIT_VIDEO); TTF_Init();
    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) { printf("Erro na imagem!\n"); return 1; }
    SDL_Surface *gray = SDL_ConvertSurface(image, image->format);
    convertToGrayscale(gray); SDL_Surface *equalized = equalizeHistogram(gray);
    SDL_Window *win_main = SDL_CreateWindow("Visualizador Proj1", gray->w, gray->h, 0);
    SDL_SetWindowPosition(win_main, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_Window *win_hist = SDL_CreateWindow("Histograma", HIST_W, HIST_H, 0);
    SDL_SetWindowParent(win_hist, win_main);
    int mx, my, mw, mh; SDL_GetWindowPosition(win_main, &mx, &my); SDL_GetWindowSize(win_main, &mw, &mh);
    SDL_SetWindowPosition(win_hist, mx + mw + 10, my);
    SDL_Renderer *ren_main = SDL_CreateRenderer(win_main, NULL);
    SDL_Renderer *ren_hist = SDL_CreateRenderer(win_hist, NULL);
    SDL_Texture *tex_main = SDL_CreateTextureFromSurface(ren_main, gray);
    TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 16);
    int hist[256]; double media, desvio; computeHistogram(gray, hist, &media, &desvio);
    Button btn = {{(HIST_W - BTN_W)/2.0f, 10, BTN_W, BTN_H}, false, false, "Equalizar"};
    bool running = true, isEqualized = false; SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_MOUSE_MOTION && e.window.windowID == SDL_GetWindowID(win_hist))
                btn.hovered = (e.motion.x >= btn.rect.x && e.motion.x <= btn.rect.x+btn.rect.w && e.motion.y >= btn.rect.y && e.motion.y <= btn.rect.y+btn.rect.h);
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && btn.hovered) btn.clicked = true;
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && btn.clicked) {
                btn.clicked = false; isEqualized = !isEqualized; SDL_DestroyTexture(tex_main);
                if (isEqualized) { tex_main = SDL_CreateTextureFromSurface(ren_main, equalized); computeHistogram(equalized, hist, &media, &desvio); btn.label = "Ver original"; }
                else { tex_main = SDL_CreateTextureFromSurface(ren_main, gray); computeHistogram(gray, hist, &media, &desvio); btn.label = "Equalizar"; }
            }
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.scancode == SDL_SCANCODE_S) {
                IMG_SavePNG(isEqualized ? equalized : gray, "output_image.png");
                printf("Imagem salva como output_image.png\n");
            }
        }
        SDL_RenderClear(ren_main); SDL_RenderTexture(ren_main, tex_main, NULL, NULL); SDL_RenderPresent(ren_main);
        drawHistogram(ren_hist, hist); renderButton(ren_hist, &btn, font); renderClassification(ren_hist, font, media, desvio); SDL_RenderPresent(ren_hist);
    }
    SDL_DestroyTexture(tex_main); SDL_DestroyRenderer(ren_main); SDL_DestroyRenderer(ren_hist);
    SDL_DestroyWindow(win_main); SDL_DestroyWindow(win_hist); TTF_CloseFont(font);
    SDL_DestroySurface(image); SDL_DestroySurface(gray); SDL_DestroySurface(equalized);
    TTF_Quit(); SDL_Quit(); return 0;
}