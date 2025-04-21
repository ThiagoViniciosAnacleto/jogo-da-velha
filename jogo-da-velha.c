#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <locale.h>

#define MAX_NAME_LEN 50
#define FILENAME "jogo_estatisticas.txt"
#define MAX_PLAYERS 100
#define LOGIN_FILE "logins.txt"
#define PARTIDA_FILE "partida_em_andamento.txt"

typedef struct {
    char nome[MAX_NAME_LEN];
    int partidas;
    int vitorias;
    int empates;
    int derrotas;
} Jogador;

typedef struct {
    char tabuleiro[3][3];
    char jogadorAtual;
} Jogo;

void limparTabuleiro(char tabuleiro[3][3]);
void exibirTabuleiro(char tabuleiro[3][3]);
int verificarVitoria(char tabuleiro[3][3]);
int verificarEmpate(char tabuleiro[3][3]);
int realizarJogada(char tabuleiro[3][3], int linha, int coluna, char jogador);
int verificarPartidaNaoFinalizada(Jogo *jogo);
void salvarEstatisticas(Jogador *jogador);
void carregarEstatisticas(Jogador *jogador);
void salvarPartida(Jogo *jogo);
void carregarPartida(Jogo *jogo);
void realizarLogin(Jogador *jogador);
void registrarJogador(Jogador *jogador);
void mascararSenha(char *senha);
int jogarNovamente();

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Portuguese_Brasil");

    Jogador jogador = {"", 0, 0, 0, 0};
    Jogo jogo;
    int opcao, linha, coluna, continuar;

    do {
        printf("Bem-vindo ao Jogo da Velha!\n");
        printf("Deseja fazer login? (1 - Sim, 2 - Não): ");
        scanf("%d", &opcao);
        getchar(); // Limpa buffer

        if (opcao == 1) {
            realizarLogin(&jogador);
        } else {
            printf("Digite seu nome: ");
            fgets(jogador.nome, MAX_NAME_LEN, stdin);
            jogador.nome[strcspn(jogador.nome, "\n")] = '\0';
            registrarJogador(&jogador);
        }

        carregarEstatisticas(&jogador);

        if (verificarPartidaNaoFinalizada(&jogo)) {
            printf("Partida anterior não finalizada, deseja continuar? (1 - Sim, 2 - Não): ");
            scanf("%d", &continuar);
            if (continuar != 1) {
                limparTabuleiro(jogo.tabuleiro);
                jogo.jogadorAtual = 'X';
            }
        } else {
            limparTabuleiro(jogo.tabuleiro);
            jogo.jogadorAtual = 'X';
        }

        while (1) {
            exibirTabuleiro(jogo.tabuleiro);
            printf("Jogador %c, faça sua jogada (linha coluna): ", jogo.jogadorAtual);
            scanf("%d %d", &linha, &coluna);

            linha--;
            coluna--;

            if (linha < 0 || linha > 2 || coluna < 0 || coluna > 2) {
                printf("Coordenadas inválidas. Use números de 1 a 3.\n");
                continue;
            }

            if (realizarJogada(jogo.tabuleiro, linha, coluna, jogo.jogadorAtual)) {
                jogador.partidas++;
                if (verificarVitoria(jogo.tabuleiro)) {
                    printf("Jogador %c venceu!\n", jogo.jogadorAtual);
                    if (jogo.jogadorAtual == 'X') jogador.vitorias++;
                    else jogador.derrotas++;
                    salvarEstatisticas(&jogador);
                    remove(PARTIDA_FILE);
                    break;
                } else if (verificarEmpate(jogo.tabuleiro)) {
                    printf("Empate!\n");
                    jogador.empates++;
                    salvarEstatisticas(&jogador);
                    remove(PARTIDA_FILE);
                    break;
                }
                jogo.jogadorAtual = (jogo.jogadorAtual == 'X') ? 'O' : 'X';
            } else {
                printf("Posição ocupada! Tente novamente.\n");
            }

            salvarPartida(&jogo);
        }

        if (!jogarNovamente()) {
            break;
        }

    } while (1);

    return 0;
}

void limparTabuleiro(char tabuleiro[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            tabuleiro[i][j] = ' ';
}

void exibirTabuleiro(char tabuleiro[3][3]) {
    printf("\nTabuleiro:\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf(" %c ", tabuleiro[i][j]);
            if (j < 2) printf("|");
        }
        printf("\n");
        if (i < 2) printf("---+---+---\n");
    }
    printf("\n");
}

int verificarVitoria(char tabuleiro[3][3]) {
    for (int i = 0; i < 3; i++) {
        if (tabuleiro[i][0] != ' ' && tabuleiro[i][0] == tabuleiro[i][1] && tabuleiro[i][1] == tabuleiro[i][2]) return 1;
        if (tabuleiro[0][i] != ' ' && tabuleiro[0][i] == tabuleiro[1][i] && tabuleiro[1][i] == tabuleiro[2][i]) return 1;
    }
    if (tabuleiro[0][0] != ' ' && tabuleiro[0][0] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][2]) return 1;
    if (tabuleiro[0][2] != ' ' && tabuleiro[0][2] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][0]) return 1;
    return 0;
}

int verificarEmpate(char tabuleiro[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (tabuleiro[i][j] == ' ') return 0;
    return 1;
}

int realizarJogada(char tabuleiro[3][3], int linha, int coluna, char jogador) {
    if (tabuleiro[linha][coluna] == ' ') {
        tabuleiro[linha][coluna] = jogador;
        return 1;
    }
    return 0;
}

int verificarPartidaNaoFinalizada(Jogo *jogo) {
    FILE *arquivo = fopen(PARTIDA_FILE, "rb");
    if (!arquivo) return 0;
    fread(jogo, sizeof(Jogo), 1, arquivo);
    fclose(arquivo);
    return 1;
}

void salvarPartida(Jogo *jogo) {
    FILE *arquivo = fopen(PARTIDA_FILE, "wb");
    if (arquivo) {
        fwrite(jogo, sizeof(Jogo), 1, arquivo);
        fclose(arquivo);
    }
}

void salvarEstatisticas(Jogador *jogador) {
    FILE *arquivo = fopen(FILENAME, "a");
    if (arquivo) {
        fprintf(arquivo, "%s %d %d %d %d\n", jogador->nome, jogador->partidas, jogador->vitorias, jogador->empates, jogador->derrotas);
        fclose(arquivo);
    }
}

void carregarEstatisticas(Jogador *jogador) {
    FILE *arquivo = fopen(FILENAME, "r");
    if (!arquivo) return;
    Jogador temp;
    while (fscanf(arquivo, "%s %d %d %d %d", temp.nome, &temp.partidas, &temp.vitorias, &temp.empates, &temp.derrotas) != EOF) {
        if (strcmp(jogador->nome, temp.nome) == 0) {
            *jogador = temp;
            break;
        }
    }
    fclose(arquivo);
}

void realizarLogin(Jogador *jogador) {
    FILE *arquivo = fopen(LOGIN_FILE, "r");
    if (!arquivo) {
        printf("Arquivo de login não encontrado.\n");
        return;
    }
    char nome[MAX_NAME_LEN], senha[20];
    char linha[MAX_NAME_LEN + 30];
    int found = 0;

    printf("Digite seu nome: ");
    fgets(nome, MAX_NAME_LEN, stdin);
    nome[strcspn(nome, "\n")] = '\0';

    printf("Digite sua senha: ");
    mascararSenha(senha);

    while (fgets(linha, sizeof(linha), arquivo)) {
        char storedName[MAX_NAME_LEN], storedPass[20];
        sscanf(linha, "%s %s", storedName, storedPass);
        if (strcmp(nome, storedName) == 0 && strcmp(senha, storedPass) == 0) {
            strcpy(jogador->nome, nome);
            found = 1;
            break;
        }
    }
    fclose(arquivo);

    if (!found) {
        printf("Login não encontrado. Registrando novo jogador.\n");
        strcpy(jogador->nome, nome);
        jogador->partidas = 0;
        jogador->vitorias = 0;
        jogador->empates = 0;
        jogador->derrotas = 0;
        registrarJogador(jogador);
    }
}

void registrarJogador(Jogador *jogador) {
    FILE *arquivo = fopen(LOGIN_FILE, "a");
    char senha[20];
    printf("Digite a senha para registrar: ");
    mascararSenha(senha);
    fprintf(arquivo, "%s %s\n", jogador->nome, senha);
    fclose(arquivo);
}

void mascararSenha(char *senha) {
    int i = 0;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != 8) {
            senha[i++] = ch;
            printf("*");
        }
    }
    senha[i] = '\0';
    printf("\n");
}

int jogarNovamente() {
    int opcao;
    printf("\nDeseja jogar novamente? (1 - Sim, 2 - Não): ");
    scanf("%d", &opcao);
    return opcao == 1;
}
