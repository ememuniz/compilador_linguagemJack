#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

//CONTAINER MARK: INCLUSÃO DE BIBLIOTECAS
#pragma once              
#include "JackTokenizer.h"
#include <fstream>
#include <string>
#include <stdexcept>

class CompilationEngine {
  //CONTAINER MARK: DECLARAÇÕES PRIVATE 
  private:
    JackTokenizer& tokenizer;                                     //Armazena o tokenizer
    std::ofstream outFile;                                        //Armazena o arquivo de saída
    std::string indent;                                      //Armazena a identação
    
    void writeXML(const std::string& text);                       //Função para escrever no arquivo XML
    void process(TokenType2 expectedType);                        //Função para verificar se é o token esperado e avança
    void processSymbol(const std::string& expectedSymbol);        //Função para verificar se é o símbolo esperado e avança
    void processKeyword(const std::string& expectedKeyword);      //Função para verificar se é a keyword esperada e avança
    std::string escapeXml(const std::string& value); // Para lidar com <, >, & e "


  //CONTAINER MARK: DECLARAÇÕES PÚBLICAS
  public:
    CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFilename);
    ~CompilationEngine();

    void compileClass();                         //Compila uma classe Jack
    void compileClassVarDec();                   //Compila uma classe com variaveis
    void compileSubroutine();                    //Compila uma subrotina- funções, metodos e construtores
    void compileParameterList();                 //Compila uma lista de parametros
    void compileSubroutineBody();                //Compila o corpo de uma subrotina - função, método ou construtor
    void compileVarDec();                        //Compila uma declaração de variavel
    void compileStatements();                    //Compila um conjunto de declarações
    void compileLet();                           //Compila uma declaração de atribuição
    void compileIf();                            //Compila uma declaração de if
    void compileWhile();                         //Compila uma declaração de while
    void compileDo();                            //Compila uma declaração de do
    void compileReturn();                        //Compila uma declaração de return
    void compileExpression();                    //Compila uma expressão
    void compileTerm();                          //Compila um termo
    void compileExpressionList();                //Compila uma lista de expressões
  //endregion
};

#endif