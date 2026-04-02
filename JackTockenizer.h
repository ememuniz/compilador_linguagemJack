#pragma once  // Evita que o arquivo seja incluído mais de uma vez
#include <string> // Inclui a biblioteca que permite trabalhar com strings
#include <fstream> // Inclui a biblioteca que permite trabalhar com arquivos
#include <vector> // Inclui a biblioteca que permite trabalhar com vetores

//! Primeiramente cria a lista de tokens que serão utilizados
enum TokenType {
  KEYWORD,
  SYMBOL,
  IDENTIFIER,
  INT_CONST,
  STRING_CONST,
  NONE
};

