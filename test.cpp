#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"

std::string TEST = "test_codigo.jack";

TEST_CASE("Testando a leitura de arquivos"){
  JackTokenizer tokenizer(TEST); //Cria um tokenizer com o arquivo de teste
  CHECK(tokenizer.getCode() == "class Main{ }"); //Verifica se o tokenizer leu o arquivo certo
}