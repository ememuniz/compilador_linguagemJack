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

    {
      JackTokenizer tk("temp_class.jack");
      CompilationEngine eng(tk, "temp_class.xml");
      eng.compileClass();
    }

    std::ifstream xmlIn("temp_class.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();
    std::string xmlGerado = buffer.str();

    std::string xmlLimpo = "";
    for (char c : xmlGerado) {
      if (c != '\r') xmlLimpo += c;
    }

    std::string xmlEsperado = 
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";

    CHECK(xmlLimpo == xmlEsperado);
  }
  //endregion

  //region MARK: TESTE DO GERA XML - CLASSE COM VARIAVEL
  SUBCASE("Gera XML para classe com variaveis de classe (classVarDec)") {
    std::ofstream outJack("temp_var.jack");
    // Uma classe com uma variavel estatica e duas de campo (field)
    outJack << "class Main { static int x; field boolean y, z; }";
    outJack.close();
    {
      JackTokenizer tk("temp_var.jack");
      CompilationEngine eng(tk, "temp_var.xml");
      eng.compileClass();
    }
    std::ifstream xmlIn("temp_var.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();
    std::string xmlGerado = "";
    for (char c : buffer.str()) {
      if (c != '\r') xmlGerado += c;
    }

    std::string xmlEsperado = 
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <classVarDec>\n"
      "    <keyword> static </keyword>\n"
      "    <keyword> int </keyword>\n"
      "    <identifier> x </identifier>\n"
      "    <symbol> ; </symbol>\n"
      "  </classVarDec>\n"
      "  <classVarDec>\n"
      "    <keyword> field </keyword>\n"
      "    <keyword> boolean </keyword>\n"
      "    <identifier> y </identifier>\n"
      "    <symbol> , </symbol>\n"
      "    <identifier> z </identifier>\n"
      "    <symbol> ; </symbol>\n"
      "  </classVarDec>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";
    
    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion

  //region MARK: TESTE DO GERA XML - CLASSE COM SUBROUTINE 
  SUBCASE("Gera XML para classe com uma subrotina (funcão) e parametros") {
    std::ofstream outJack("temp_sub.jack");
    // Uma classe com uma função que recebe dois parâmetros e tem corpo vazio
    // SubroutineDec: (constructor | function | method) + (void | tipo) + nomeDaSubrotina + "(" + parameterList + ")" + subroutineBody
    outJack << "class Main { function void main(int a, boolean b) { } }";
    outJack.close();

    {
      JackTokenizer tk("temp_sub.jack");
      CompilationEngine eng(tk, "temp_sub.xml");
      eng.compileClass();
    }

    std::ifstream xmlIn("temp_sub.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();

    std::string xmlGerado = "";
    for (char c : buffer.str()) {
      if (c != '\r') xmlGerado += c;
    }

    std::string xmlEsperado = 
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <subroutineDec>\n"
      "    <keyword> function </keyword>\n"
      "    <keyword> void </keyword>\n"
      "    <identifier> main </identifier>\n"
      "    <symbol> ( </symbol>\n"
      "    <parameterList>\n"
      "      <keyword> int </keyword>\n"
      "      <identifier> a </identifier>\n"
      "      <symbol> , </symbol>\n"
      "      <keyword> boolean </keyword>\n"
      "      <identifier> b </identifier>\n"
      "    </parameterList>\n"
      "    <symbol> ) </symbol>\n"
      "    <subroutineBody>\n"
      "      <symbol> { </symbol>\n"
      "      <statements>\n"
      "      </statements>\n"
      "      <symbol> } </symbol>\n"
      "    </subroutineBody>\n"
      "  </subroutineDec>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";

    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion

  //region MARK: TESTE DO GERA XML - CLASSE COM BODY COM VARIAVEIS E RETURN
  SUBCASE("Gera XML para variaveis locais (varDec) e comando return") {
    std::ofstream outJack("temp_stmts.jack");
    outJack << "class Main { function void main() { var int a, b; return; } }";
    outJack.close();

    {
      JackTokenizer tk("temp_stmts.jack");
      CompilationEngine eng(tk, "temp_stmts.xml");
      eng.compileClass();
    }

    std::ifstream xmlIn("temp_stmts.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();

    std::string xmlGerado = "";
    for (char c : buffer.str()) {
      if (c != '\r') xmlGerado += c;
    }

    std::string xmlEsperado = 
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <subroutineDec>\n"
      "    <keyword> function </keyword>\n"
      "    <keyword> void </keyword>\n"
      "    <identifier> main </identifier>\n"
      "    <symbol> ( </symbol>\n"
      "    <parameterList>\n"
      "    </parameterList>\n"
      "    <symbol> ) </symbol>\n"
      "    <subroutineBody>\n"
      "      <symbol> { </symbol>\n"
      "      <varDec>\n"
      "        <keyword> var </keyword>\n"
      "        <keyword> int </keyword>\n"
      "        <identifier> a </identifier>\n"
      "        <symbol> , </symbol>\n"
      "        <identifier> b </identifier>\n"
      "        <symbol> ; </symbol>\n"
      "      </varDec>\n"
      "      <statements>\n"
      "        <returnStatement>\n"
      "          <keyword> return </keyword>\n"
      "          <symbol> ; </symbol>\n"
      "        </returnStatement>\n"
      "      </statements>\n"
      "      <symbol> } </symbol>\n"
      "    </subroutineBody>\n"
      "  </subroutineDec>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";

    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion

  //region MARK: TESTE DO GERA XML - COMANDOS IF E ELSE
  SUBCASE("Gera XML para comandos if e while") {
    std::ofstream outJack("temp_if_while.jack");
    outJack << "class Main { function void main() { if (x) { return; } while (y) { return; } } }";
    outJack.close();

    {
      JackTokenizer tk("temp_if_while.jack");
      CompilationEngine eng(tk, "temp_if_while.xml");
      eng.compileClass();
    }

    std::ifstream xmlIn("temp_if_while.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();

    std::string xmlGerado = "";
    for (char c : buffer.str()) {
      if (c != '\r') xmlGerado += c;
    };

    std::string xmlEsperado =
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <subroutineDec>\n"
      "    <keyword> function </keyword>\n"
      "    <keyword> void </keyword>\n"
      "    <identifier> main </identifier>\n"
      "    <symbol> ( </symbol>\n"
      "    <parameterList>\n"
      "    </parameterList>\n"
      "    <symbol> ) </symbol>\n"
      "    <subroutineBody>\n"
      "      <symbol> { </symbol>\n"
      "      <statements>\n"
      "        <ifStatement>\n"
      "          <keyword> if </keyword>\n"
      "          <symbol> ( </symbol>\n"
      "          <expression>\n"
      "            <term>\n"
      "              <identifier> x </identifier>\n"
      "            </term>\n"
      "          </expression>\n"
      "          <symbol> ) </symbol>\n"
      "          <symbol> { </symbol>\n"
      "          <statements>\n"
      "            <returnStatement>\n"
      "              <keyword> return </keyword>\n"
      "              <symbol> ; </symbol>\n"
      "            </returnStatement>\n"
      "          </statements>\n"
      "          <symbol> } </symbol>\n"
      "        </ifStatement>\n"
      "        <whileStatement>\n"
      "          <keyword> while </keyword>\n"
      "          <symbol> ( </symbol>\n"
      "          <expression>\n"
      "            <term>\n"
      "              <identifier> y </identifier>\n"
      "            </term>\n"
      "          </expression>\n"
      "          <symbol> ) </symbol>\n"
      "          <symbol> { </symbol>\n"
      "          <statements>\n"
      "            <returnStatement>\n"
      "              <keyword> return </keyword>\n"
      "              <symbol> ; </symbol>\n"
      "            </returnStatement>\n"
      "          </statements>\n"
      "          <symbol> } </symbol>\n"
      "        </whileStatement>\n"
      "      </statements>\n"
      "      <symbol> } </symbol>\n"
      "    </subroutineBody>\n"
      "  </subroutineDec>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";
    
    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion

  //region MARK: TESTE DO GERA XML - COMANDOS LET
  SUBCASE("Gera XML para comandos if e while") {
    std::ofstream outJack("temp_let.jack");
    outJack << "class Main { function void main() { let x = y; let a[i] = j; } }";
    outJack.close();

    {
      JackTokenizer tk("temp_let.jack");
      CompilationEngine eng(tk, "temp_let.xml");
      eng.compileClass();
    }

    std::ifstream xmlIn("temp_let.xml");
    std::stringstream buffer;
    buffer << xmlIn.rdbuf();

    std::string xmlGerado = "";
    for (char c : buffer.str()) {
      if (c != '\r') xmlGerado += c;
    };

    std::string xmlEsperado =
      "<class>\n"
      "  <keyword> class </keyword>\n"
      "  <identifier> Main </identifier>\n"
      "  <symbol> { </symbol>\n"
      "  <subroutineDec>\n"
      "    <keyword> function </keyword>\n"
      "    <keyword> void </keyword>\n"
      "    <identifier> main </identifier>\n"
      "    <symbol> ( </symbol>\n"
      "    <parameterList>\n"
      "    </parameterList>\n"
      "    <symbol> ) </symbol>\n"
      "    <subroutineBody>\n"
      "      <symbol> { </symbol>\n"
      "      <statements>\n"
      "        <letStatement>\n"
      "          <keyword> let </keyword>\n"
      "          <identifier> x </identifier>\n"
      "          <symbol> = </symbol>\n"
      "          <expression>\n"
      "            <term>\n"
      "              <identifier> y </identifier>\n"
      "            </term>\n"
      "          </expression>\n"
      "          <symbol> ; </symbol>\n"
      "        </letStatement>\n"
      "        <letStatement>\n"
      "          <keyword> let </keyword>\n"
      "          <identifier> a </identifier>\n"
      "          <symbol> [ </symbol>\n"
      "          <expression>\n"
      "            <term>\n"
      "              <identifier> i </identifier>\n"
      "            </term>\n"
      "          </expression>\n"
      "          <symbol> ] </symbol>\n"
      "          <symbol> = </symbol>\n"
      "          <expression>\n"
      "            <term>\n"
      "              <identifier> j </identifier>\n"
      "            </term>\n"
      "          </expression>\n"
      "          <symbol> ; </symbol>\n"
      "        </letStatement>\n"
      "      </statements>\n"
      "      <symbol> } </symbol>\n"
      "    </subroutineBody>\n"
      "  </subroutineDec>\n"
      "  <symbol> } </symbol>\n"
      "</class>\n";
    
    CHECK(xmlGerado == xmlEsperado);
  }
  //endregion*/


}
