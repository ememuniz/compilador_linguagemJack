#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"

std::string TESTFILE = "test_codigo.jack";
std::string TESTSYMBOL = "test_symbol.jack";

//Teste para verificar se o tokenizer leu o arquivo certo
TEST_CASE("Testando a leitura de arquivos"){
  JackTokenizer tokenizer(TESTFILE); //Cria um tokenizer com o arquivo de teste
  CHECK(tokenizer.getCode() == "class Main{ }"); //Verifica se o tokenizer leu o arquivo certo
}

//Teste para verificar se os simbolos estão sendo lidos corretamente
TEST_CASE("Testando o reconhecimento de Simbolos"){
  JackTokenizer tokenizer("test_symbol.jack");  //Cria um tokenizer com o arquivo de teste
  CHECK(tokenizer.hasMoreTokens() == true);     //Verifica se o texto foi lido todo.  

  tokenizer.advance();                          //Avança para o primeiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "{");           //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == SYMBOL);       //Verifica se o token lido foi um simbolo

  tokenizer.advance();                          //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "}");           //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == SYMBOL);       //Verifica se o token lido foi um simbolo
}