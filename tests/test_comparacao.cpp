#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"
#include <fstream>
#include <sstream>
#include <string>

std::string readAndNormalize(const std::string& caminho) {
  std::ifstream arquivo(caminho);

  if (!arquivo.is_open()) {
    return "ERRO: Não foi possível abrir o arquivo" + caminho;
  }

  std::string conteudo = "";
  std::string linha;

  while (std::getline(arquivo, linha)) {
    if (!linha.empty() && linha.back() == '\r') {
      linha.pop_back(); // Remove o caractere de retorno de carro, se presente
    }  //se a linha não for vazia e terminar com um caractere de retorno de carro, ele é removido. Isso é importante para garantir que as quebras de linha sejam tratadas de forma consistente, independentemente do sistema operacional em que o arquivo foi criado.
    conteudo += linha + "\n";
  }
  return conteudo;
}

//region MARK: TESTES DO ANALISADOR LÉXICO - TOKENIZER
TEST_CASE("Analisador Léxico - Comparando *T.xml com o gabarito") {
  
  SUBCASE("Testando arquivo MainT.xml") {
    std::string caminhoMainJack = "Main.jack";
    std::string caminhoMainXMLGerado = "files/MainT.xml";
    std::string caminhoMainXMLGabarito = "gabarito/MainT.xml";

    JackTokenizer tokenizer(caminhoMainJack);
    tokenizer.generateTokenXML(caminhoMainXMLGerado);

    std::string textoGerado = readAndNormalize(caminhoMainXMLGerado);
    std::string textoGabarito = readAndNormalize(caminhoMainXMLGabarito);
    CHECK(textoGerado == textoGabarito);
  }

  SUBCASE("Testando arquivo SquareT.xml") {
    std::string caminhoSquareJack = "Square.jack";
    std::string caminhoSquareXMLGerado = "files/SquareT.xml";
    std::string caminhoSquareXMLGabarito = "gabarito/SquareT.xml";

    JackTokenizer tokenizer(caminhoSquareJack);
    tokenizer.generateTokenXML(caminhoSquareXMLGerado);

    std::string textoGerado = readAndNormalize(caminhoSquareXMLGerado);
    std::string textoGabarito = readAndNormalize(caminhoSquareXMLGabarito);
    CHECK(textoGerado == textoGabarito);
  }

  SUBCASE("Testando arquivo SquareGameT.xml") {
    std::string caminhoSquareGameJack = "SquareGame.jack";
    std::string caminhoSquareGameXMLGerado = "files/SquareGameT.xml";
    std::string caminhoSquareGameXMLGabarito = "gabarito/SquareGameT.xml";

    JackTokenizer tokenizer(caminhoSquareGameJack);
    tokenizer.generateTokenXML(caminhoSquareGameXMLGerado);

    std::string textoGerado = readAndNormalize(caminhoSquareGameXMLGerado);
    std::string textoGabarito = readAndNormalize(caminhoSquareGameXMLGabarito);
    CHECK(textoGerado == textoGabarito);
  }
}
//endregion

/*//region MARK: TESTES DO ANALISADOR SINTÁTICO - PARSER
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

*/

