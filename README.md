# Front-End do Compilador Jack (Analisador Léxico e Sintático)
Autor: Emerson Paulo Pinheiro Muniz

Matrícula: 20250013523

## 💻 Sobre o Projeto
Este projeto é a implementação do Front-End completo do compilador para a linguagem Jack, correspondente às etapas de Análise Léxica e Análise Sintática da disciplina de Compiladores do curso de Engenharia da Computação da Universidade Federal do Maranhão (UFMA).

O processo é dividido em duas grandes engrenagens principais:

### 1. Analisador Léxico (`JackTokenizer`)
O objetivo desta etapa é ler arquivos de código-fonte (`.jack`), ignorar ruídos (como espaços em branco, quebras de linha e comentários) e extrair os tokens da linguagem, classificando-os semanticamente em cinco categorias fundamentais:

`<keyword>` (Palavras-chave, ex: `class`, `if`)

`<symbol>` (Símbolos, ex: `{`, `+`)

`<identifier>` (Identificadores de variáveis, funções ou classes)

`<integerConstant>` (Constantes Inteiras, `ex: 15`)

`<stringConstant>` (Constantes de Texto)

O resultado desta etapa é salvo em um arquivo com o sufixo `*T.xml`.

### 2. Analisador Sintático (`CompilationEngine`)
O Parser recebe os tokens gerados na etapa anterior e verifica se eles formam frases válidas de acordo com a Gramática Livre de Contexto da linguagem Jack. O resultado é a geração de uma Árvore Sintática (AST) no formato XML, que descreve a estrutura hierárquica do código.

As tags geradas pelo analisador sintático agrupam os tokens em estruturas lógicas (Não-Terminais):

Estrutura do Programa: `<class>`, `<classVarDec>`, `<subroutineDec>`, `<parameterList>`, `<subroutineBody>`, `<varDec>`.

Comandos (Statements): `<statements>` (que agrupa os comandos de um bloco), `<letStatement>` (atribuição), `<ifStatement>` (condicional), `<whileStatement>` (laço de repetição), `<doStatement>` (chamada de função/método), `<returnStatement>` (retorno).

Expressões Matemáticas/Lógicas: `<expression>`, `<term>` (operandos, que podem incluir chamadas de função ou variáveis), `<expressionList>` (argumentos passados para uma função).

O resultado desta etapa é salvo em um arquivo com o sufixo `*P.xml`.

## 📁 Estrutura de Pastas
Para manter a organização do código (padrão profissional em C++), o projeto está estruturado da seguinte forma:

`/src`: Contém os códigos-fonte (`.cpp`).

`/include`: Contém os cabeçalhos das classes (`.h`).

`/gabarito`: Contém os arquivos XML originais fornecidos pelo curso Nand2Tetris para validação.

Raiz do projeto: Arquivos de teste da linguagem (`.jack`) e executáveis.

## 🚀 Execução Principal (`main.cpp`)
O programa principal é responsável por ler o arquivo `.jack` na pasta raiz e gerar ambos os arquivos XML (Léxico e Sintático).

Como Compilar e Executar
### 1. Compilação:
No terminal, dentro da raiz do projeto, execute:
```Bash
g++ src/main.cpp src/JackTokenizer.cpp src/CompilationEngine.cpp -Iinclude -o JackAnalyzer
```

(A flag `-Iinclude` avisa ao compilador para procurar os arquivos `.h` dentro da pasta `include`)

### 2. Execução (exemplo com o arquivo `Main.jack`):
Para rodar o analisador, passe o nome do arquivo .jack que você deseja traduzir como argumento.

No Windows:
```DOS
.\JackAnalyzer.exe Main.jack
```
No Linux/Mac:
```Bash
./JackAnalyzer Main.jack
```
Saída Esperada no Terminal:

```Plaintext
Sucesso! Arquivo lexico gerado: MainT.xml
Sucesso! Arquivo sintatico gerado: MainP.xml
```
## 🧪 Estrutura de Testes Automatizados
Este projeto utiliza o framework doctest para garantir a confiabilidade do código.

### 1. Testes de Comparação XML Fim a Fim (test_comparacao.cpp)
Este teste atua como o corretor automático final. Ele abre os arquivos gerados pelo seu compilador (`*T.xml` e `*P.xml`), remove eventuais problemas de formatação de quebra de linha (Windows vs Linux) e compara o conteúdo com os gabaritos oficiais da pasta `/gabarito`.

⚠️ Pré-requisito: Antes de rodar este teste, você deve executar o programa principal (`JackAnalyzer`) para que os arquivos XML sejam gerados na raiz do projeto.

Como compilar e rodar:

```Bash
# Compilação
 g++ .\tests\test_comparacao.cpp -o TesteComparacao.exe

# Execução (Windows)
.\TesteComparacao.exe

# Execução (Linux/Mac)
./TesteComparacao
```
Saída Esperada no Terminal:

```Plaintext
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  2 |  2 passed | 0 failed | 0 skipped
[doctest] assertions:  6 |  6 passed | 0 failed |
[doctest] Status: SUCCESS!
```
### 2. Testes de Unidade Internos 
Verifica as "engrenagens" internas da classe JackTokenizer, testando se ela consegue avançar corretamente pelos caracteres, pular comentários isolados e ignorar espaços brancos.

Bash
# Compilação
```
g++ -Iinclude .\tests\test_parser.cpp .\src\JackTokenizer.cpp .\src\CompilationEngine.cpp -o TesteParser
g++ -Iinclude .\tests\test_scanner.cpp .\src\JackTokenizer.cpp .\src\CompilationEngine.cpp -o TesteScanner
```
# Execução
```
.\TesteParser.exe 
.\TesteScanner.exe
```