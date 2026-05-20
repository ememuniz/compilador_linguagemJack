#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "VMWriter.h"
#include "SymbolTable.h"
#include <fstream>
#include <string>

//CONTAINER MARK: COMANDOS BÁSICOS NA MÁQUINA VIRTUAL
TEST_CASE("VMWriter - Validação dos comandos básicos da máquina virtual") {
  std::string arquivoTeste = "teste_doctest.vm";
  {
    VMWriter writer(arquivoTeste);                        // Cria um VMWriter para o arquivo de teste
    writer.writePush(Segment::CONSTANT, 5);               // Escreve um comando push constant 5, que empilha o valor 5 na pilha
    writer.writePop(Segment::LOCAL, 1);                   // Escreve um comando pop local 1, que desempilha o valor 5 da pilha e armazena na variável local 1
    writer.writeArithmetic(Command::ADD);                 // Escreve um comando aritmético add, que realiza a soma dos dois valores na pilha
    writer.writeCall("Math.abs", 1);                      // Escreve um comando de chamada de função, que chama a função Math.abs com um argumento
    writer.writeReturn();                                 // Escreve um comando de retorno, que retorna o valor da pilha para a pilha de chamada
  }

  std::ifstream in(arquivoTeste);                         // Abre o arquivo de teste para leitura
  REQUIRE(in.is_open());                                  // Verifica se o arquivo foi aberto corretamente

  std::string linha;                                      // Variável para armazenar cada linha do arquivo
  
  std::getline(in, linha);                                // Lê a primeira linha do arquivo
  CHECK(linha == "push constant 5");                      // Verifica se a primeira linha é o comando push constant 5

  std::getline(in, linha);                                // Lê a segunda linha do arquivo
  CHECK(linha == "pop local 1");                          // Verifica se a segunda linha é o comando pop local 1

  std::getline(in, linha);                                // Lê a terceira linha do arquivo
  CHECK(linha == "add");                                  // Verifica se a terceira linha é o comando aritmético add

  std::getline(in, linha);                                // Lê a quarta linha do arquivo
  CHECK(linha == "call Math.abs 1");                     // Verifica se a quarta linha é o comando de chamada de função call Math.abs 1

  std::getline(in, linha);                                // Lê a quinta linha do arquivo
  CHECK(linha == "return");                               // Verifica se a quinta linha é o comando de retorno return
}

//CONTAINER MARK: GERENCIAMENTO DE ESCOPOS E INDICES
TEST_CASE("SymbolTable - Gerenciamento de escopos e indices") {
  SymbolTable table;

  table.define("subtotal", "int", Kind::STATIC);           // Define uma variável static chamada subtotal do tipo int, que é compartilhada por todas as instâncias da classe
  table.define("ax", "int", Kind::FIELD);                  // Define uma variável field chamada ax do tipo int, que é específica para cada instância da classe
  table.define("ay", "int", Kind::FIELD);                  // Define uma variável field chamada ay do tipo int, que é específica para cada instância da classe

  CHECK(table.varCount(Kind::STATIC) == 1);              // Verifica se há 1 variável static definida
  CHECK(table.varCount(Kind::FIELD) == 2);               // Verifica se há 2 variáveis field definidas

  CHECK(table.kindOf("subtotal") == Kind::STATIC);       // Verifica se a variável subtotal é do tipo static
  CHECK(table.indexOf("subtotal") == 0);                 // Verifica se o índice da variável subtotal é 0
  CHECK(table.typeOf("subtotal") == "int");              // Verifica se o tipo da variável subtotal é int

  CHECK(table.kindOf("ax") == Kind::FIELD);              // Verifica se a variável ax é do tipo field
  CHECK(table.indexOf("ax") == 0);                       // Verifica se o índice da variável ax é 0
  CHECK(table.indexOf("ay") == 1);                       // Verifica se o índice da variável ay é 1

  table.startSubroutine();                               // Inicia uma nova subrotina, limpando as variáveis de subrotina e reiniciando os contadores de arg e local
  table.define("this", "Square", Kind::ARG);             // Define uma variável arg chamada this do tipo Square, que é um argumento da subrotina
  table.define("x", "int", Kind::LOCAL);                  // Define uma variável local chamada x do tipo int, que é específica para a subrotina
  table.define("y", "int", Kind::LOCAL);                  // Define uma variável local chamada y do tipo int, que é específica para a subrotina

  CHECK(table.varCount(Kind::ARG) == 1);                  // Verifica se há 1 variável arg definida
  CHECK(table.varCount(Kind::LOCAL) == 2);                // Verifica se há 2 variáveis local definidas

  CHECK(table.kindOf("this") == Kind::ARG);               // Verifica se a variável this é do tipo arg
  CHECK(table.indexOf("this") == 0);                      // Verifica se o índice da variável this é 0
  CHECK(table.kindOf("x") == Kind::LOCAL);                // Verifica se a variável x é do tipo local
  CHECK(table.indexOf("x") == 0);                         // Verifica se o índice da variável x é 0
  CHECK(table.indexOf("y") == 1);                         // Verifica se o índice da variável y é 1

  CHECK(table.kindOf("ax") == Kind::FIELD);               // Verifica se a variável ax ainda é do tipo field, confirmando que as variáveis de classe permanecem acessíveis mesmo dentro de uma subrotina
  CHECK(table.indexOf("ax") == 0);                        // Verifica se o índice da variável ax ainda é 0, confirmando que as variáveis de classe mantêm seus índices mesmo dentro de uma subrotina

  table.startSubroutine();                               // Inicia outra subrotina, limpando as variáveis de subrotina e reiniciando os contadores de arg e local (Deve limpar o escopo local [x, y e this] e manter os escopos de classe [subtotal, ax e ay] )
  CHECK(table.varCount(Kind::LOCAL) == 0);                // Verifica se não há mais variáveis local definidas, confirmando que o escopo local foi limpo
  CHECK(table.varCount(Kind::ARG) == 0);                  // Verifica se não há mais variáveis arg definidas, confirmando que o escopo local foi limpo
  CHECK(table.kindOf("x") == Kind::NONE);                 // Verifica se a variável x nao existe, confirmando que o escopo local foi limpo

  CHECK(table.kindOf("ay") == Kind::FIELD);               // Verifica se a variável ay ainda é do tipo field, confirmando que as variáveis de classe permanecem acessíveis mesmo dentro de uma subrotina
  CHECK(table.indexOf("ay") == 1);                        // Verifica se o índice da variável ay ainda é 1, confirmando que as variáveis de classe mantêm seus índices mesmo dentro de uma subrotina
}

