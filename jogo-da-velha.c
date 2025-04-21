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
void mostrarMenuFinal(int *opcao);
void reiniciarJogo(Jogo *jogo, Jogador *jogador1, Jogador *jogador2);

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Portuguese_Brasil");

    Jogador jogador1 = {"", 0, 0, 0, 0};
    Jogador jogador2 = {"", 0, 0, 0, 0};
    Jogo jogo;
    int opcao, linha, coluna, continuar, jogarMais;

    while (1) {
        // Menu principal - A cada novo ciclo, o login será requisitado
        printf("Bem-vindo ao Jogo da Velha!\n");

        // Login para jogador 1
        printf("Jogador 1 - Deseja fazer login? (1 - Sim, 2 - Não): ");
        scanf("%d", &opcao);
        getchar(); // Limpa buffer

        if (opcao == 1) {
            realizarLogin(&jogador1);
        } else {
            printf("Digite o nome do Jogador 1: ");
            fgets(jogador1.nome, MAX_NAME_LEN, stdin);
            jogador1.nome[strcspn(jogador1.nome, "\n")] = '\0';
            registrarJogador(&jogador1);
        }

        // Login para jogador 2
        printf("Jogador 2 - Deseja fazer login? (1 - Sim, 2 - Não): ");
        scanf("%d", &opcao);
        getchar(); // Limpa buffer

        if (opcao == 1) {
            realizarLogin(&jogador2);
        } else {
            printf("Digite o nome do Jogador 2: ");
            fgets(jogador2.nome, MAX_NAME_LEN, stdin);
            jogador2.nome[strcspn(jogador2.nome, "\n")] = '\0';
            registrarJogador(&jogador2);
        }

        carregarEstatisticas(&jogador1);
        carregarEstatisticas(&jogador2);

        if (verificarPartidaNaoFinalizada(&jogo)) {
            printf("Partida anterior não finalizada, deseja continuar? (1 - Sim, 2 - Não): ");
            scanf("%d", &continuar);
            if (continuar != 1) {
                reiniciarJogo(&jogo, &jogador1, &jogador2);  // Reinicia tudo se a partida não for continuada
            }
        } else {
            reiniciarJogo(&jogo, &jogador1, &jogador2);  // Inicializa o jogo
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
                if (jogo.jogadorAtual == 'X') jogador1.partidas++;
                else jogador2.partidas++;

                if (verificarVitoria(jogo.tabuleiro)) {
                    printf("Jogador %c venceu!\n", jogo.jogadorAtual);
                    if (jogo.jogadorAtual == 'X') jogador1.vitorias++;
                    else jogador2.vitorias++;
                    salvarEstatisticas(&jogador1);
                    salvarEstatisticas(&jogador2);
                    remove(PARTIDA_FILE);
                    break;
                } else if (verificarEmpate(jogo.tabuleiro)) {
                    printf("Empate!\n");
                    jogador1.empates++;
                    jogador2.empates++;
                    salvarEstatisticas(&jogador1);
                    salvarEstatisticas(&jogador2);
                    remove(PARTIDA_FILE);
                    break;
                }
                jogo.jogadorAtual = (jogo.jogadorAtual == 'X') ? 'O' : 'X';
            } else {
                printf("Posição ocupada! Tente novamente.\n");
            }

            salvarPartida(&jogo);
        }

        // Menu final: opções para jogar novamente, voltar ao início ou encerrar
        mostrarMenuFinal(&jogarMais);

        if (jogarMais == 1) {
            continue; // Jogo segue para mais uma partida
        } else if (jogarMais == 2) {
            reiniciarJogo(&jogo, &jogador1, &jogador2); // Reinicia o jogo e volta ao início
            printf("\nVoltando ao início...\n");
            continue; // Volta ao início, solicitando login novamente
        } else {
            printf("Jogo encerrado. Até logo!\n");
            break; // Encerra o jogo
        }
    }

    return 0;
}

void reiniciarJogo(Jogo *jogo, Jogador *jogador1, Jogador *jogador2) {
    memset(jogo, 0, sizeof(Jogo)); // Zera o estado do jogo
    memset(jogador1, 0, sizeof(Jogador)); // Zera o estado do jogador 1
    memset(jogador2, 0, sizeof(Jogador)); // Zera o estado do jogador 2
    limparTabuleiro(jogo->tabuleiro);
    jogo->jogadorAtual = 'X';  // Jogador 1 começa
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
    char senha[50], senhaArquivo[50];
    printf("Digite o nome de usuário: ");
    fgets(jogador->nome, MAX_NAME_LEN, stdin);
    jogador->nome[strcspn(jogador->nome, "\n")] = '\0';
    printf("Digite a senha: ");
    mascararSenha(senha);
    FILE *arquivo = fopen(LOGIN_FILE, "r");
    if (!arquivo) {
        printf("Erro ao acessar o arquivo de login.\n");
        return;
    }
    while (fscanf(arquivo, "%s %s", jogador->nome, senhaArquivo) != EOF) {
        if (strcmp(jogador->nome, jogador->nome) == 0 && strcmp(senha, senhaArquivo) == 0) {
            printf("Login bem-sucedido!\n");
            fclose(arquivo);
            return;
        }
    }
    fclose(arquivo);
    printf("Falha no login. Usuário ou senha incorretos.\n");
}

void registrarJogador(Jogador *jogador) {
    FILE *arquivo = fopen(LOGIN_FILE, "a");
    if (!arquivo) {
        printf("Erro ao acessar o arquivo de login.\n");
        return;
    }
    fprintf(arquivo, "%s %s\n", jogador->nome, jogador->nome);  // senha igual ao nome
    fclose(arquivo);
}

void mascararSenha(char *senha) {
    char ch;
    int i = 0;
    while ((ch = _getch()) != '\r') {
        if (ch == 8 && i > 0) {  // backspace
            i--;
            printf("\b \b");  // Remove a última estrela
        } else if (ch != 8) {
            senha[i++] = ch;
            printf("*");
        }
    }
    senha[i] = '\0';  // Termina a string
}

void mostrarMenuFinal(int *opcao) {
    printf("\nMenu Final\n");
    printf("1. Jogar novamente\n");
    printf("2. Voltar ao início\n");
    printf("3. Sair\n");
    printf("Escolha uma opção: ");
    scanf("%d", opcao);
}
