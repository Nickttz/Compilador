#include <bits/stdc++.h>
using namespace std;

// Definições dos tokens
#define INICIO  256 
#define FIM     257 
#define SE      258 
#define SENAO   259 
#define ENTAO   260 
#define ENQUANTO 261 
#define FACA    262 
#define LEIA    263 
#define ESCREVA 264 
#define VAR     265 
#define FIM_SE  266 
#define FIM_ENQUANTO 267 
#define ID      268 
#define NUM_INTEIRO 269 
#define NUM_FLOAT 270 
#define STRING 271
#define TIPO 272
#define BOOL 273
#define COMENTARIO 274
#define SIMBOLO 275

// Atributos
#define ATRIBUTO_DEFAULT 0
#define ATRIBUTO_ID 276
#define ATRIBUTO_NUM_INTEIRO 277
#define ATRIBUTO_NUM_FLOAT 278
#define ATRIBUTO_STRING 279
#define ATRIBUTO_TIPO 280
#define ATRIBUTO_COMENTARIO 281
#define ATRIBUTO_SIMBOLO 282

// Estrutura para os tokens
struct Token {
    int nome_token;
    int atributo;
};

// Estrutura para a tabela de símbolos
struct TabelaSimbolos {
    string lexema;
    string tipo;
};

char *readFile(const char *fileName);
int verificar_palavra_chave(string lexema);
bool issimbolo(char c);
int verificarTabela(string lexema, string tipo);
void erro(string esperado, string contexto);
void obterProximoToken();
Token proximo_token();
string lexema_aux;

void programa();       // Declaração das funções recursivas
void declaracoes();
void declaracao_lista();
void declaracao_lista_auxiliar();
void declaracao();
void tipo();
void id_lista();
void id_lista_auxiliar();
void comandos();
void comando_lista();
void comando_lista_auxiliar();
void comando();
void atribuicao();
void condicional();
void else_part();
void laco();
void entrada_saida();
void expressao();
void expressao_opcional();
void termo();
void termo_opcional();
void fator();
void expressao_bool();

char *code;
int cont_sim_lido = 0;
int estado = 0;
Token tokenAtual;
vector<TabelaSimbolos> tabela; // Vetor para armazenar a tabela de símbolos

int main() {
    code = readFile("programa.txt");
    if (code == NULL) {
        cout << "Erro ao ler o arquivo." << endl;
        return 1;
    }

    tokenAtual = proximo_token();
    while (tokenAtual.nome_token != FIM and tokenAtual.nome_token != EOF) {
      if (tokenAtual.nome_token == INICIO) {
          programa();
      } else {
          erro("inicio", "programa"); // Sincroniza o analisador
          obterProximoToken(); // Avança para evitar looping infinito
      }
    }

    delete[] code;  // Libera a memória alocada
    return 0;
}

// Função para ler arquivo
char *readFile(const char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *code = new char[f_size + 1];
    fread(code, 1, f_size, file);
    code[f_size] = '\0';
    fclose(file);
    return code;
}

// Verifica se a palavra é uma palavra-chave
int verificar_palavra_chave(string lexema) {
    if (lexema == "inicio") return INICIO;
    else if (lexema == "fim") return FIM;
    else if (lexema == "se") return SE;
    else if (lexema == "senao") return SENAO;
    else if (lexema == "enquanto") return ENQUANTO;
    else if (lexema == "faca") return FACA;
    else if (lexema == "leia") return LEIA;
    else if (lexema == "escreva") return ESCREVA;
    else if (lexema == "var") return VAR;
    else if (lexema == "fim_se") return FIM_SE;
    else if (lexema == "fim_enquanto") return FIM_ENQUANTO;
    else if (lexema == "entao") return ENTAO;
    else if (lexema == "int" || lexema == "bool" || lexema == "real") return TIPO;
    else if (lexema == "verdadeiro" || lexema == "falso") return BOOL;
    
    return -1;
}

bool issimbolo(char c) {
  const string simbolo = "\\;()=<>:%?!#$/|*,+-";
  return simbolo.find(c) != string::npos;
}

int verificarTabela(string lexema, string tipo) {
    // Verifica se já existe na tabela de símbolos
    for (int i = 0; i< tabela.size();i++) {
        if (tabela[i].lexema == lexema && tabela[i].tipo == tipo) {
            return i; // Retorna a linha se já existe
        }
    }

    // Se não existe, adiciona à tabela
    TabelaSimbolos nova_entrada = {lexema, tipo}; // Cria a nova entrada
    tabela.push_back(nova_entrada); // Adiciona à tabela
    return tabela.size() -1; // Retorna a nova linha
}

Token proximo_token() {
    Token token;
    char c;
    string lexema;
    int w;
    bool Eh_id, Eh_curto;
    int cont_colchete_i, cont_colchete_f;
    int linha;

    while (code[cont_sim_lido] != '\0') {
        
        switch (estado) {
          
          case 0:
            
            cont_colchete_i = 0;
            cont_colchete_f = 0;
            c = code[cont_sim_lido];
            lexema.clear();
            
            if (isdigit(c) or c == '+') { // número
              estado = 1;
            }else if(c == '-'){ //possível comentário
              if(code[cont_sim_lido + 1] == '-'){ //é comentario
                cont_sim_lido += 2;
                estado = 11;
              } else{estado = 1;} // é numero negativo
            } else if (c == '"') { // string
                estado = 5;
            } else if (isalpha(c) || c == '_') { // ID ou palavra-chave
                estado = 7;
            } else if (isspace(c)) { // Ignora espaços em branco
                cont_sim_lido++; 
            } else if (issimbolo(c)) {    
                lexema += c;
                cont_sim_lido++;
                estado = 15;
            } else { // Outro caractere
                printf("Simbolo não reconhecido: %c\n", c);
                estado = 0;
                cont_sim_lido++; // Ignora o caractere não reconhecido
            }
            break;
  
          case 1: // estado para numero
            lexema += code[cont_sim_lido++];
            
            while (isdigit(code[cont_sim_lido])) {
              lexema += code[cont_sim_lido++];
            }
            
            if (lexema[lexema.size()-1] == '+' || lexema[lexema.size()-1] == '-'){
              estado = 15;
              break;
        
            } else if (code[cont_sim_lido] == '.') {
                estado = 2;
                lexema += code[cont_sim_lido++];
            } else if (!isdigit(code[cont_sim_lido]) and code[cont_sim_lido] != ';') {
                //printf("Numero invalido\n");
                
                while(code[cont_sim_lido] != ';') { //pular os caracteres invalidos apos o numero
                  cont_sim_lido++;
                }
                estado = 0;
            } else {
              estado = 3;
            }
            break;
            
          case 2:
            while (isdigit(code[cont_sim_lido])) {
                lexema += code[cont_sim_lido++];
            }
            estado = 4;
            break;
          
          case 3:
            linha = verificarTabela(lexema.c_str(), "NUM");
            lexema_aux = lexema;
            token.nome_token = NUM_INTEIRO;
            token.atributo = ATRIBUTO_NUM_INTEIRO;
            estado = 0;
            return token;
            break;
          
          case 4:
            linha = verificarTabela(lexema.c_str(), "NUM");
            lexema_aux = lexema;
            token.nome_token = NUM_FLOAT;
            token.atributo = ATRIBUTO_NUM_FLOAT;
            estado = 0;
            return token;
            break;

          case 5: // estado string
            cont_sim_lido++;
            while (cont_sim_lido < strlen(code)) { // Use strlen para verificar o tamanho da string
            // Adiciona o caractere atual ao lexema
              if (code[cont_sim_lido] == '"' && (cont_sim_lido == 0 || code[cont_sim_lido - 1] != '\\')) {
                cont_sim_lido++; // Move para o próximo caractere após a aspa de fechamento
                estado = 6; // Define o próximo estado após fechar a string
                break;
              }
              lexema += code[cont_sim_lido++];
            }
            // Verifica se chegou ao fim do código sem encontrar o fechamento da string
            if (estado != 6) {
              printf("Erro: String não fechada.\n");
              estado = 0; // Volta ao estado inicial em caso de erro
            }
          break;
        
          case 6:
            linha = verificarTabela(lexema.c_str(), "STRING");
            lexema_aux = lexema;
            token.nome_token = STRING;
            token.atributo = ATRIBUTO_STRING;
            estado = 0;
            return token;
            
          case 7:
            c = code[cont_sim_lido];
            if (isalpha(c) || c == '_') {
                estado = 8;
            } else {
                cont_sim_lido++; // Ignora caracteres inválidos
            }
            break;
          
          case 8:  // estado para id
            Eh_id = true;
            while (isalpha(code[cont_sim_lido]) || code[cont_sim_lido] == '_') {
                lexema += code[cont_sim_lido++];
            }
            w = verificar_palavra_chave(lexema); // Verificar se o lexema é uma palavra-chave
            if (w != -1) {
                Eh_id = false;
                estado = 9;
            } else {
                estado = 10;
            }
            break;
          
          case 9:
            lexema_aux = lexema;
            token.nome_token = w;
            token.atributo = ATRIBUTO_DEFAULT;
            estado = 0;
            return token;
            break;
          
          case 10:
            linha = verificarTabela(lexema.c_str(), "ID");
            lexema_aux = lexema;
            token.nome_token = ID;
            token.atributo = ATRIBUTO_ID;
            estado = 0;
            return token;
            break;
        
          case 11: // estado para comentario
            Eh_curto = true;
            while (code[cont_sim_lido] != '\n') { 
              lexema += code[cont_sim_lido++];
              
              if (code[cont_sim_lido] == '[' and code[cont_sim_lido - 1] == '[') { //verifica se é "[["
                cont_colchete_i++;
              } 
              if (cont_colchete_i == 1) {
                Eh_curto = false;
                break;
              }
            }
            if (Eh_curto) {
              estado = 12;
            } else {
              estado = 13;
            }
            break;
            
          case 12:
            estado = 0;
            break;
          
          case 13:
            while (code[cont_sim_lido] != '\0') {
              lexema += code[cont_sim_lido++];
              
              if (code[cont_sim_lido] == '[') { //verifica se é "[["
                if(code[cont_sim_lido + 1] == '[' and code[cont_sim_lido - 1] != '[') {
                 cont_colchete_i++; // contar os colchetes iniciais
                 lexema += code[cont_sim_lido++]; // pular o segundo colchete e ir para o proximo
                } else {
                  estado = 0;
                  printf("Erro no comentario\n");
                  while(code[cont_sim_lido] != '\n') { //pular todos os caracteres
                   cont_sim_lido++;
                  }
                  break;
                }
              } else if (code[cont_sim_lido] == ']') { // verifica se é "]]"
                  if(code[cont_sim_lido + 1] == ']') {
                    cont_colchete_f++; // contar os colchetes finais
                    lexema += code[cont_sim_lido++]; // pular o segundo colchete e ir para o proximo
                  } else {
                    estado = 0;
                    printf("Erro no comentario\n");
                    while(code[cont_sim_lido] != '\n') { //pular todos os caracteres
                      cont_sim_lido++;
                    }
                    break;
                  }
                }
              if ((cont_colchete_i == cont_colchete_f) and !isspace(code[cont_sim_lido + 1])) {
                string aux = ";()[]=<>:%?!#$/|*";
                if(aux.find(code[cont_sim_lido]) != string::npos or isalnum(code[cont_sim_lido])) {
                 estado = 0;
                 printf("Erro encontrado no codigo\n");
                 
                 while(code[cont_sim_lido] != '\n' or isspace(code[cont_sim_lido])) { //pular todos os caracteres e espaços
                   cont_sim_lido++;
                 }
                 break;
                } else {
                    while (isspace(lexema[lexema.size() - 1])) {
                        lexema.erase(lexema.size() - 1, 1);
                    }
                  estado = 14;
                  break;
                }
              } else if(code[cont_sim_lido] == '\n' and cont_colchete_f != cont_colchete_i) {
                  estado = 0;
                  printf("Erro no comentario\n");
                  while(code[cont_sim_lido] != '\n') { //pular todos os caracteres
                    cont_sim_lido++;
                  }
                  break;
                }
            }
            break;
            
          case 14:
            estado = 0;
            cont_colchete_f = 0;
            cont_colchete_i = 0;
            break;
            
          case 15:
            lexema_aux = lexema;
            token.nome_token = SIMBOLO;
            token.atributo = ATRIBUTO_SIMBOLO;
            estado = 0;
            return token;
            break;
        }
    }
    // Finaliza a leitura
    token.nome_token = EOF;
    token.atributo = -1;
    return token;
}

// Função que obtém o próximo token
void obterProximoToken() {
    lexema_aux.clear(); 
    tokenAtual = proximo_token(); // Chama o analisador léxico
}

// Mapeamento do conjunto FIRST para cada função
map<string, set<string>> FIRST = {
    {"programa", {"inicio"}},
    {"declaracoes", {"var", ""}},
    {"declaracao_lista", {"int", "real", "bool", ""}},
    {"comandos", {"ID", "se", "enquanto", "leia", "escreva"}},
    {"comando", {"ID", "se", "enquanto", "leia", "escreva"}},
    {"atribuicao", {"ID"}},
    {"condicional", {"se"}},
    {"laco", {"enquanto"}},
    {"entrada_saida", {"leia", "escreva"}},
    {"expressao", {"ID", "NUM", "(", "verdadeiro", "falso"}},
    {"fator", {"ID", "NUM", "(", "verdadeiro", "falso"}}
};

// Mapeamento do conjunto DELIMITADOR para cada função
map<string, set<string>> DELIMITADORES = {
    {"programa", {"fim"}},  // O delimitador do programa é "fim"
    {"declaracoes", {"inicio", ""}},  // Pode ser "inicio" ou epsilon (ε) para fim de declaração
    {"declaracao_lista", {"inicio", ""}},  // Pode ser "inicio" ou epsilon (ε)
    {"comandos", {"fim", "fim_se", "fim_enquanto"}},  // "fim", "fim_se" ou "fim_enquanto" marcam o fim de comandos
    {"comando", {"fim", "fim_se", "fim_enquanto", ";"}},  // Instruções terminam com "fim", "fim_se", "fim_enquanto" ou ";"
    {"atribuicao", {"fim", "fim_se", "fim_enquanto", ";"}},  // A atribuição termina com "fim", "fim_se", "fim_enquanto" ou ";"
    {"condicional", {"fim_se", ";"}},  // O comando condicional termina com "fim_se" ou ";"
    {"laco", {"fim_enquanto", ";"}},  // O laço termina com "fim_enquanto" ou ";"
    {"entrada_saida", {"fim", "fim_se", "fim_enquanto", ";"}},  // Entrada e saída terminam com "fim", "fim_se", "fim_enquanto" ou ";"
    {"expressao", {")", ";", "fim", "fim_se", "fim_enquanto"}},  // Expressão termina com ")", ";", "fim", "fim_se", ou "fim_enquanto"
    {"fator", {")", ";", "fim", "fim_se", "fim_enquanto"}},  // Fator termina com ")", ";", "fim", "fim_se", ou "fim_enquanto"
    {"comando_lista", {"fim", "fim_enquanto", "fim_se"}},
    {"else_part", {"fim_se", "fim"}}
};

// Função de erro com modo pânico considerando FIRST e DELIMITADORES
void erro(string esperado, string contexto) {
    cout << "Erro sintático: esperado " << esperado 
         << ", mas encontrado " << lexema_aux << endl;

    // Verifique se a chave existe no mapa FIRST
    if (FIRST.find(contexto) == FIRST.end()) {
        cout << "Erro: contexto '" << contexto << "' não encontrado no mapa FIRST." << endl;
        return;
    }

    // Conjunto FIRST para a função atual
    set<string> firstTokens = FIRST[contexto];

    // Verifique se a chave existe no mapa DELIMITADORES
    if (DELIMITADORES.find(contexto) == DELIMITADORES.end()) {
        cout << "Erro: contexto '" << contexto << "' não encontrado no mapa DELIMITADORES." << endl;
        return;
    }

    // Conjunto de DELIMITADORES para a função atual
    set<string> delimitadores = DELIMITADORES[contexto];

    // Avança até encontrar um token válido do FIRST ou um delimitador
    while (tokenAtual.nome_token != FIM &&
           tokenAtual.nome_token != EOF &&
           lexema_aux != ";" &&
           firstTokens.find(lexema_aux) == firstTokens.end() &&
           delimitadores.find(lexema_aux) == delimitadores.end()) {
        obterProximoToken();  // Avança o token
        cout << "Avançando para o próximo token: " << lexema_aux << endl;  // Para depuração
    }

    // Verifique se o token encontrado é válido (encontrado no FIRST ou nos DELIMITADORES)
    if (firstTokens.find(lexema_aux) != firstTokens.end()) {
        cout << "Recuperado no contexto '" << contexto << "' com token do FIRST: " << lexema_aux << endl;
    } else if (delimitadores.find(lexema_aux) != delimitadores.end()) {
        cout << "Recuperado no contexto '" << contexto << "' com delimitador: " << lexema_aux << endl;
    } else {
        cout << "Delimitador ou token do FIRST não encontrado. Continuando a análise..." << endl;
    }
}

// Função para tratar o programa
void programa() {
    if (tokenAtual.nome_token == INICIO) {
        obterProximoToken();
        declaracoes();
        comandos();
        if (tokenAtual.nome_token == FIM) {
            cout << "Análise sintática concluída com sucesso!" << endl << endl;
            obterProximoToken();
        } else {
          erro("fim", "programa");
        }
    } else {
        erro("inicio", "declaracoes");
    }
}

// Função para tratar as declarações
void declaracoes() {
    if (tokenAtual.nome_token == VAR) {
        obterProximoToken();
        declaracao_lista();
    } else {
        erro("var", "declaracao_lista");
        declaracao_lista();
    }
}

// Função para tratar a lista de declarações
void declaracao_lista() {
    if (lexema_aux == "int" || lexema_aux == "real" || lexema_aux == "bool") {
        declaracao();  // Analisa uma declaração de variável
        declaracao_lista_auxiliar();
    }
}

void declaracao_lista_auxiliar(){
  if(lexema_aux == ";") {
      obterProximoToken();
      declaracao_lista();
  }
  else{
      return;
  }
}

// Função para tratar a declaração de variável
void declaracao() {
    tipo();   // Analisa o tipo da variável
    id_lista();  // Analisa os identificadores (nomes das variáveis)
}

// Função para tratar o tipo de variável
void tipo() {
    if (lexema_aux == "int" || lexema_aux == "real" || lexema_aux == "bool") {
        cout << "Tipo: " << lexema_aux << endl;
        obterProximoToken();
    } else {
        erro("tipo válido", "declaracao");
    }
}

// Função para tratar a lista de identificadores
void id_lista() {
    if (tokenAtual.nome_token == ID) {
        cout << "Declarando a variável " << lexema_aux << endl;
        obterProximoToken();
        id_lista_auxiliar();
        
    } else {
        erro("ID", "declaracao");
    }
}

void id_lista_auxiliar() {
  if (lexema_aux == ",") {
      obterProximoToken();
      id_lista();
  } else {
      return;
  }
}

// Função para tratar os comandos
void comandos() {
    comando_lista();  // Chama a função para analisar a lista de comandos
}

// Função para tratar a lista de comandos
// Função para tratar a lista de comandos
void comando_lista() {
    if (tokenAtual.nome_token == ID || tokenAtual.nome_token == SE || tokenAtual.nome_token == ENQUANTO || tokenAtual.nome_token == LEIA || tokenAtual.nome_token == ESCREVA) {
        comando();
        comando_lista_auxiliar();
    } 
}

// Função para tratar a parte auxiliar da lista de comandos
void comando_lista_auxiliar() {
    if (lexema_aux == ";") {
        obterProximoToken();
        comando_lista(); // Chamada recursiva para mais comandos
    } else {
        return;
    }
}

// Função para tratar um comando
void comando() {
    if (tokenAtual.nome_token == ID) {
        atribuicao();  // Analisa uma atribuição
    } else if (tokenAtual.nome_token == SE) {
        condicional();  // Analisa uma estrutura condicional
    } else if (tokenAtual.nome_token == ENQUANTO) {
        laco();  // Analisa um laço
    } else if (tokenAtual.nome_token == LEIA || tokenAtual.nome_token == ESCREVA) {
        entrada_saida();  // Analisa entrada ou saída
    } else {
        erro("comando válido", "comando");
    }
}

// Função para tratar uma atribuição
void atribuicao() {
    obterProximoToken();  // Avança para o identificador
    if (lexema_aux == "=") {
        obterProximoToken();
        expressao();  // Analisa a expressão à direita da atribuição
    } else {
        erro("=", "expressao");
    }
}

// Função para tratar uma estrutura condicional
void condicional() {
    obterProximoToken();  // Avança para o parêntese de abertura
    if (lexema_aux == "(") {
        obterProximoToken();
        expressao();  // Analisa a expressão da condição
        if (lexema_aux == ")") {
            obterProximoToken();
            if (tokenAtual.nome_token == ENTAO) {
                obterProximoToken();
                comandos();  // Analisa os comandos no "entao"
                else_part();  // Trata a parte do "senao" (opcional)
                if (tokenAtual.nome_token == FIM_SE) {
                    obterProximoToken();
                } else {
                    erro("fim_se", "condicional");
                }
            } else {
                erro("entao", "comandos");
                comandos();
                else_part();
                if (tokenAtual.nome_token == FIM_SE) {
                    obterProximoToken();
                } else {
                    erro("fim_se", "condicional");
                }
            }
        } else {
            erro(")", "condicional");
        }
    } else {
        erro("(", "expressao");
        expressao();
    }
}

// Função para tratar a parte do "senao" (opcional)
void else_part() {
    if (tokenAtual.nome_token == SENAO) {
        obterProximoToken();
        comandos();  // Analisa os comandos do "senao"
    } else {
        erro("senao", "comandos");
    }
}

// Função para tratar um laço
void laco() {
    obterProximoToken();  // Avança para o parêntese de abertura
    if (lexema_aux == "(") {
        obterProximoToken();
        expressao();  // Analisa a expressão da condição do laço
        if (lexema_aux == ")") {
            obterProximoToken();
            if (tokenAtual.nome_token == FACA) {
                obterProximoToken();
                comandos();  // Analisa os comandos dentro do laço
                if (tokenAtual.nome_token == FIM_ENQUANTO) {
                    obterProximoToken();
                } else {
                    erro("fim_enquanto", "laco");
                }
            } else {
                erro("faca", "comandos");
                comandos();
            }
        } else {
            erro(")", "laco"); // encontrar delimitador de laco
        }
    } else {
        erro("(", "expressao");
        expressao();
    }
}

// Função para tratar entrada/saída
void entrada_saida() {
    int acao = tokenAtual.nome_token;  // "leia" ou "escreva"
    obterProximoToken();  // Avança para o parêntese de abertura
    if (lexema_aux == "(") {
        obterProximoToken();
        if (acao == LEIA && tokenAtual.nome_token == ID) {
            cout << "Lendo valor de " << lexema_aux << endl;
            obterProximoToken();
        }else if (acao == ESCREVA) {
            expressao();  // Analisa a expressão a ser escrita
        } else {
            erro("ID ou expressão", "entrada_saida");
        }
        if (lexema_aux == ")") {
            obterProximoToken();
        } else {
            erro(")", "entrada_saida");
        }
    } else {
        if(acao == ESCREVA) {
            erro("(", "expressao");
        } else {
            erro("(", "entrada_saida");
        }
    }
}

// Função para tratar uma expressão
void expressao() {
    termo();  // A expressão começa com um termo
    expressao_opcional();  // Analisa a parte opcional da expressão
}

// Função para tratar a parte opcional de uma expressão (operação de soma ou subtração)
void expressao_opcional() {
    while (lexema_aux == "+" || lexema_aux == "-") {
        string operador = lexema_aux;
        obterProximoToken();
        termo();  // Analisa o próximo termo
        cout << "Operador: " << operador << endl;
    }
}

// Função para tratar um termo
void termo() {
    fator();  // Um termo começa com um fator
    termo_opcional();  // Analisa a parte opcional do termo
}

// Função para tratar a parte opcional de um termo (operação de multiplicação ou divisão)
void termo_opcional() {
    while (lexema_aux == "*" || lexema_aux == "/") {
        string operador = lexema_aux;
        obterProximoToken();
        fator();  // Analisa o próximo fator
        cout << "Operador: " << operador << endl;
    }
}

// Função para tratar um fator
void fator() {
    if (tokenAtual.nome_token == ID || tokenAtual.nome_token == NUM_INTEIRO || tokenAtual.nome_token == NUM_FLOAT || tokenAtual.nome_token == STRING) {
        cout << "Fator: " << lexema_aux << endl;
        obterProximoToken();
    } else if (lexema_aux == "verdadeiro" or lexema_aux == "falso") {
        expressao_bool();  // Trata o valor booleano
    } else if (lexema_aux == "(") {
        obterProximoToken();
        expressao();  // Analisa a expressão dentro dos parênteses
        if (lexema_aux == ")") {
            obterProximoToken();
        } else {
            erro(")", "fator");
        }
    } else {
        erro("ID, NUM, verdadeiro, falso ou expressão entre parênteses", "fator");
    }
}

// Função para tratar a expressão booleana (verdadeiro ou falso)
void expressao_bool() {
    if (lexema_aux == "verdadeiro") {
        cout << "Valor booleano: verdadeiro" << endl;
        obterProximoToken();
    } else if (lexema_aux == "falso") {
        cout << "Valor booleano: falso" << endl;
        obterProximoToken();
    } else {
        erro("verdadeiro ou falso", "expressao_bool");
    }
}
