#include <iostream>
#include <fstream>
#include <string>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

//Funções para formatar os símbolos que quebram o XML
std::string escapeXML(const std::string& text){
  if (text == "<") return "&lt;";
  if (text == ">") return "&gt;";
  if (text == "&") return "&amp;";
  if (text == "\"") return "&quot;";
  return text;
}

//argc - palavras que foram digitadas no terminal
//argv guarda as palavras
int main(int argc, char* argv[]){
  //verificar se o usuário digitou o nome do arquivo
  if (argc != 2){
    std::cout << "Uso correto: ./JackAnalyzer <arquivo,jack>" << std::endl; //Imprime o uso correto
    return 1; //Encerra o erro
  }
  std::string inputFile = argv[1]; //Armazena o nome do arquivo


  //region MARK: ANALISADOR LÉXICO (Gera o arquivo tokens *T.xml)

  //Cria o nome do arquivo de saida
  std::string tokenOutputFile = inputFile.substr(0, inputFile.find_last_of(".")) + "T.xml";

  //Inicia o nosso analisador e prepara o arquivo de saída
  JackTokenizer tokenizerTokens(inputFile);
  std::ofstream out(tokenOutputFile);

  //abrir a tag
  out << "<tokens>\n";

  //Enquanto houver código, extrai e escreve
  while(tokenizerTokens.hasMoreTokens()){
    tokenizerTokens.advance(); //Avançar para o próximo token

    //Se o token for vazio, apenas pula
    if (tokenizerTokens.tokenType() == NONE) continue;
    std::string token = tokenizerTokens.getToken();

    if (token.empty() || token.find_first_not_of(" \t\n\r\v\f") == std::string::npos){
      continue;
    }
    switch (tokenizerTokens.tokenType())
    {
    case KEYWORD:
      out << "<keyword> " << token << " </keyword>\n";
      break;
    case SYMBOL:
      out << "<symbol> " << escapeXML(token) << " </symbol>\n";
      break;
    case IDENTIFIER:
      out << "<identifier> " << token << " </identifier>\n";
      break;
    case INT_CONST:
      out << "<integerConstant> " << token << " </integerConstant>\n";
      break;
    case STRING_CONST:
      out << "<stringConstant> " << token << " </stringConstant>\n";
      break;
    default:
      break;
    }
  }

  out << "</tokens>\n"; //fechar a tag

  out.close();

  std::cout << "Sucesso!Arquivo gerado: " << tokenOutputFile << std::endl;

  //endregion

  //region MARK: ANALISADOR SINTÁTICO (Gera o árvore sintática *P.xml)
  try {
    std::string parserOutputFile = inputFile.substr(0, inputFile.find_last_of(".")) + "P.xml";

    JackTokenizer tokenizerParser(inputFile);

    CompilationEngine engine(tokenizerParser, parserOutputFile);

    engine.compileClass();

    std::cout << "Sucesso! Arquivo sintático gerado: " << parserOutputFile << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "\nErro durante a analise sintática de " << inputFile << ":" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  //endregion
  return 0;  //Encerra o programa
}
