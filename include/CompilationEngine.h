#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

//CONTAINER MARK: INCLUSÃO DE BIBLIOTECAS
#include <string>
#include <fstream>
#include <memory>            
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

class CompilationEngine {
  //CONTAINER MARK: DECLARAÇÕES PRIVATE 
  private:
    JackTokenizer& tokenizer;                                     //Armazena o tokenizer
    std::ofstream outFile;        //CORRIGIR - verificar se isso ainda é necessario         //Armazena o arquivo de saída
    std::string indent;                                      //Armazena a identação
    int labelIndex = 0;                                      //Armazena o contador de labels

    SymbolTable symbolTable;                                 //Tabela de símbolos para gerenciar variáveis e escopos
    std::unique_ptr<VMWriter> vmWriter;                      //Gerenciador de escrita de código VM, usando ponteiro inteligente para garantir liberação de recursos
    std::string className;                                   //Armazena o nome da classe atual, necessário para gerar código VM corretamente, especialmente para métodos e construtores
    
    void writeXML(const std::string& text);   //CORRIGIR - verificar se é necessário        //Função para escrever no arquivo XML
    void process(TokenType2 expectedType);                        //Função para verificar se é o token esperado e avança
    void processKeyword(const std::string& expectedKeyword);      //Função para verificar se é a keyword esperada e avança
    void processSymbol(const std::string& expectedSymbol);        //Função para verificar se é o símbolo esperado e avança
    std::string escapeXml(const std::string& value); //CORRIGIR - verificar se é necessário // Para lidar com <, >, & e "

  //CONTAINER MARK: DECLARAÇÕES PÚBLICAS
  public:
    CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFilename);
    ~CompilationEngine();           //CORRIGIR - verificar se é indispensável

    void compileClass();                         //Compila uma classe Jack
    void compileClassVarDec();                   //Compila uma classe com variaveis
    void compileSubroutine();                    //Compila uma subrotina- funções, metodos e construtores
    void compileParameterList();                 //Compila uma lista de parametros
    void compileSubroutineBody(const std::string& subroutineName, const std::string& keyword); //Compila o corpo de uma subrotina - função, método ou construtor
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
    int compileExpressionList2();                //Compila uma lista de expressões (versão 2)
  //endregion
};

#endif