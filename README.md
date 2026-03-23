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
