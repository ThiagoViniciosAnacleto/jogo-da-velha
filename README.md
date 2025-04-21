# Jogo da Velha com Temporizador

Um jogo da velha completo em C com sistema de login, estatísticas, temporizador e várias funcionalidades avançadas.

## Funcionalidades

### Sistema de Jogadores
- **Login e Registro**: Crie uma conta com senha ou jogue sem cadastro
- **Estatísticas Completas**: Acompanhe vitórias, derrotas e empates de cada jogador
- **Troca de Jogadores**: Troque jogadores sem precisar reiniciar o programa

### Mecânica de Jogo
- **Temporizador de Jogadas**: Cada jogador tem 30 segundos para fazer sua jogada
- **Penalidade por Tempo**: Jogador perde a partida se exceder o tempo limite
- **Salvamento Automático**: Partidas em andamento são salvas automaticamente
- **Continuação de Partidas**: Opção de continuar partidas não finalizadas

### Interface
- **Tabuleiro Visual**: Visualização clara do tabuleiro após cada jogada
- **Placar Detalhado**: Exibição de estatísticas atualizadas entre partidas
- **Mensagens Motivacionais**: Mensagens personalizadas baseadas no placar atual
- **Indicador de Tempo**: Exibição suave do tempo restante para cada jogada

## Requisitos do Sistema

- Sistema operacional Windows
- Compilador C (GCC, MinGW ou Visual Studio)
- Terminal com suporte a caracteres UTF-8

## Como Compilar

### Usando GCC/MinGW
```bash
gcc jogo-da-velha.c -o jogo-da-velha
```

### Usando Visual Studio
1. Crie um novo projeto de aplicativo de console em C
2. Adicione o arquivo jogo-da-velha.c ao projeto
3. Compile o projeto

## Como Jogar

1. Execute o programa compilado
2. Escolha entre fazer login, criar uma nova conta ou jogar sem cadastro
3. Cada jogador deve inserir suas jogadas no formato "linha coluna" (ex: 1 2)
4. Você tem 30 segundos para fazer cada jogada
5. Vence quem completar uma linha, coluna ou diagonal
6. Ao final da partida, escolha entre jogar novamente, trocar jogadores ou encerrar

## Arquivos Gerados

O jogo cria e utiliza os seguintes arquivos:
- **jogo_estatisticas.txt**: Armazena as estatísticas dos jogadores
- **logins.txt**: Armazena as credenciais de login dos jogadores
- **partida_em_andamento.txt**: Salva o estado da partida atual

## Estrutura do Código

### Estruturas de Dados
- `Jogador`: Armazena informações sobre um jogador (nome, estatísticas)
- `Jogo`: Armazena o estado atual do jogo (tabuleiro, jogador atual)

### Principais Funções
- `menuInicialJogador()`: Gerencia o menu inicial para cada jogador
- `realizarLogin()`: Gerencia o processo de login
- `registrarNovoJogador()`: Cria uma nova conta de jogador
- `trocarJogadores()`: Permite trocar jogadores durante a sessão
- `reiniciarJogo()`: Reinicia o tabuleiro para uma nova partida
- `lerJogadaComTemporizador()`: Gerencia a entrada do jogador com limite de tempo
- `verificarVitoria()`: Verifica se há um vencedor
- `verificarEmpate()`: Verifica se houve empate

## Dicas e Estratégias

- Lembre-se que você tem apenas 30 segundos para fazer sua jogada
- O primeiro jogador (X) tem uma pequena vantagem estratégica
- Cantos e centro são posições estrategicamente importantes
- Bloqueie seu oponente quando ele tiver duas peças alinhadas
- Tente criar duas ameaças simultâneas para garantir a vitória

## Contribuições e Melhorias Futuras

Ideias para futuras melhorias:
- Modo de jogo contra o computador (IA)
- Sistema de ranking dos melhores jogadores
- Personalização de símbolos e cores
- Replay de partidas anteriores
- Modo torneio (melhor de 3, 5, etc.)

## Licença

Este projeto é livre para uso educacional e pessoal.

---

Desenvolvido como projeto educacional para prática de programação em C.
