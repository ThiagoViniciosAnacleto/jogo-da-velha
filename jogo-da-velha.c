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
void trocarJogadores(Jogador *jogador1, Jogador *jogador2);

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Portuguese_Brasil");

    Jogador jogador1 = {"", 0, 0, 0, 0};
    Jogador jogador2 = {"", 0, 0, 0, 0};
    Jogo jogo;
    int opcao, linha, coluna, continuar, jogarMais;
    int sessaoAtiva = 1; // Variável para controlar a sessão do jogo

    // Login inicial (somente uma vez)
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

    // Loop externo para controlar a sessão do jogo
    while (sessaoAtiva) {
        if (verificarPartidaNaoFinalizada(&jogo)) {
            printf("Partida anterior não finalizada, deseja continuar? (1 - Sim, 2 - Não): ");
            scanf("%d", &continuar);
            if (continuar != 1) {
                reiniciarJogo(&jogo, &jogador1, &jogador2);  // Reinicia tudo se a partida não for continuada
            }
        } else {
            reiniciarJogo(&jogo, &jogador1, &jogador2);  // Inicializa o jogo
        }

        int partidaEmAndamento = 1;
        
        // Loop interno para controlar uma partida específica
        while (partidaEmAndamento) {
            exibirTabuleiro(jogo.tabuleiro);
            printf("Jogador %c (%s), faça sua jogada (linha coluna): ", 
                   jogo.jogadorAtual, 
                   jogo.jogadorAtual == 'X' ? jogador1.nome : jogador2.nome);
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
                    exibirTabuleiro(jogo.tabuleiro); // Mostrar o tabuleiro final
                    printf("Jogador %c (%s) venceu!\n", 
                           jogo.jogadorAtual, 
                           jogo.jogadorAtual == 'X' ? jogador1.nome : jogador2.nome);
                    if (jogo.jogadorAtual == 'X') {
                        jogador1.vitorias++;
                        jogador2.derrotas++;
                    } else {
                        jogador2.vitorias++;
                        jogador1.derrotas++;
                    }
                    salvarEstatisticas(&jogador1);
                    salvarEstatisticas(&jogador2);
                    remove(PARTIDA_FILE);
                    partidaEmAndamento = 0; // Encerra a partida atual
                } else if (verificarEmpate(jogo.tabuleiro)) {
                    exibirTabuleiro(jogo.tabuleiro); // Mostrar o tabuleiro final
                    printf("Empate!\n");
                    jogador1.empates++;
                    jogador2.empates++;
                    salvarEstatisticas(&jogador1);
                    salvarEstatisticas(&jogador2);
                    remove(PARTIDA_FILE);
                    partidaEmAndamento = 0; // Encerra a partida atual
                } else {
                    jogo.jogadorAtual = (jogo.jogadorAtual == 'X') ? 'O' : 'X';
                    salvarPartida(&jogo);
                }
            } else {
                printf("Posição ocupada! Tente novamente.\n");
            }
        }

        // Menu final APÓS o término da partida (não dentro do loop de jogadas)
        mostrarMenuFinal(&jogarMais);

        if (jogarMais == 1) {
            // Apenas reinicia o jogo para uma nova partida, mantendo os jogadores
            reiniciarJogo(&jogo, &jogador1, &jogador2);
            // Não sai do loop externo, então os jogadores continuam os mesmos
        } else if (jogarMais == 2) {
            // Nova funcionalidade: trocar jogadores sem sair do programa
            printf("\nTrocando jogadores...\n");
            trocarJogadores(&jogador1, &jogador2);
            reiniciarJogo(&jogo, &jogador1, &jogador2);
        } else {
            printf("Jogo encerrado. Até logo!\n");
            sessaoAtiva = 0; // Encerra a sessão do jogo e sai do loop externo
        }
    }

    return 0;
}

void trocarJogadores(Jogador *jogador1, Jogador *jogador2) {
    int opcao;
    char buffer[MAX_NAME_LEN];
    
    // Salvar estatísticas dos jogadores atuais antes de trocar
    salvarEstatisticas(jogador1);
    salvarEstatisticas(jogador2);
    
    printf("\n=== Troca de Jogadores ===\n");
    
    // Opções para o Jogador 1
    printf("\nJogador 1 atual: %s\n", jogador1->nome);
    printf("Deseja trocar o Jogador 1? (1 - Sim, 0 - Não): ");
    scanf("%d", &opcao);
    getchar(); // Limpa buffer
    
    if (opcao == 1) {
        // Reinicializar o jogador 1
        memset(jogador1, 0, sizeof(Jogador));
        
        printf("Novo Jogador 1 - Deseja fazer login? (1 - Sim, 2 - Não): ");
        scanf("%d", &opcao);
        getchar(); // Limpa buffer
        
        if (opcao == 1) {
            realizarLogin(jogador1);
        } else {
            printf("Digite o nome do novo Jogador 1: ");
            fgets(buffer, MAX_NAME_LEN, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(jogador1->nome, buffer);
            registrarJogador(jogador1);
        }
        
        // Carregar estatísticas do novo jogador 1, se existirem
        carregarEstatisticas(jogador1);
    }
    
    // Opções para o Jogador 2
    printf("\nJogador 2 atual: %s\n", jogador2->nome);
    printf("Deseja trocar o Jogador 2? (1 - Sim, 0 - Não): ");
    scanf("%d", &opcao);
    getchar(); // Limpa buffer
    
    if (opcao == 1) {
        // Reinicializar o jogador 2
        memset(jogador2, 0, sizeof(Jogador));
        
        printf("Novo Jogador 2 - Deseja fazer login? (1 - Sim, 2 - Não): ");
        scanf("%d", &opcao);
        getchar(); // Limpa buffer
        
        if (opcao == 1) {
            realizarLogin(jogador2);
        } else {
            printf("Digite o nome do novo Jogador 2: ");
            fgets(buffer, MAX_NAME_LEN, stdin);
            buffer[strcspn(buffer, "\n")] = '\0';
            strcpy(jogador2->nome, buffer);
            registrarJogador(jogador2);
        }
        
        // Carregar estatísticas do novo jogador 2, se existirem
        carregarEstatisticas(jogador2);
    }
    
    printf("\nJogadores atualizados!\n");
    printf("Jogador 1: %s\n", jogador1->nome);
    printf("Jogador 2: %s\n", jogador2->nome);
}

void reiniciarJogo(Jogo *jogo, Jogador *jogador1, Jogador *jogador2) {
    memset(jogo, 0, sizeof(Jogo)); // Zera o estado do jogo
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
        fprintf(arquivo, "%s - Partidas: %d, Vitorias: %d, Empates: %d, Derrotas: %d\n",
                jogador->nome, jogador->partidas, jogador->vitorias, jogador->empates, jogador->derrotas);
        fclose(arquivo);
    }
}

void carregarEstatisticas(Jogador *jogador) {
    FILE *arquivo = fopen(FILENAME, "r");
    if (arquivo) {
        char linha[100];
        while (fgets(linha, sizeof(linha), arquivo)) {
            if (strstr(linha, jogador->nome)) {
                sscanf(linha, "%s - Partidas: %d, Vitorias: %d, Empates: %d, Derrotas: %d",
                       jogador->nome, &jogador->partidas, &jogador->vitorias, &jogador->empates, &jogador->derrotas);
            }
        }
        fclose(arquivo);
    }
}

void realizarLogin(Jogador *jogador) {
    char nome[MAX_NAME_LEN];
    char senha[MAX_NAME_LEN];
    printf("Digite seu nome: ");
    fgets(nome, MAX_NAME_LEN, stdin);
    nome[strcspn(nome, "\n")] = '\0'; // Remove a nova linha
    printf("Digite sua senha: ");
    mascararSenha(senha);
    // Aqui seria possível verificar o login usando arquivos ou banco de dados, mas estamos simplificando.
    strcpy(jogador->nome, nome);
}

void registrarJogador(Jogador *jogador) {
    printf("Jogador %s registrado com sucesso!\n", jogador->nome);
}

void mascararSenha(char *senha) {
    char ch;
    int i = 0;
    while ((ch = getch()) != '\r') { // Espera até o Enter
        if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            senha[i] = ch;
            i++;
            printf("*");
        }
    }
    senha[i] = '\0'; // Finaliza a string de senha
    printf("\n");
}

void mostrarMenuFinal(int *opcao) {
    printf("\nO que deseja fazer?\n");
    printf("1 - Jogar novamente\n");
    printf("2 - Trocar jogadores\n");
    printf("3 - Encerrar jogo\n");
    printf("Escolha: ");
    scanf("%d", opcao);
    
    // Validação da entrada
    while (*opcao != 1 && *opcao != 2 && *opcao != 3) {
        printf("Opção inválida! Escolha 1 para jogar novamente, 2 para trocar jogadores ou 3 para encerrar: ");
        scanf("%d", opcao);
    }
}
