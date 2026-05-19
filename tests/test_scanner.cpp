#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"

//CONTAINER MARK: VARIÁVEIS DE TESTE
std::string TESTFILE = "files/test_codigo.jack";
std::string TESTSYMBOL = "files/test_symbol.jack";
std::string TESTNUMBER = "files/test_number.jack";
std::string TESTSTRING = "files/test_string.jack";
std::string TESTWORDS = "files/test_words.jack";
std::string TESTCOMMENTS = "files/test_comentarios.jack";

//CONTAINER MARK: TESTE DE LEITURA DE ARQUIVOS
TEST_CASE("Testando a leitura de arquivos"){
  JackTokenizer tokenizer(TESTFILE); //Cria um tokenizer com o arquivo de teste
  CHECK(tokenizer.hasMoreTokens() == true); //Verifica se o texto foi lido todo.
  CHECK(tokenizer.getToken() == "class");    //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == KEYWORD);   //Verifica se o token lido foi uma keyword
}

//CONTAINER MARK: TESTE DE RECONHECIMENTO DE SIMBOLOS
TEST_CASE("Testando o reconhecimento de Simbolos"){
  JackTokenizer tokenizer(TESTSYMBOL);          //Cria um tokenizer com o arquivo de teste
  CHECK(tokenizer.hasMoreTokens() == true);     //Verifica se o texto foi lido todo.  

  CHECK(tokenizer.getToken() == "{");           //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == SYMBOL); 
  
  tokenizer.advance();
  CHECK(tokenizer.getToken() == ">");
  CHECK(tokenizer.tokenType() == SYMBOL);

  tokenizer.advance();                          //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "}");           //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == SYMBOL);       //Verifica se o token lido foi um simbolo
}

//CONTAINER MARK: TESTE DE RECONHECIMENTO DE NÚMEROS
TEST_CASE("Testando o reconhecimento de Números"){
  JackTokenizer tokenizer(TESTNUMBER);         //Cria um tokenizer com o arquivo de teste
                        //Avança para o primeiro token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "123");        //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == INT_CONST);   //Verifica se o token lido foi um inteiro

  tokenizer.advance();                         //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "45");        //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == INT_CONST);   //Verifica se o token lido foi um inteiro
}

//CONTAINER MARK: TESTE DE RECONHECIMENTO DE STRINGS
TEST_CASE("Testando o reconhecimento de Strings"){
  JackTokenizer tokenizer(TESTSTRING);         //Cria um tokenizer com o arquivo de teste

  CHECK(tokenizer.getToken() == "ola mundo");  //Verifica se o primeiro token foi lido
  CHECK(tokenizer.tokenType() == STRING_CONST); //Verifica se o token lido foi uma string 

  tokenizer.advance();                         //Avança para o segundo token e armazena ele e seu tipo em suas respectivas variáveis
  CHECK(tokenizer.getToken() == "Jack");       //Verifica se o segundo token foi lido
  CHECK(tokenizer.tokenType() == STRING_CONST); //Verifica se o token lido foi uma string
}

//CONTAINER MARK: TESTE DE RECONHECIMENTO DE PALAVRAS E KEYWORDS
TEST_CASE("Testando o reconhecimento de Palavras e Keywords"){
  JackTokenizer tokenizer(TESTWORDS);          //Cria um tokenizer com o arquivo de teste
  
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

//CONTAINER MARK: TESTE DE REMOÇÃO DE COMENTÁRIOS
TEST_CASE("Testando a remoção de Comentários"){
  JackTokenizer tokenizer(TESTCOMMENTS);       //Cria um tokenizer com o arquivo de teste

  CHECK(tokenizer.getToken() == "class");      
  tokenizer.advance();
  CHECK(tokenizer.getToken() == "Main");
  tokenizer.advance();
  CHECK(tokenizer.getToken() == "{");
  tokenizer.advance();
  CHECK(tokenizer.getToken() == "}");
  tokenizer.advance();
  CHECK(tokenizer.hasMoreTokens() == false);
}