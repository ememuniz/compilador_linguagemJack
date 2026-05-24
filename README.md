# Front-End do Compilador Jack (Analisador Léxico e Sintático)
Autor: Emerson Paulo Pinheiro Muniz

Matrícula: 20250013523

## 💻 Sobre o Projeto
Este projeto é a implementação do Front-End completo do compilador para a linguagem Jack, correspondente às etapas de Análise Léxica,Análise Sintática e Geração de código intermediário da disciplina de Compiladores do curso de Engenharia da Computação da Universidade Federal do Maranhão (UFMA).

O processo é dividido em três grandes engrenagens principais:

### 1. Analisador Léxico (`JackTokenizer`)
O objetivo desta etapa é ler arquivos de código-fonte (`.jack`), ignorar ruídos (como espaços em branco, quebras de linha e comentários) e extrair os tokens da linguagem, classificando-os semantic  amente em cinco categorias fundamentais:

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

### 3. Gerador de Código Intermediário (`CodeWriter`)
Esta é a etapa de Back-End do compilador. Em vez de gerar apenas XML, o compilador agora traduz a lógica da linguagem Jack para a linguagem intermediária da máquina virtual (VM) do Nand2Tetris baseada em pilha. Para isso, utiliza dois componentes vitais:

* **Tabela de Símbolos (`SymbolTable`):** Gerencia o escopo e o tempo de vida das variáveis. Ela lembra se uma variável é um campo da classe (`FIELD`), estática (`STATIC`), local da função (`LOCAL`) ou um argumento (`ARG`), e atribui um índice numérico a cada uma.
* **Escritor VM (`VMWriter`):** Recebe os comandos traduzidos pela `CompilationEngine` e escreve as instruções finais (ex: `push constant 5`, `call Math.multiply 2`, `pop local 0`).

O resultado final desta etapa é salvo em um arquivo com o sufixo `.vm`, pronto para ser executado no VM Emulator.

## 📁 Estrutura de Pastas
Para manter a organização do código (padrão profissional em C++), o projeto está estruturado da seguinte forma:

* `/src`: Contém os códigos-fonte (`.cpp`).
* `/include`: Contém os cabeçalhos das classes (`.h`).
* `/gabarito`: Contém os arquivos originais (XML ou VM) fornecidos pelo curso Nand2Tetris para validação.
* `/tests`: Contém os arquivos de código para os testes automatizados.
* `/results`: contém os arquivos de testes para o gerador de códigos intermediários vm proposto pelo curso Nand2Tetris.

## 🚀 Execução Principal (`main.cpp`)
O programa principal é responsável por ler o arquivo `.jack` na pasta raiz e gerar ambos os arquivos XML (Léxico e Sintático).

Como Compilar e Executar
### 1. Compilação:
No terminal, dentro da raiz do projeto, execute:
```Bash
 g++ -std=c++17 src/main.cpp -Iinclude -o JackCompiler.exe 
```

(A flag `-Iinclude` avisa ao compilador para procurar os arquivos `.h` dentro da pasta `include`)

### 2. Execução:
Para rodar o analisador, passe o nome do diretorio que você deseja traduzir como argumento.

No Windows:
```DOS
.\JackCompiler.exe ./results/pong
```
No Linux/Mac:
```Bash
.\JackCompiler ./results/pong
```
Saída Esperada no Terminal:

```Plaintext
Total de ficheiros encontrados: 4

A processar: Ball.jack -> Ball.vm
A processar: Bat.jack -> Bat.vm
A processar: Main.jack -> Main.vm
A processar: PongGame.jack -> PongGame.vm

Processamento de lote concluido!
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
Verifica as "engrenagens" internas da classe JackTokenizer, testando se ela consegue avançar corretamente pelos caracteres, pular comentários isolados e ignorar espaços brancos. Também testa se o VMWritter e a tabela de simbolos está funcionando perfeitamente 

Bash
# Compilação
```
g++ -Iinclude .\tests\test_parser.cpp .\src\JackTokenizer.cpp .\src\CompilationEngine.cpp -o TesteParser
g++ -Iinclude .\tests\test_scanner.cpp .\src\JackTokenizer.cpp .\src\CompilationEngine.cpp -o TesteScanner
g++ tests/test_vmwriter.cpp src/VMWriter.cpp src/SymbolTable.cpp  -Iinclude -o TesteVM.exe
```
# Execução
```
.\TesteParser.exe 
.\TesteScanner.exe
.\TesteVM.exe
```