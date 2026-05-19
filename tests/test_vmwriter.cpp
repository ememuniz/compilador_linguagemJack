#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "VMWriter.h"
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