#pragma once            // Evita que o arquivo seja incluído mais de uma vez
#include <string>       // Inclui a biblioteca que permite trabalhar com strings
#include <fstream>      // Inclui a biblioteca que permite trabalhar com arquivos
#include <vector>       // Inclui a biblioteca que permite trabalhar com vetores

//! Primeiramente cria a lista de tokens que serão utilizados
enum TokenTypeList {
  KEYWORD,
  SYMBOL,
  IDENTIFIER,
  INT_CONST,
  STRING_CONST,
  NONE
};

class JackTokenizer {
  private:
    std::string input_code;             //armazena o código do arquivo
    size_t position;                    //armazena a posição atual do arquivo
    std::string currentToken;           //armazena os caracteres do token até encontrar um delimitador
    TokenTypeList currentTokenType;     //armazena o tipo de token definido após o token completo ser armazenado em currentToken
    
  public:
    JackTokenizer(std::string filename);    // Construtor da classe
    std::string getCode();                  //função para testar se leu o arquivo certo
    bool hasMoreTokens();                   //verificador se ainda há texto pra ler ou não
    void advance();                         //função para avançar na leitura e ir armazenando os tokens
    TokenTypeList tokenType();              //determina o tipo de token do token atual
    std::string getToken();                 //procura qual o texto do token atual
};
