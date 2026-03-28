# Projeto 1: Processamento Digital de Imagens (PDI) 
### Disciplina: Computação Visual 

## Integrantes do Grupo
* **Felipe Carvalho** - RA: 10409804
* **Gabriel Rodrigues** - RA: 10409071
* **Giulia Araki** - RA: 10408954

Este projeto consiste em um processador de imagens desenvolvido em linguagem C utilizando a biblioteca **SDL3**. O software é capaz de carregar imagens, convertê-las para escala de cinza, analisar histogramas (brilho e contraste) e realizar a equalização de histograma para melhoria visual.

---

## Demonstração em Vídeo
Como o projeto foi desenvolvido em ambiente **macOS**, decidimos disponibilizar uma demonstração completa para garantir a visualização dos requisitos em funcionamento, caso o professor não utilize esse processador:

 **[ASSISTIR VÍDEO DE DEMONSTRAÇÃO NO YOUTUBE](https://youtu.be/ysX5F_Pxfzc)**

---

## Contribuição do grupo
* ***Giulia Araki:*** Configuração do ambiente macOS, interface gráfica (botões/janelas), lógica de eventos e integração final.
* ***Felipe Carvalho:*** Funções de conversão para escala de cinza e cálculo matemático do histograma original.
* ***Gabriel Rodrigues:*** Implementação do algoritmo de equalização (CDF) e função de salvamento de arquivos.

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

## Requisitos Técnicos e Compilação

* **Linguagem:** C (Padrão C99+).
* **Compilador:** Compatível com `gcc` 15.1.0.
* **Qualidade:** Código organizado em estruturas e funções, com gerenciamento rigoroso de memória (`SDL_Destroy`).

## Como Compilar e Executar (macOS)
### Compilação
gcc main.c -o programa -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL3 -lSDL3_image -lSDL3_ttf

### Execução (Exemplo)
./programa test.jpg

### Pré-requisitos
Certifique-se de ter o **Homebrew** instalado e as bibliotecas necessárias:
brew install sdl3 sdl3_image sdl3_ttf

### Demonstração da execução em ambiente macOS: compilação via GCC, interface de janelas coordenadas e análise de histograma em tempo real:
<img width="1047" height="895" alt="Captura de Tela 2026-03-28 às 13 30 16" src="https://github.com/user-attachments/assets/10a21658-b96c-4485-9b1f-4592f1bffe5b" />


---

Abaixo, detalhamos como cada exigência técnica foi implementada no código-fonte:

### 1. Carregamento de Imagem e Tratamento de Erros
* **Formatos Suportados:** PNG, JPG e BMP via `SDL_image`.
* **Segurança:** O programa valida o ponteiro da imagem e interrompe a execução com mensagem de erro caso o arquivo seja inválido ou inexistente.
<img width="371" height="18" alt="Captura de Tela 2026-03-23 às 17 22 38" src="https://github.com/user-attachments/assets/98594e11-bf79-4783-899f-8821637b2153" />


### 2. Escala de Cinza (Conversão e Base)
* **Fórmula de Luminância:** Implementação rigorosa dos pesos $Y = 0.2125 \cdot R + 0.7154 \cdot G + 0.0721 \cdot B$.
* **Processamento:** A imagem em escala de cinza (`gray`) é a entrada mandatória para o histograma e a equalização.
<img width="521" height="73" alt="Captura de Tela 2026-03-23 às 19 54 43" src="https://github.com/user-attachments/assets/8f8cc139-719c-4575-bb47-d0f3906d704e" />


### 3. Interface Gráfica (GUI) com Duas Janelas
* **Janela Principal:** Exibe a imagem processada, adapta-se ao tamanho do arquivo e inicia centralizada.
* **Janela Secundária:** Janela "filha" de tamanho fixo posicionada lateralmente, contendo o histograma e os controles.
<img width="593" height="150" alt="Captura de Tela 2026-03-23 às 19 57 22" src="https://github.com/user-attachments/assets/d44ae6a4-5d5e-4be6-a6f7-cbe9bd38fd72" />


### 4. Análise e Exibição do Histograma
* **Gráfico:** Exibição proporcional das intensidades de cinza.
* **Estatísticas:** Cálculo de Média (Brilho) e Desvio Padrão (Contraste) com classificação automática.
* **Texto:** Uso da biblioteca `SDL_ttf` para renderizar as informações dinâmicas em amarelo.
<img width="586" height="72" alt="Captura de Tela 2026-03-23 às 19 58 58" src="https://github.com/user-attachments/assets/59bb62c0-d3d3-4192-8c26-30e9fdc01c3d" />


### 5. Equalização do Histograma (Interatividade)
* **Botão Primitivo:** Desenhado nativamente com `SDL_RenderFillRect`.
* **Feedback Visual:** Cores Azul (Neutro), Azul Claro (Hover) e Azul Escuro (Click).
* **Toggle de Estado:** O botão alterna o processamento e o texto entre "Equalizar" e "Ver Original".
<img width="572" height="106" alt="Captura de Tela 2026-03-23 às 20 00 18" src="https://github.com/user-attachments/assets/7150c629-9277-427d-82f6-ec9aafa4e140" />


### 6. Salvamento de Imagem
* **Tecla 'S':** Salva o estado atual da janela principal (original cinza ou equalizada) como `output_image.png`, sobrescrevendo arquivos existentes.
<img width="499" height="43" alt="Captura de Tela 2026-03-23 às 20 01 07" src="https://github.com/user-attachments/assets/a2d80176-3e8f-4164-87e9-78179258b3e3" />

### Resultado do Processamento: Abaixo, a evidência da geração do arquivo output_image.png na pasta do projeto após pressionar a tecla 'S':
<img width="474" height="431" alt="Captura de Tela 2026-03-28 às 13 19 20" src="https://github.com/user-attachments/assets/475800bb-c0ff-4d71-9c7a-ed665726c13e" />
