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


TEST_CASE("Comparando XML gerado com o Gabarito do Nand2Tetris") {
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