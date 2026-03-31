/* * Projeto 1 (Proj1) - Processamento de Imagens
 * Disciplina: Computação Visual - Turma 07C
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

// Converte a imagem para escala de cinza usando pesos perceptuais
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
            // Y = 0.2126R + 0.7152G + 0.0722B (Padrão BT.709)
            Uint8 gray = (Uint8)(0.2126f * r + 0.7152f * g + 0.0722f * b);
            pixels[y * pitch + x] = SDL_MapRGBA(fmt, NULL, gray, gray, gray, a);
        }
    }
    SDL_UnlockSurface(temp);
    SDL_BlitSurface(temp, NULL, surface, NULL);
    SDL_DestroySurface(temp);
}

// Calcula Histograma e CDF (Função de Distribuição Acumulada)
void computeStats(SDL_Surface *surface, int hist[256], float cdf[256], double *media, double *desvio) {
    for (int i = 0; i < 256; i++) hist[i] = 0;
    
    SDL_Surface *temp = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    Uint32 *pixels = (Uint32 *)temp->pixels;
    int total = temp->w * temp->h;
    int pitch = temp->pitch / 4;
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(temp->format);

    for (int y = 0; y < temp->h; y++) {
        for (int x = 0; x < temp->w; x++) {
            Uint8 r, g, b; 
            SDL_GetRGB(pixels[y * pitch + x], fmt, NULL, &r, &g, &b);
            hist[r]++;
        }
    }

    // Cálculo da CDF Normalizada (0.0 a 1.0)
    int acc = 0;
    for (int i = 0; i < 256; i++) {
        acc += hist[i];
        cdf[i] = (float)acc / total;
    }

    double soma = 0; for (int i = 0; i < 256; i++) soma += i * hist[i];
    *media = soma / total;
    double soma2 = 0; for (int i = 0; i < 256; i++) soma2 += (i - *media) * (i - *media) * hist[i];
    *desvio = sqrt(soma2 / total);
    
    SDL_DestroySurface(temp);
}

// Equalização baseada na CDF
SDL_Surface* equalizeHistogram(SDL_Surface *src, float cdf[256]) {
    SDL_Surface *dst = SDL_ConvertSurface(src, SDL_PIXELFORMAT_RGBA32);
    Uint32 *pixOut = (Uint32 *)dst->pixels;
    int pitch = dst->pitch / 4;
    const SDL_PixelFormatDetails *fmt = SDL_GetPixelFormatDetails(dst->format);

    float cdf_min = 0;
    for(int i=0; i<256; i++) if(cdf[i] > 0) { cdf_min = cdf[i]; break; }

    for (int y = 0; y < dst->h; y++) {
        for (int x = 0; x < dst->w; x++) {
            Uint8 r, g, b, a; 
            SDL_GetRGBA(pixOut[y * pitch + x], fmt, NULL, &r, &g, &b, &a);
            
            // Aplicação da fórmula de equalização: h(v) = round((cdf(v) - cdf_min) / (1 - cdf_min) * 255)
            Uint8 newVal = (Uint8)(SDL_clamp(((cdf[r] - cdf_min) / (1.0f - cdf_min)) * 255.0f, 0, 255));
            
            pixOut[y * pitch + x] = SDL_MapRGBA(fmt, NULL, newVal, newVal, newVal, a);
        }
    }
    return dst;
}

// Desenha Histograma + Curva da CDF
void drawHistogramUI(SDL_Renderer *ren, int hist[256], float cdf[256]) {
    SDL_SetRenderDrawColor(ren, 20, 20, 20, 255); 
    SDL_RenderClear(ren);

    int maxVal = 1; 
    for (int i = 0; i < 256; i++) if (hist[i] > maxVal) maxVal = hist[i];

    // Desenha Barras do Histograma
    for (int i = 0; i < 256; i++) {
        float x = (float)i * HIST_W / 256.0f;
        int h = (hist[i] * (HIST_H - 100)) / maxVal;
        SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
        SDL_RenderLine(ren, x, (float)HIST_H, x, (float)HIST_H - (float)h);
    }

    // Desenha Curva da CDF (Linha Amarela)
    SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);
    for (int i = 0; i < 255; i++) {
        float x1 = (float)i * HIST_W / 256.0f;
        float y1 = (float)HIST_H - (cdf[i] * (HIST_H - 100));
        float x2 = (float)(i + 1) * HIST_W / 256.0f;
        float y2 = (float)HIST_H - (cdf[i+1] * (HIST_H - 100));
        SDL_RenderLine(ren, x1, y1, x2, y2);
    }
}

void renderButton(SDL_Renderer *ren, Button *btn, TTF_Font *font) {
    if (btn->clicked) SDL_SetRenderDrawColor(ren, 0, 0, 139, 255);
    else if (btn->hovered) SDL_SetRenderDrawColor(ren, 70, 70, 255, 255);
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
    const char *lum = (media < 85) ? "Luminosidade: Baixa (Escura)" : (media < 170) ? "Luminosidade: Media" : "Luminosidade: Alta (Clara)";
    const char *con = (desvio < 50) ? "Contraste: Baixo" : (desvio < 100) ? "Contraste: Medio" : "Contraste: Alto";
    
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
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) return 1;

    SDL_Surface *image = IMG_Load(argv[1]);
    if (!image) { printf("Erro ao carregar imagem!\n"); return 1; }

    // Preparação das superfícies
    SDL_Surface *gray = SDL_ConvertSurface(image, SDL_PIXELFORMAT_RGBA32);
    convertToGrayscale(gray);
    
    int hist[256]; float cdf[256]; double media, desvio;
    computeStats(gray, hist, cdf, &media, &desvio);
    
    SDL_Surface *equalized = equalizeHistogram(gray, cdf);

    // Janelas e Renderers
    SDL_Window *win_main = SDL_CreateWindow("Visualizador Mackenzie - Proj1", gray->w, gray->h, 0);
    SDL_Window *win_hist = SDL_CreateWindow("Estatisticas e Histograma", HIST_W, HIST_H, 0);
    SDL_Renderer *ren_main = SDL_CreateRenderer(win_main, NULL);
    SDL_Renderer *ren_hist = SDL_CreateRenderer(win_hist, NULL);

    SDL_Texture *tex_main = SDL_CreateTextureFromSurface(ren_main, gray);
    TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 14);

    Button btn = {{(HIST_W - BTN_W)/2.0f, 10, BTN_W, BTN_H}, false, false, "Equalizar"};
    bool running = true, isEqualized = false; SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) running = false;
            
            if (e.type == SDL_EVENT_MOUSE_MOTION && e.window.windowID == SDL_GetWindowID(win_hist))
                btn.hovered = (e.motion.x >= btn.rect.x && e.motion.x <= btn.rect.x+btn.rect.w && e.motion.y >= btn.rect.y && e.motion.y <= btn.rect.y+btn.rect.h);
            
            if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && btn.hovered) btn.clicked = true;
            
            if (e.type == SDL_EVENT_MOUSE_BUTTON_UP && btn.clicked) {
                btn.clicked = false;
                isEqualized = !isEqualized;
                SDL_DestroyTexture(tex_main);
                
                if (isEqualized) {
                    tex_main = SDL_CreateTextureFromSurface(ren_main, equalized);
                    computeStats(equalized, hist, cdf, &media, &desvio);
                    btn.label = "Ver Original";
                } else {
                    tex_main = SDL_CreateTextureFromSurface(ren_main, gray);
                    computeStats(gray, hist, cdf, &media, &desvio);
                    btn.label = "Equalizar";
                }
            }
        }

        SDL_RenderClear(ren_main);
        SDL_RenderTexture(ren_main, tex_main, NULL, NULL);
        SDL_RenderPresent(ren_main);

        drawHistogramUI(ren_hist, hist, cdf);
        renderButton(ren_hist, &btn, font);
        renderClassification(ren_hist, font, media, desvio);
        SDL_RenderPresent(ren_hist);
    }

    // Cleanup
    SDL_DestroyTexture(tex_main);
    SDL_DestroyRenderer(ren_main); SDL_DestroyRenderer(ren_hist);
    SDL_DestroyWindow(win_main); SDL_DestroyWindow(win_hist);
    TTF_CloseFont(font);
    SDL_DestroySurface(image); SDL_DestroySurface(gray); SDL_DestroySurface(equalized);
    TTF_Quit(); SDL_Quit();
    return 0;
}