#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <windows.h>
#include <locale.h>

#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 50
#define FILENAME "jogo_estatisticas.txt"
#define MAX_PLAYERS 100
#define LOGIN_FILE "logins.txt"
#define PARTIDA_FILE "partida_em_andamento.txt"
#define MAX_TENTATIVAS 3  // Constante para o número máximo de tentativas de login

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
void registrarNovoJogador(Jogador *jogador);
void registrarJogador(Jogador *jogador);
void mascararSenha(char *senha);
void mostrarMenuFinal(int *opcao);
void reiniciarJogo(Jogo *jogo, Jogador *jogador1, Jogador *jogador2);
void trocarJogadores(Jogador *jogador1, Jogador *jogador2);
int verificarCredenciais(const char *nome, const char *senha);
void salvarCredenciais(const char *nome, const char *senha);
void menuInicialJogador(Jogador *jogador, const char *numeroJogador);

int main() {
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Portuguese_Brasil");

    Jogador jogador1 = {"", 0, 0, 0, 0};
    Jogador jogador2 = {"", 0, 0, 0, 0};
    Jogo jogo;
    int linha, coluna, continuar, jogarMais;
    int sessaoAtiva = 1; // Variável para controlar a sessão do jogo

    // Login inicial (somente uma vez)
    printf("Bem-vindo ao Jogo da Velha!\n");

    // Menu inicial para jogador 1
    menuInicialJogador(&jogador1, "1");
    
    // Menu inicial para jogador 2
    menuInicialJogador(&jogador2, "2");

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
                // Removido o incremento de partidas a cada jogada

                if (verificarVitoria(jogo.tabuleiro)) {
                    exibirTabuleiro(jogo.tabuleiro); // Mostrar o tabuleiro final
                    printf("Jogador %c (%s) venceu!\n", 
                           jogo.jogadorAtual, 
                           jogo.jogadorAtual == 'X' ? jogador1.nome : jogador2.nome);
                    
                    // Incrementar partidas para ambos os jogadores ao final da partida
                    jogador1.partidas++;
                    jogador2.partidas++;
                    
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
                    
                    // Incrementar partidas para ambos os jogadores ao final da partida
                    jogador1.partidas++;
                    jogador2.partidas++;
                    
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

// Nova função para exibir o menu inicial para cada jogador
void menuInicialJogador(Jogador *jogador, const char *numeroJogador) {
    int opcao;
    
    printf("\n=== Jogador %s ===\n", numeroJogador);
    printf("1 - Fazer login (já tenho cadastro)\n");
    printf("2 - Criar nova conta\n");
    printf("3 - Jogar sem cadastro\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    getchar(); // Limpa buffer
    
    switch (opcao) {
        case 1:
            realizarLogin(jogador);
            break;
        case 2:
            registrarNovoJogador(jogador);
            break;
        case 3:
            printf("Digite o nome do Jogador %s: ", numeroJogador);
            fgets(jogador->nome, MAX_NAME_LEN, stdin);
            jogador->nome[strcspn(jogador->nome, "\n")] = '\0';
            printf("Jogador %s registrado sem cadastro.\n", jogador->nome);
            break;
        default:
            printf("Opção inválida! Jogando sem cadastro.\n");
            printf("Digite o nome do Jogador %s: ", numeroJogador);
            fgets(jogador->nome, MAX_NAME_LEN, stdin);
            jogador->nome[strcspn(jogador->nome, "\n")] = '\0';
            break;
    }
}

// Nova função para registrar um novo jogador com senha
void registrarNovoJogador(Jogador *jogador) {
    char nome[MAX_NAME_LEN];
    char senha[MAX_PASS_LEN];
    
    printf("=== Criação de Nova Conta ===\n");
    printf("Digite um nome de usuário: ");
    fgets(nome, MAX_NAME_LEN, stdin);
    nome[strcspn(nome, "\n")] = '\0';
    
    printf("Digite uma senha: ");
    mascararSenha(senha);
    
    // Salvar as novas credenciais
    salvarCredenciais(nome, senha);
    
    printf("Conta criada com sucesso!\n");
    strcpy(jogador->nome, nome);
}

void trocarJogadores(Jogador *jogador1, Jogador *jogador2) {
    int opcao;
    
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
        
        // Usar o novo menu inicial para o jogador 1
        menuInicialJogador(jogador1, "1");
        
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
        
        // Usar o novo menu inicial para o jogador 2
        menuInicialJogador(jogador2, "2");
        
        // Carregar estatísticas do novo jogador 2, se existirem
        carregarEstatisticas(jogador2);
    }
    
    printf("\nJogadores atualizados!\n");
    printf("Jogador 1: %s\n", jogador1->nome);
    printf("Jogador 2: %s\n", jogador2->nome);
}

void reiniciarJogo(Jogo *jogo, Jogador *jogador1, Jogador *jogador2) {
    // Limpar o tabuleiro e reiniciar o jogo
    memset(jogo, 0, sizeof(Jogo));
    limparTabuleiro(jogo->tabuleiro);
    jogo->jogadorAtual = 'X';  // Jogador 1 começa
    
    // Exibir mensagem personalizada com placar
    printf("\n===================================\n");
    printf("   NOVA PARTIDA DE JOGO DA VELHA   \n");
    printf("===================================\n");
    printf("%s (X) vs %s (O)\n", jogador1->nome, jogador2->nome);
    printf("\nPLACAR ATUAL:\n");
    printf("%-15s: %d partidas, %d vitórias, %d empates, %d derrotas\n", 
           jogador1->nome, jogador1->partidas, jogador1->vitorias, jogador1->empates, jogador1->derrotas);
    printf("%-15s: %d partidas, %d vitórias, %d empates, %d derrotas\n", 
           jogador2->nome, jogador2->partidas, jogador2->vitorias, jogador2->empates, jogador2->derrotas);
    printf("===================================\n");
    
    // Mensagem motivacional baseada nas estatísticas
    if (jogador1->vitorias > jogador2->vitorias) {
        printf("%s está na frente! %s, hora da revanche!\n", jogador1->nome, jogador2->nome);
    } else if (jogador2->vitorias > jogador1->vitorias) {
        printf("%s está na frente! %s, hora da revanche!\n", jogador2->nome, jogador1->nome);
    } else {
        printf("Placar empatado! Quem vai desempatar?\n");
    }
    printf("===================================\n\n");
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
        // Novo formato: "Nome, partidas, vitorias, empates, derrotas;"
        fprintf(arquivo, "%s, %d, %d, %d, %d;\n",
                jogador->nome, jogador->partidas, jogador->vitorias, jogador->empates, jogador->derrotas);
        fclose(arquivo);
    }
}

void carregarEstatisticas(Jogador *jogador) {
    FILE *arquivo = fopen(FILENAME, "r");
    if (arquivo) {
        char linha[100];
        while (fgets(linha, sizeof(linha), arquivo)) {
            // Verificar se a linha contém o nome do jogador
            if (strstr(linha, jogador->nome)) {
                // Tentar ler no novo formato primeiro
                char nome[MAX_NAME_LEN];
                int partidas, vitorias, empates, derrotas;
                
                // Novo formato: "Nome, partidas, vitorias, empates, derrotas;"
                if (sscanf(linha, "%[^,], %d, %d, %d, %d;", 
                          nome, &partidas, &vitorias, &empates, &derrotas) == 5) {
                    strcpy(jogador->nome, nome);
                    jogador->partidas = partidas;
                    jogador->vitorias = vitorias;
                    jogador->empates = empates;
                    jogador->derrotas = derrotas;
                } 
                // Tentar ler no formato antigo como fallback
                else if (sscanf(linha, "%s - Partidas: %d, Vitorias: %d, Empates: %d, Derrotas: %d",
                              jogador->nome, &jogador->partidas, &jogador->vitorias, 
                              &jogador->empates, &jogador->derrotas) == 5) {
                    // Dados já foram carregados pelo sscanf
                }
            }
        }
        fclose(arquivo);
    }
}

// Verifica se as credenciais existem no arquivo de login
int verificarCredenciais(const char *nome, const char *senha) {
    FILE *arquivo = fopen(LOGIN_FILE, "r");
    if (!arquivo) return 0;
    
    char linha[MAX_NAME_LEN + MAX_PASS_LEN + 2]; // +2 para o separador e \0
    char nomeArquivo[MAX_NAME_LEN];
    char senhaArquivo[MAX_PASS_LEN];
    
    while (fgets(linha, sizeof(linha), arquivo)) {
        // Remover quebra de linha
        linha[strcspn(linha, "\n")] = '\0';
        
        // Separar nome e senha
        if (sscanf(linha, "%[^,],%s", nomeArquivo, senhaArquivo) == 2) {
            if (strcmp(nome, nomeArquivo) == 0 && strcmp(senha, senhaArquivo) == 0) {
                fclose(arquivo);
                return 1; // Credenciais válidas
            }
        }
    }
    
    fclose(arquivo);
    return 0; // Credenciais inválidas
}

// Salva as credenciais no arquivo de login
void salvarCredenciais(const char *nome, const char *senha) {
    FILE *arquivo = fopen(LOGIN_FILE, "a");
    if (arquivo) {
        fprintf(arquivo, "%s,%s\n", nome, senha);
        fclose(arquivo);
    }
}

void realizarLogin(Jogador *jogador) {
    char nome[MAX_NAME_LEN];
    char senha[MAX_PASS_LEN];
    int tentativas = 0;
    int opcao;
    
    printf("=== Login de Usuário ===\n");
    
    // Implementação do limite de tentativas de login
    do {
        printf("Digite seu nome: ");
        fgets(nome, MAX_NAME_LEN, stdin);
        nome[strcspn(nome, "\n")] = '\0'; // Remove a nova linha
        
        printf("Digite sua senha: ");
        mascararSenha(senha);
        
        // Verificar se as credenciais existem
        if (verificarCredenciais(nome, senha)) {
            printf("Login realizado com sucesso!\n");
            strcpy(jogador->nome, nome);
            return; // Sai da função se o login for bem-sucedido
        } else {
            tentativas++;
            if (tentativas < MAX_TENTATIVAS) {
                printf("Credenciais inválidas. Tentativa %d/%d.\n", tentativas, MAX_TENTATIVAS);
                printf("1 - Tentar novamente\n");
                printf("2 - Criar nova conta\n");
                printf("Escolha uma opção: ");
                scanf("%d", &opcao);
                getchar(); // Limpa buffer
                
                if (opcao == 2) {
                    registrarNovoJogador(jogador);
                    return;
                }
            }
        }
    } while (tentativas < MAX_TENTATIVAS);
    
    // Se chegou aqui, é porque excedeu o número máximo de tentativas
    printf("Número máximo de tentativas excedido.\n");
    
    // Perguntar se deseja registrar um novo usuário
    printf("1 - Criar nova conta\n");
    printf("2 - Jogar sem cadastro\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    getchar(); // Limpa buffer
    
    if (opcao == 1) {
        registrarNovoJogador(jogador);
    } else {
        // Se não quiser registrar, usar um nome temporário
        printf("Digite um nome para jogar sem cadastro: ");
        fgets(nome, MAX_NAME_LEN, stdin);
        nome[strcspn(nome, "\n")] = '\0';
        strcpy(jogador->nome, nome);
    }
}

void registrarJogador(Jogador *jogador) {
    char senha[MAX_PASS_LEN];
    int opcao;
    
    printf("Deseja registrar este jogador com senha? (1 - Sim, 0 - Não): ");
    scanf("%d", &opcao);
    getchar(); // Limpa buffer
    
    if (opcao == 1) {
        printf("Digite uma senha para o jogador %s: ", jogador->nome);
        mascararSenha(senha);
        
        // Salvar as credenciais
        salvarCredenciais(jogador->nome, senha);
        printf("Jogador %s registrado com sucesso!\n", jogador->nome);
    } else {
        printf("Jogador %s registrado sem senha.\n", jogador->nome);
    }
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
