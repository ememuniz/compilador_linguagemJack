#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

//CONTAINER MARK: ENUM KIND
enum class Kind {
  STATIC,
  FIELD,
  ARG,
  LOCAL,
  NONE
};
//CONTAINER MARK: STRUCT SYMBOL
struct Symbol {
  std::string type;                            // Tipo da variável (int, boolean, char ou nome de classe)
  Kind kind;                                   // Categoria da variável (static, field, arg ou local)
  int index;                                   // Índice da variável dentro de sua categoria
};
//CONTAINER MARK: CLASS SYMBOLTABLE
class SymbolTable {
  //SUBITEM VARIÁVEIS PRIVADAS
  private:
    std::unordered_map<std::string, Symbol> classSymbols;      // Tabela de símbolos para variáveis de classe (static e field)
    std::unordered_map<std::string, Symbol> subroutineSymbols; // Tabela de símbolos para variáveis de subrotina (arg e local)

    int staticIdx;                                                                           // Contador para variáveis static
    int fieldIdx;                                                                             // Contador para variáveis field
    int argIdx;                                                                                 // Contador para variáveis arg
    int localIdx;                                                                             // Contador para variáveis local
  //SUBITEM VARIÁVEIS PÚBLICAS
  public:
    SymbolTable();                                       // Construtor para inicializar os contadores e as tabelas de símbolos
    void startSubroutine();                                      // Inicia uma nova subrotina, limpando as tabelas de símbolos
    void define(const std::string& name, const std::string& type, Kind kind);// Define uma nova variável na tabela de símbolos
    int varCount(Kind kind) const;                           // Retorna o número de variáveis definidas de um determinado tipo
    Kind kindOf(const std::string& name) const;                // Retorna o tipo (static, field, arg ou local) de uma variável
    std::string typeOf(const std::string& name) const;// Retorna o tipo (int, boolean, char ou nome de classe) de uma variável
    int indexOf(const std::string& name) const;                                                            // Retorna o índice
};
#endif
