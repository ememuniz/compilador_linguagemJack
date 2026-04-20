#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <fstream>

TEST_CASE("Testando ferramentas auxiliares do Compilation Engine") {

  //#region MARK: PREPARAÇÃO DO ARQUIVO DE TESTE
  std::ofstream out("temp_test.jack");                      //Cria o arquivo de teste
  out << "class Main{ }";                                   //Escreve no arquivo
  out.close();                                              //Fecha o arquivo
  //#endregion

  //region MARK: INICIA O SCANNER E O PARSER
  JackTokenizer tokenizer("temp_test.jack");                //Cria um tokenizer com o arquivo de teste
  CompilationEngine engine(tokenizer, "temp_test.xml");     //Cria um engine com o tokenizer e o arquivo de saída, uma engine é um parser
  //#endregion

  //region MARK: TESTE DO MATCH()
  SUBCASE("Verifica se match() funciona corretamente"){
    CHECK(engine.match(KEYWORD) == true);                   //Verifica se a keyword foi lida
    CHECK(engine.match(KEYWORD, "class") == true);          //Verifica se a keyword "class" foi lida
    CHECK(engine.match(SYMBOL) == false);                   //Verifica se o simbolo foi lido
  }
  //endregion

  //region MARK: TESTE DO CONSUME()
  SUBCASE("Verifica se consume() consome o token correto e lança erro no incorreto"){
    CHECK_NOTHROW(engine.consume(KEYWORD, "class"));        //Verifica se "class" é consumida
    CHECK(engine.match(IDENTIFIER, "Main") == true);        //O token atual deve ser 'Main
    CHECK_THROWS_AS(engine.consume(SYMBOL, "{"), std::runtime_error); //Se um símbolo for consumido agora, é lançado um erro
  }
  //endregion

  //region MARK: TESTE DO GERA XML - CLASSE VAZIA
  SUBCASE("Gera XML correto para uma declaração de classe vazia"){
    std::ofstream outJack("temp_class.jack");
    outJack << "class Main{ }";
    outJack.close();

    JackTokenizer tk("temp_class.jack");
    CompilationEngine eng(tk, "temp_class.xml");
    eng.compileClass();

    std::ifstream xmlIn("temp_class.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();
    std::string xmlGerado = buffer.str();

    std::string xmlEsperado = 
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";

    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion

}