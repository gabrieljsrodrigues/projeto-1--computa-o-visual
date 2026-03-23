# Projeto 1: Processamento Digital de Imagens (PDI) 
### Disciplina: Computação Visual 

## Integrantes do Grupo
* **Giulia Araki** - RA: 10408954
* **Felipe Carvalho** - RA: 10409804
* **Gabriel Rodrigues** - RA: 10409071

Este projeto consiste em um processador de imagens desenvolvido em linguagem C utilizando a biblioteca **SDL3**. O software é capaz de carregar imagens, convertê-las para escala de cinza, analisar histogramas (brilho e contraste) e realizar a equalização de histograma para melhoria visual.

---

## Demonstração em Vídeo
Como o projeto foi desenvolvido em ambiente **macOS**, decidimos disponibilizar uma demonstração completa para garantir a visualização dos requisitos em funcionamento, caso o professor não utilize esse processador:

 **[ASSISTIR VÍDEO DE DEMONSTRAÇÃO NO YOUTUBE](https://youtu.be/ysX5F_Pxfzc)**

---

## Integrantes do Grupo
* **Giulia Araki** - RA: 10408954
* **Felipe Carvalho** - RA: 10409804
* **Gabriel Rodrigues** - RA: 10409071

---

## Funcionalidades Realizadas
- [x] **Carregamento Multiformato:** Suporte a PNG, JPG e BMP via `SDL_image`.
- [x] **Tratamento de Erros:** Verificação de integridade de arquivos e caminhos.
- [x] **Conversão Gray:** Transformação de cores usando a fórmula de luminância:  
  $$Y = 0.2125 \cdot R + 0.7154 \cdot G + 0.0721 \cdot B$$
- [x] **Análise de Histograma:** Cálculo em tempo real de Média (Brilho) e Desvio Padrão (Contraste).
- [x] **Classificação Automática:** Textos dinâmicos classificando a imagem (Ex: "Imagem Escura", "Contraste Baixo").
- [x] **Equalização de Histograma:** Algoritmo baseado na Função de Distribuição Acumulada (CDF).
- [x] **Interface Interativa:** Botão dinâmico com estados de cor (Hover/Click) e alternância entre original/equalizado.
- [x] **Exportação:** Tecla `S` para salvar o resultado final como `output_image.png`.

---

## Como Compilar e Executar (macOS)

### Pré-requisitos
Certifique-se de ter o **Homebrew** instalado e as bibliotecas necessárias:
```bash
brew install sdl3 sdl3_image sdl3_ttf

---

## Provas de Implementação

Esta seção detalha exatamente onde e como cada requisito obrigatório do projeto foi cumprido no código-fonte `main.c`.

### 1. Requisitos Técnicos e Ambiente
- **Linguagem (C99+):** O código utiliza o padrão moderno da linguagem C, permitindo declarações de variáveis dentro do escopo de loops `for`.
  - *Evidência (Linha 28):* `for (int y = 0; y < temp->h; y++)`
- **Bibliotecas Obrigatórias:** Uso de `SDL3`, `SDL3_image` e `SDL3_ttf`.
  - *Evidência (Linhas 8-10):* Inclusões de cabeçalho `#include <SDL3/SDL.h>`, etc.

### 2. Carregamento e Tratamento de Erros
- **Formatos Comuns (PNG, JPG, BMP):** Implementado via `SDL_image`.
  - *Evidência (Linha 127):* Uso da função `IMG_Load(argv[1])`.
- **Tratamento de Arquivo Não Encontrado:** O programa verifica a integridade do ponteiro da imagem e encerra de forma segura caso falte o arquivo.
  - *Evidência (Linhas 128-130):* ```c
    if (!image) { printf("Erro na imagem!\n"); return 1; }
    ```

### 3. Análise e Conversão para Escala de Cinza
- **Fórmula de Luminância ($Y$):** Implementação exata dos pesos solicitados pelo professor.
  - *Evidência (Linha 34):* `Uint8 gray = (Uint8)(0.2125 * r + 0.7154 * g + 0.0721 * b);`
- **Base para Operações:** A imagem convertida (`gray`) é a entrada obrigatória para o histograma e a equalização.

### 4. Análise e Exibição do Histograma
- **Exibição Gráfica:** Desenho proporcional das barras brancas sobre fundo preto.
  - *Evidência:* Função `drawHistogram` (Linhas 87-93).
- **Análise Estatística:** Cálculo matemático de Média (Brilho) e Desvio Padrão (Contraste).
  - *Evidência:* Função `computeHistogram` (Linhas 42-53).
- **Classificação Automática:** Legendagem dinâmica em amarelo usando a biblioteca `SDL_ttf`.
  - *Evidência (Linhas 104-105):* Classificações entre "clara/média/escura" e "alto/médio/baixo contraste".

### 5. Equalização do Histograma (Interatividade)
- **Botão com Primitivas SDL:** Botão desenhado e renderizado nativamente.
  - *Evidência:* Função `renderButton` (Linhas 95-101) via `SDL_RenderFillRect`.
- **Estados de Cor do Botão:** Feedback visual completo para o usuário.
  - *Azul:* Neutro | *Azul Claro:* Hover | *Azul Escuro:* Clicado.
- **Reversão para Original (Toggle):** O sistema alterna texturas e labels entre "Equalizar" e "Ver Original".
  - *Evidência (Linhas 147-152):* Uso da flag `isEqualized` para alternar o estado do sistema.
