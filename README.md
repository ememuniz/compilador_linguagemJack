# Analisador Léxico (JackTokenizer)

**Autor:** Emerson Paulo Pinheiro Muniz  
**Matrícula:** 20250013523  

## 💻 Sobre o Projeto

Este projeto é a implementação do **Analisador Léxico (Tokenizer)** para a linguagem Jack, correspondente à primeira etapa de construção do compilador do curso *Nand2Tetris* (Projeto 10). 

O objetivo deste programa é ler arquivos de código-fonte (`.jack`), ignorar ruídos (como espaços em branco, quebras de linha e comentários) e extrair os **tokens** da linguagem, classificando-os semanticamente em cinco categorias:
* *Keywords* (Palavras-chave)
* *Symbols* (Símbolos)
* *Identifiers* (Identificadores)
* *Integer Constants* (Constantes Inteiras)
* *String Constants* (Constantes de Texto)

Ao final do processamento, o programa gera um arquivo `.xml` estruturado com as tags correspondentes a cada token encontrado.

---

## 🚀 Execução Principal (`main.cpp`)

O programa principal é responsável por ler o arquivo `.jack` e gerar o arquivo `.xml` final.

### Como Compilar e Executar

**1. Compilação:**
No terminal, dentro da pasta do projeto, execute:
```bash
g++ main.cpp JackTokenizer.cpp -o JackAnalyzer
```

**2. Execução (exemplo com o arquivo Main.jack):**

Para rodar o analisador, passe o nome do arquivo .jack que você deseja traduzir como argumento.
Por exemplo, para processar um arquivo chamado Main.jack:

No Windows:

```bash
./JackAnalyzer.exe Main.jack
```

No Linux/Mac:
```bash
./JackAnalyzer Main.jack
```

Saída Esperada no Terminal:

```plaintext
Sucesso! Arquivo gerado: MainT.xml
```

Se o processo for bem sucedido o programa criará um arquivo chamado `MainT.xml` na pasta do projeto, contendo todos os tokens extraídos e formatados.


## 🧪 Estrutura de Testes Automatizados
Este projeto utiliza o framework doctest para garantir a confiabilidade do código em diferentes níveis. Os testes foram divididos em dois arquivos separados para facilitar a organização.

### 1. Testes de Unidade (test.cpp)
O que faz: Este arquivo testa a "engrenagem" do sistema, ou seja, as funções internas da classe JackTokenizer. Ele verifica se o código consegue avançar corretamente pelos caracteres, pular comentários (de linha // e de bloco /**/), ignorar espaços e identificar o tipo exato de cada token isoladamente.

**Como compilar e rodar:**

```bash
# Compilação
g++ test.cpp JackTokenizer.cpp -o TesteUnidade

# Execução (Windows)
./TesteUnidade.exe

# Execução (Linux/Mac)
./TesteUnidade
```

**Saída Esperada no Terminal:**

```plaintext
[doctest] doctest version is "2.4.11"
[doctest] run with "--help" for options
===============================================================================
[doctest] test cases:  X |  X passed | 0 failed | 0 skipped
[doctest] assertions:  Y |  Y passed | 0 failed |
[doctest] Status: SUCCESS!
```

### 2. Testes de Comparação / Fim a Fim (test_comparacao.cpp)
O que faz: Este arquivo atua como o corretor final. Ele não olha para a lógica interna, mas sim para o resultado prático. O teste abre os arquivos .xml gerados pelo JackAnalyzer, limpa as diferenças de quebra de linha (Windows vs Linux) e compara o conteúdo exato com os arquivos `.xml` oficiais de gabarito fornecidos pelo curso Nand2Tetris. Se o XML gerado for 100% idêntico ao gabarito, o teste passa.

Pré-requisito: Antes de rodar este teste, certifique-se de que o programa principal já processou os arquivos `.jack` e gerou os respectivos arquivos `*T.xml` na pasta raiz, e que os gabaritos oficiais estão dentro da pasta `originais/`.

**Como compilar e rodar:**

```bash
# Compilação
g++ test_comparacao.cpp -o TesteComparacao

# Execução (Windows)
./TesteComparacao.exe

# Execução (Linux/Mac)
./TesteComparacao
```
