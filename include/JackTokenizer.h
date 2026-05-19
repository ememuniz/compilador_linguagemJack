#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#pragma once            // Evita que o arquivo seja incluído mais de uma vez
#include <string>       // Inclui a biblioteca que permite trabalhar com strings
#include <fstream>      // Inclui a biblioteca que permite trabalhar com arquivos
#include <vector>       // Inclui a biblioteca que permite trabalhar com vetores

//! Primeiramente cria a lista de tokens que serão utilizados
enum TokenType2 {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,
    NONE
};

struct Token {
    TokenType2 type;
    std::string lexeme; // Corrigido de 'lexame' para 'lexeme'
};

class JackTokenizer {
private:
    std::vector<Token> tokens;                            // Aqui armazena todos os tokens dentro do construtor
    size_t currentTokenIndex;                             // Armazena a posição do token atual
    std::string input_code;                               // Armazena o código do arquivo

    void tokenize(const std::string& sourceCode);         // Função para tokenizar o arquivo
    bool isKeyword(const std::string& lexeme);            // Função para verificar se um token é uma keyword
    
public:
    JackTokenizer(const std::string& fatiarArquivo);      // Construtor da classe
    bool hasMoreTokens() const;                           // Verificador se ainda há texto pra ler ou não. 
    void advance();                                       // Função para avançar na leitura e ir armazenando os tokens

    TokenType2 tokenType() const;                          // Função para retornar o tipo do token atual
    std::string getToken() const;                         // Função para retornar o token atual
    Token peek() const;                                   // Função para olhar o próximo token sem avançar
    void generateTokenXML(const std::string& outputFilename) const; // Função para gerar o arquivo XML dos tokens
};

#endif