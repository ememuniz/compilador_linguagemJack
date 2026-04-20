#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

//region MARK: INCLUSÃO DE BIBLIOTECAS
#include "JackTokenizer.h"
#include <fstream>
#include <string>
#include <stdexcept>
//endregion

class CompilationEngine {
  //region MARK: DECLARAÇÕES PRIVATE 
  private:
    JackTokenizer& tokenizer;                           //Armazena o tokenizer
    std::ofstream out;                                  //Armazena o arquivo de saída
    void advance();                                     //Funcao para avançar na leitura
    std::string escapeXML (const std::string& text);    //Formatam os simbolos que quebram o XML
    void printXMLToken();                               //escreve o token no arquivo de saída
  //endregion

  //region MARK: DECLARAÇÕES PÚBLICAS
  public:
    CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFile);
    ~CompilationEngine();
    bool match(TokenTypeList type);                     //O token atual corresponde ao esperado?
    bool match(TokenTypeList type, const std::string& value);  //value e type correspondem ao token atual?
    void consume(TokenTypeList type, const std::string& expectedValue = "");    //Consome o token atual
  //endregion
};

#endif