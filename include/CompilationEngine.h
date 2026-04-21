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
    std::string indent;                                 //Armazena a identação
    void advance();                                     //Funcao para avançar na leitura
    std::string escapeXML (const std::string& text);    //Formatam os simbolos que quebram o XML
    void printXMLToken();                               //escreve o token no arquivo de saída
    void printNonTerminalStart(const std::string& tag); //escreve a tag de inicio de uma declaração
    void printNonTerminalEnd(const std::string& tag);   //escreve a tag de fim de uma declaração
  //endregion

  //region MARK: DECLARAÇÕES PÚBLICAS
  public:
    CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFile);
    ~CompilationEngine();
    bool match(TokenTypeList type);                     //O token atual corresponde ao esperado?
    bool match(TokenTypeList type, const std::string& value);  //value e type correspondem ao token atual?
    void consume(TokenTypeList type, const std::string& expectedValue = "");  //Consome o token atual
    void compileClass();                                //Compila uma classe Jack
    void compileClassVarDec();                          //Compila uma classe com variaveis
    void compileSubroutine();                           //Compila uma subrotina
    void compileParameterList();                        //Compila uma lista de parametros
    void compileSubroutineBody();                       //Compila o corpo de uma subrotina
  //endregion
};

#endif