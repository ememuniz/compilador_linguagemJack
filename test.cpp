#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"

std::string TESTFILE = "test_codigo.jack";
std::string TESTSYMBOL = "test_symbol.jack";
std::string TESTNUMBER = "test_number.jack";
std::string TESTSTRING = "test_string.jack";
std::string TESTWORDS = "test_words.jack";

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
  CHECK(tokenizer.tokenType() == SYMBOL); 
  
  tokenizer.advance();
  CHECK(tokenizer.getToken() == ">");
  CHECK(tokenizer.tokenType() == SYMBOL);

  tokenizer.advance();                          //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "}");           //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == SYMBOL);       //Verifica se o token lido foi um simbolo
}

//Teste para verificar se os números estão sendo lidos corretamente
TEST_CASE("Testando o reconhecimento de Números"){
  JackTokenizer tokenizer(TESTNUMBER);         //Cria um tokenizer com o arquivo de teste

  tokenizer.advance();                         //Avança para o primeiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "123");        //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == INT_CONST);   //Verifica se o token lido foi um inteiro

  tokenizer.advance();                         //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "45");        //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == INT_CONST);   //Verifica se o token lido foi um inteiro
}

//Teste para verificar se as letras estão sendo lidas corretamente
TEST_CASE("Testando o reconhecimento de Strings"){
  JackTokenizer tokenizer(TESTSTRING);         //Cria um tokenizer com o arquivo de teste

  tokenizer.advance();                         //Avança para o primeiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "ola mundo");  //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == STRING_CONST); //Verifica se o token lido foi uma string 

  tokenizer.advance();                         //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "Jack");       //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == STRING_CONST); //Verifica se o token lido foi uma string
}

//Teste para verificar se as palavras chave e os identificadores estão sendo lidos corretamentes
TEST_CASE("Testando o reconhecimento de Palavras e Keywords"){
  JackTokenizer tokenizer(TESTWORDS);          //Cria um tokenizer com o arquivo de teste

  tokenizer.advance();                         //Avança para o primeiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "class");      //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == KEYWORD);     //Verifica se o token lido foi uma keyword

  tokenizer.advance();                         //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "var");        //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == KEYWORD);     //Verifica se o token lido foi uma keyword

  tokenizer.advance();                         //Avança para o terceiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "_minhaVariavel");       //Verifica se o terceiro token foi lido
  CHECK(tokenizer.tokenType() == IDENTIFIER);  //Verifica se o token lido foi um identificador

  tokenizer.advance();                         //Avança para o quarto token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "count123");   //Verifica se o quarto token foi lido
  CHECK(tokenizer.tokenType() == IDENTIFIER);  //Verifica se o token lido foi um identificador
}