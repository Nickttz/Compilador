# Projeto de compilador
⚠️ **Atenção:** O projeto está incompleto, não há a análise semântica e apresenta alguns bugs que precisam ser corrigidos  

O projeto consiste em dois analisadores: léxico e sintático.
- *Analisador Léxico:* Responsável pelo fornecimento dos tokens
- *Analisador Sintático:* Responsável pela gramática da linguagem  

O analisador léxico é estruturado com base em autômatos finitos, e o analisador sintático com procedimentos de cada expressão da gramática abaixo

## Gramática
A estrutura da gramática é:
  
`<programa>` ::= início `<declarações>` `<comandos>` fim  
`<declarações>` ::= var `<declaração_lista>` | ε  
`<declaração_lista>` ::= `<declaração>` `<declaração_lista_auxiliar>`  
`<declaração_lista_auxiliar>` ::= ; `<declaração_lista>` |  ε  
`<declaração>` ::= `<tipo>` `<id_lista>`  
`<tipo>` ::= int | real | bool  
`<id_lista>` ::= ID `<id_lista_auxiliar>`  
`<id_lista_auxiliar>` ::= , `<id_lista>` | ε  
`<comandos>` ::= `<comando_lista>`  
`<comando_lista>` ::= `<comando>` `<comando_lista_auxiliar>`  
`<comando_lista_auxiliar>` ::= ; `<comando_lista>` |  ε  
`<comando>` ::= `<atribuição>` | `<condicional>` | `<laço>` | `<entrada_saida>`  
`<atribuição>` ::= ID = `<expressão>`  
`<condicional>` ::= se ( `<expressão>` ) entao `<comandos>` `<else_part>` fim_se  
`<else_part>` ::= senao `<comandos>` | ε  
`<laço>` ::= enquanto ( `<expressão>` ) faca `<comandos>` fim_enquanto  
`<entrada_saida>` ::= leia ( ID ) | escreva ( `<expressão>` )  
`<expressão>` ::= `<termo>` `<expressao_opcional>`  
`<expressao_opcional>` ::= + `<expressão>` | - `<expressão>` | ε  
`<termo>` ::= `<fator>` `<termo_opcional>`  
`<termo_opcional>` ::= * `<termo>`| / `<termo>` | ε  
`<fator>` ::= ID | NUM | STRING | `<bool>` | (`<expressão>` )  
`<bool>` ::= verdadeiro | falso  

# Estrutura Geral
A análise sintática implementada segue o modelo de um analisador preditivo recursivo. Cada não terminal da gramática é representado por uma função recursiva que valida se a entrada respeita as regras da linguagem. O controle de erros é gerenciado pelo modo pânico, permitindo que o analisador continue analisando o restante do código mesmo após encontrar um erro.  

## Componentes Principais

### Token Atual e lexema_aux
- O token atual (tokenAtual) é a estrutura que contém o tipo e o valor do token lido do analisador léxico.
- A variável lexema_aux armazena o valor textual do token e é preenchida pela função proximo_token.(lexema_aux recebe o valor textual armazenado em lexema, que é uma variável contida dentro da função proximo_token)
### Conjunto FIRST 
- Um mapeamento explícito da gramática, associando os não terminais aos seus conjuntos FIRST. É utilizado na recuperação de erros para decidir o ponto de reentrada na análise.
### Função de Erro com Recuperação
- A função erro é chamada quando um token inesperado é encontrado.
- A recuperação no modo pânico avança os tokens até encontrar um delimitador (;) ou um token pertencente ao conjunto FIRST do contexto atual.
### Conjunto DELIMITADORES
- Um mapeamento explícito da gramática, associando os não-terminais aos seus conjuntos DELIMITADORES. É utilizado na recuperação de erros para decidir o ponto de reentrada na análise.
## Descrição das Funções
### Funções Gerais
- **obterProximoToken():** Obtém o próximo token chamando o analisador léxico, limpando o lexema_aux para garantir que o novo valor seja atualizado.
- **erro(string esperado, string contexto):** Exibe mensagens de erro indicando o token esperado e tenta a recuperação no modo pânico. A análise continuará a partir do próximo ponto válido.
### Funções Sintáticas
- **programa():** Inicia a análise sintática validando a estrutura principal do programa. Espera encontrar a palavra-chave inicio, seguida pelas declarações e comandos, finalizando com fim.
- **declaracoes(), declaracao_lista() e declaracao_lista_auxiliar():** Lidam com a análise das declarações de variáveis. A função declaracoes valida se o bloco começa com a palavra-chave var, e declaracao_lista trata as declarações individuais, incluindo o tipo e os identificadores.
- **tipo():** Verifica os tipos permitidos (int, real ou bool) e avança para a análise dos identificadores.
- **id_lista() e id_lista_auxiliar():** Analisa uma lista de identificadores, separados por vírgulas.
- **comandos(), comandos_lista() e comandos_lista_auxiliar():** Validam o bloco de comandos, sendo cada comando analisado pela função comando.
- **comando():** Determina qual tipo de comando está sendo analisado (atribuição, condicional, laço ou entrada/saída) e chama a função correspondente.
### Funções Específicas de Comandos
- **atribuicao():** Valida uma atribuição no formato ID = expressão. Gera erro se o operador de atribuição (=) estiver ausente.
- **condicional() e else_part():** Tratam comandos condicionais. A função condicional valida a sintaxe do se e, opcionalmente, chama else_part para tratar o bloco do senao.
- **laco():** Analisa laços no formato enquanto (condição) faca comandos fim_enquanto.
- **entrada_saida():** Lida com os comandos leia e escreva. No caso de leia, espera um identificador; em escreva, valida uma expressão.
### Funções para Expressões e Termos
- **expressao():** Analisa uma expressão que pode conter operações de soma/subtração e chama as funções para tratar termos.
- **expressao_opcional():** Trata operadores de soma ou subtração que seguem um termo, permitindo a análise de expressões completas.
- **termo() e termo_opcional():** Seguem o mesmo modelo de expressao e expressao_opcional, mas para operações de multiplicação ou divisão.
- **fator():** Trata os componentes básicos de uma expressão, como identificadores, números, valores booleanos e expressões entre parênteses.
- **expressao_bool():** Verifica se o valor booleano é verdadeiro ou falso.
### Fluxo de Controle
- O analisador é iniciado pela função programa().
- As funções são chamadas recursivamente para analisar cada parte da entrada, seguindo a estrutura da gramática.
- Em caso de erro, a análise continua no próximo ponto válido, graças ao modo pânico.

# Testes e Resultados
## teste 1:
```c
inicio  
    x = 5;  
    escreva(x);  
fim
````

## Saída:
```c
Fator: 5  
Fator: x    
Análise sintática concluída com sucesso!  
```
## Descrição:
- O programa começa com inicio e termina com fim, delimitando o bloco principal.
- A atribuição x = 5 é validada corretamente, identificando 5 como um fator.
- O comando escreva(x) analisa x como fator, gerando a saída correspondente.

## teste 2:
```c
inicio
    var int x, y;
    x = 10;
    y = 20;
    escreva(x + y);
fim
```

## Saída:
```c
Tipo: int
Declarando a variável x
Declarando a variável y
Fator: 10
Fator: 20
Fator: x
Fator: y
Operador: +
Análise sintática concluída com sucesso!
```

## Descrição:
- A declaração das variáveis x e y do tipo int é reconhecida.
- As atribuições x = 10 e y = 20 são validadas, identificando 10 e 20 como fatores.
- A expressão x + y é corretamente analisada, identificando os fatores x e y e o operador +.

## teste 3:
```c
inicio
    var bool expressao;
    expressao = verdadeiro;
    enquanto (expressao) faca
    escreva("Expressao verdadeira");
    fim_enquanto
fim
```
## Saída:
```c
Tipo: bool
Declarando a variável expressao
Valor booleano: verdadeiro
Fator: expressao
Fator: Expressao verdadeira
Análise sintática concluída com sucesso!
```
## Descrição:
- A declaração da variável expressao do tipo bool é reconhecida.
- A atribuição expressao = verdadeiro identifica verdadeiro como um valor booleano válido.
- O laço enquanto valida a condição expressao e analisa o comando escreva("Expressao verdadeira"), reconhecendo o texto como fator.

## teste 4:
```c
inicio
    var bool condicao;
    condicao = verdadeiro;
    se (condicao) entao
        escreva("Condição verdadeira");
    senao
        escreva("Condição falsa");
    fim_se
fim
```
## Saída:
```c
Tipo: bool
Declarando a variável condicao
Valor booleano: verdadeiro
Fator: condicao
Fator: Condição verdadeira
Fator: Condição falsa
Análise sintática concluída com sucesso!
```
## Descrição:
- A declaração da variável condicao do tipo bool é reconhecida.
- A atribuição condicao = verdadeiro identifica verdadeiro como um valor booleano válido.
- O comando condicional se valida a condição condicao e analisa os blocos entao e senao, reconhecendo os textos nos comandos escreva como fatores.

## teste 5:
```c
inicio
	var bool condicao;
	condicao = verdadeiro;
	se (condicao) entaao
    	    escreva("Condição verdadeira");
	senao
    	escreva("Condição falsa");
	fim_se
fim
```
## Saída:
```c
Tipo: bool
Declarando a variável condicao
Valor booleano: verdadeiro
Fator: condicao
Erro sintático: esperado entao, mas encontrado entaao
Avançando para o próximo token: escreva
Recuperado no contexto 'comandos' com token do FIRST: escreva
Fator: Condição verdadeira
Fator: Condição falsa
Análise sintática concluída com sucesso!
```

## Descrição:
- A declaração da variável condicao do tipo bool é reconhecida.
- A atribuição condicao = verdadeiro identifica verdadeiro como um valor booleano válido.
- O comando condicional se valida a condição condicao e analisa os blocos entao e senao. Porém, como código possui erro de sintaxe, tendo a palavra reservada escrita ‘’entaao’’ ao invés de ‘’entao’’, o analisador sintático identifica o erro e recupera com o modo pânico utilizando o conjunto FIRST
- Os textos nos comandos escreva são reconhecidos como fatores.

