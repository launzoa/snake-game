# Snake Game — Reinforcement Learning (Q-Learning)

Uma versão do clássico jogo da cobrinha (**snake**), desenvolvido em **C++17** com a biblioteca gráfica **[Raylib](https://www.raylib.com/)** e portada para a Web via **WebAssembly (Emscripten)**.

O objetivo central do projeto é modelar o jogo como um **Processo de Decisão de Markov (MDP)** e comparar duas abordagens clássicas de Aprendizado por Reforço (*Reinforcement Learning*):
1. **Q-Learning Tabular (Seção 2.5.1 do livro de Kevin P. Murphy)**
2. **Q-Learning por Função de Aproximação Linear (Seção 2.5.2 do livro de Kevin P. Murphy)**

---

## Modelagem do Ambiente (MDP)

Para que os algoritmos de RL aprendam a jogar, o ambiente foi "desacoplado" da renderização gráfica através dos seguintes conceitos:

### Espaço de Ações ($\mathcal{A}$)
Em vez de simples direções (e.g., cima, baixo, esquerda, direita), o agente toma **ações relativas à sua cabeça**, i.e.,
- `0: STRAIGHT` (Seguir Reto)
- `1: TURN_LEFT` (Virar à Esquerda)
- `2: TURN_RIGHT` (Virar à Direita)

> Dessa forma reduz o espaço de ações para apenas 3 opções e elimina suicídios da cobra de $180^\circ$ (onde a cobra colide com o próprio corpo). Além disso, a política aprendida torna-se independente de rotação.

### Função de Recompensa ($\mathcal{R}$)
A modelagem das recompensas foi feita como:
- **Comer Maçã:** $+10.0$
- **Colisão (Morte na Parede ou Corpo):** $-10.0$
- **Passo Normal (Penalidade de Tempo):** $-0.1$ (força o agente a buscar o caminho mais curto e evita *loops* infinitos).
- **Vitória (Grid Completo):** $+100.0$

---

##  Algoritmos Implementados

### Q-Learning Tabular
- **Espaço de Estados:** Vetor binário (*bitmask*) de 9 bits ($2^9 = 512$ estados):
  - 3 bits para perigos (à frente, à esquerda, à direita).
  - 4 bits para posição da comida no grid (acima, abaixo, esquerda, direita).
  - 2 bits para a orientação atual da cabeça.
- **Tabela Q:** Matriz/Tablea de $512 \times 3$.
- **Regra de Atualização (Equação 2.33 de Murphy):**
  $$Q(s, a) \leftarrow Q(s, a) + \eta \left[ r + (1 - \text{done}) \gamma \max_{a'} Q(s', a') - Q(s, a) \right]$$

### Q-Learning por Função de Aproximação Linear
- **Espaço de Estados:** Vetor contínuo de 7 características (*features*) relativas normalizadas entre $0.0$ e $1.0$:
  - 3 perigos (`dangerStraight`, `dangerLeft`, `dangerRight`).
  - 4 direções da comida em relação à visão da cabeça (`foodAhead`, `foodBehind`, `foodLeft`, `foodRight`).
- **Parâmetros:** Matriz de pesos $W_{3 \times 7}$ e vetor de viés $b_3$ (apenas 24 parâmetros no total).
- **Predição:** $Q(s, a) = b_a + \sum_{f=1}^{7} W_{a, f} \cdot s_f$
- **Gradiente Descendente (SGD):**
  $$W_{a, f} \leftarrow W_{a, f} + \eta \cdot \delta \cdot s_f$$
  $$b_a \leftarrow b_a + \eta \cdot \delta$$

---

###️ Análise e Escolha dos Hiperparâmetros

Os hiperparâmetros foram calibrados com base na no livro do *Murphy*:

| Hiperparâmetro | Tabular | Linear | Motivação & Comparação |
| :--- | :---: | :---: | :--- |
| **Taxa de Aprendizado ($\eta$)** | `0.1` | `0.01` | No modo tabular, cada par $(s, a)$ é independente dos outros, logo um $\eta = 0.1$ (baixo) é suficiente. Para valores maiores de $\eta$ ($\eta = 0.5$ e $\eta = 0.9$), . Na aproximação, os pesos são compartilhados por todo o espaço de estados, logo valores maiores tendem a gerar esquecimento do gradiente. |
| **Fator de Desconto ($\gamma$)** | `0.9` | `0.9` | Garante um planejamento de ~10 passos ($\frac{1}{1-\gamma}$). Se $\gamma=0$, a cobra se torna míope, comendo por acaso (isso foi testado); se $\gamma \ge 0.99$ o agente foca muito na comida, morrendo de colisão por não estruturar bem as rotas de caminho após as comidas (rotas de fuga). |
| **Exploração Inicial ($\epsilon_0$)** | `1.0` | `1.0` | Início 100% aleatório para que o agente explore amplamente o mapa e descubra o padrão de recompensas e mortes. |
| **Decaimento do Epsilon** | `0.995` | `0.995` | Reduz progressivamente a aleatoriedade ao longo de ~600 episódios, garantindo que o agente comece a explorar menos e explorar mais a política ótima aprendida. |
| **Epsilon Mínimo ($\epsilon_{\min}$)** | `0.01` | `0.01` | Mantém 1% de exploração (aleatoriedade) para as ações tomadas. |

---

## Como Compilar e Executar

### Pré-requisitos
- Compilador C++17 (GCC, Clang ou MSVC)
- CMake (>= 3.16)

---

### 1. Build Nativo Desktop (Linux)
Requer a biblioteca `raylib` instalada no sistema.

```bash
# Configura o diretório de build
cmake -B build

# Compila o executável
cmake --build build

# Executa o jogo
./build/app
```

---

### 2. Build WebAssembly (Navegador)
Requer o [Emscripten SDK (emsdk)](https://emscripten.org/) instalado e ativado no terminal.

```bash
# 1. Configura a toolchain do Emscripten (utilizando o Raylib embutido)
emcmake cmake -B build-web -DPLATFORM=Web

# 2. Compila para WebAssembly (.wasm, .js, .html)
cmake --build build-web

# 3. Inicia um servidor HTTP local
python3 -m http.server 8000 --directory build-web
```
Abra o navegador em `http://localhost:8000/index.html`.

---


Desenvolvido por **[launzoa](https://github.com/launzoa)**.  
Fundamentado em *Probabilistic Machine Learning / Reinforcement Learning* (Kevin P. Murphy).
