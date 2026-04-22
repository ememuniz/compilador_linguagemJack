#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <fstream>
#include <sstream>
#include <string>

std::string readAndNormalize(const std::string& caminho) {
  std::ifstream arquivo(caminho);

  if (!arquivo.is_open()) {
    return "ERRO: Não foi possível abrir o arquivo" + caminho;
  }

  std::stringstream buffer;
  buffer << arquivo.rdbuf();
  std::string conteudo = buffer.str();
  std::string conteudoLimpo = "";

  for (char c : conteudo) {
    if (c != '\r') {
      conteudoLimpo += c;
    }
  } //so pra garantir caso quebra de linha do windows de problema pra comparar com o do linux

  return conteudoLimpo;
}

//region MARK: TESTES DO ANALISADOR LÉXICO - TOKENIZER
TEST_CASE("Analisador Léxico - Comparando *T.xml com o gabarito") {
  SUBCASE("Testando arquivo MainT.xml") {
    std::string meuXML = readAndNormalize("MainT.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/MainT.xml");
    CHECK(meuXML == gabaritoXML);
  }

  SUBCASE("Testando arquivo SquareT.xml") {
    std::string meuXML = readAndNormalize("SquareT.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/SquareT.xml");
    CHECK(meuXML == gabaritoXML);
  }

  SUBCASE("Testando arquivo SquareGameT.xml") {
    std::string meuXML = readAndNormalize("SquareGameT.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/SquareGameT.xml");
    CHECK(meuXML == gabaritoXML);
  }
}
//endregion

//region MARK: TESTES DO ANALISADOR SINTÁTICO - PARSER
TEST_CASE("Analisador Sintático - Comparando *P.xml com o Gabarito"){
  SUBCASE("Testando arquivo MainP.xml"){
    std::string meuXML = readAndNormalize("MainP.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/Main.xml");
    CHECK(meuXML == gabaritoXML);
  }

  SUBCASE("Testando arquivo SquareP.xml"){
    std::string meuXML = readAndNormalize("SquareP.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/Square.xml");
    CHECK(meuXML == gabaritoXML);
  }

  SUBCASE("Testando arquivo SquareGameP.xml"){
    std::string meuXML = readAndNormalize("SquareGameP.xml");
    std::string gabaritoXML = readAndNormalize("gabarito/SquareGame.xml");
    CHECK(meuXML == gabaritoXML);
  }
}
//endregion

