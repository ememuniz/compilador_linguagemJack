#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include <fstream>

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

TEST_CASE("Testando ferramentas auxiliares do Compilation Engine") {
  std::string caminhoJack = "tests/arquivoJack/SimpleClass.jack";
  std::string caminhoXmlGerado = "tests/gerado/SimpleClassP.xml";
  std::string caminhoGabarito = "tests/gabarito/SimpleClass.xml";
  {
    JackTokenizer tokenizer(caminhoJack);                           // Inicializa o tokenizer
    CompilationEngine engine(tokenizer, caminhoXmlGerado);          // Inicializa o Engine passando o tokenizer e o local de saida
  
    engine.compileClass();
  }
  std::string textoGerado = readAndNormalize(caminhoXmlGerado);
  std::string textoGabarito = readAndNormalize(caminhoGabarito);
  CHECK(textoGerado == textoGabarito);
}

TEST_CASE("Comparacao do XML Sintatico - compileClass com variaveis") {
    std::string caminhoJack = "tests/arquivoJack/ClassVarDec.jack";
    std::string caminhoXmlGerado = "tests/gerado/ClassVarDecP.xml";
    std::string caminhoGabarito = "tests/gabarito/ClassVarDec.xml";

    {
        JackTokenizer tokenizer(caminhoJack);
        CompilationEngine engine(tokenizer, caminhoXmlGerado);
        engine.compileClass();
    } 

    std::string textoGerado = readAndNormalize(caminhoXmlGerado);
    std::string textoGabarito = readAndNormalize(caminhoGabarito);

    CHECK(textoGerado == textoGabarito);
}

TEST_CASE("Comparacao do XML Sintatico - compileClass com subrotina, argumentos e corpo vazio") {
    std::string caminhoJack = "tests/arquivoJack/Subroutine.jack";
    std::string caminhoXmlGerado = "tests/gerado/SubroutineP.xml";
    std::string caminhoGabarito = "tests/gabarito/Subroutine.xml";

    {
        JackTokenizer tokenizer(caminhoJack);
        CompilationEngine engine(tokenizer, caminhoXmlGerado);
        engine.compileClass();
    } 

    std::string textoGerado = readAndNormalize(caminhoXmlGerado);
    std::string textoGabarito = readAndNormalize(caminhoGabarito);

    CHECK(textoGerado == textoGabarito);
}

TEST_CASE("Comparacao do XML Sintatico - compileClass com variaveis locais") {
    std::string caminhoJack = "tests/arquivoJack/ClassWithLocalVars.jack";
    std::string caminhoXmlGerado = "tests/gerado/ClassWithLocalVarsP.xml";
    std::string caminhoGabarito = "tests/gabarito/ClassWithLocalVars.xml";

    {
        JackTokenizer tokenizer(caminhoJack);
        CompilationEngine engine(tokenizer, caminhoXmlGerado);
        engine.compileClass();
    } 

    std::string textoGerado = readAndNormalize(caminhoXmlGerado);
    std::string textoGabarito = readAndNormalize(caminhoGabarito);

    CHECK(textoGerado == textoGabarito);
}

TEST_CASE("Comparacao do XML Sintatico - compileStatements com return vazio") {
    std::string caminhoJack = "tests/arquivoJack/ClassWithReturn.jack";
    std::string caminhoXmlGerado = "tests/gerado/ClassWithReturnP.xml";
    std::string caminhoGabarito = "tests/gabarito/ClassWithReturn.xml";

    {
        JackTokenizer tokenizer(caminhoJack);
        CompilationEngine engine(tokenizer, caminhoXmlGerado);
        engine.compileClass();
    } 

    std::string textoGerado = readAndNormalize(caminhoXmlGerado);
    std::string textoGabarito = readAndNormalize(caminhoGabarito);

    CHECK(textoGerado == textoGabarito);
}
  
TEST_CASE("Comparacao do XML Sintatico - Bloco Unificado Let, Do, If, While") {
    std::string caminhoJack = "tests/arquivoJack/AllStatements.jack";
    std::string caminhoXmlGerado = "tests/gerado/AllStatementsP.xml";
    std::string caminhoGabarito = "tests/gabarito/AllStatements.xml";

    {
        JackTokenizer tokenizer(caminhoJack);
        CompilationEngine engine(tokenizer, caminhoXmlGerado);
        engine.compileClass();
    } 

    std::string textoGerado = readAndNormalize(caminhoXmlGerado);
    std::string textoGabarito = readAndNormalize(caminhoGabarito);

    CHECK(textoGabarito.find("ERRO") == std::string::npos); 
    CHECK(textoGerado == textoGabarito);
}