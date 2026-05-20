#include "SymbolTable.h"
//CONTAINER MARK: SYMBOLTABLE
SymbolTable::SymbolTable() {
  staticIdx = 0;
  fieldIdx = 0;
  argIdx = 0;
  localIdx = 0;
}                                                                //Inicializa os contadores para cada tipo de variável
//CONTAINER MARK: START SUBROUTINE
void SymbolTable::startSubroutine() {
  subroutineSymbols.clear();
  argIdx = 0;
  localIdx = 0;
}                                  //Limpa a tabela de símbolos da subrotina e reinicia os contadores para arg e local
//CONTAINER MARK: DEFINE
void SymbolTable::define(const std::string& name, const std::string& type, Kind kind) {
  if (kind == Kind::STATIC) {
    classSymbols[name] = { type, kind, staticIdx++ };
  } else if (kind == Kind::FIELD) {
    classSymbols[name] = { type, kind, fieldIdx++ };
  } else if (kind == Kind::ARG) {
    subroutineSymbols[name] = { type, kind, argIdx++ };
  } else if (kind == Kind::LOCAL) {
    subroutineSymbols[name] = { type, kind, localIdx++ };
  }
}    //Adiciona uma nova variável à tabela de simbolos, associando seu nome ao tipo, categoria e índice correspondente
//CONTAINER MARK: VARCOUNT
int SymbolTable::varCount(Kind kind) const {
  switch (kind){
    case Kind::STATIC: return staticIdx;
    case Kind::FIELD:  return fieldIdx; 
    case Kind::ARG:    return argIdx;
    case Kind::LOCAL:  return localIdx;
    default:           return 0;
  }
}              //Retorna o número de variáveis definidas de um determinado tipo, verificando o contador correspondente
//CONTAINER MARK: KINDOF
Kind SymbolTable::kindOf(const std::string& name) const {
  if (subroutineSymbols.find(name) != subroutineSymbols.end()) {
    return subroutineSymbols.at(name).kind;
  }                //Procura no escopo da subrotina primeiro, se encontrar a variável, retorna seu tipo (arg ou local)
  if (classSymbols.find(name) != classSymbols.end()) {
    return classSymbols.at(name).kind;
  } //Se não encontrar no escopo da subrotina, procura no escopo da classe, se encontrar a variável, retorna seu tipo (static ou field)
  return Kind::NONE;
}
//CONTAINER MARK: TYPEOF
std::string SymbolTable::typeOf(const std::string& name) const {
  if(subroutineSymbols.find(name) != subroutineSymbols.end()) {
    return subroutineSymbols.at(name).type;
  }          //Procura no escopo da subrotina primeiro, se encontrar a variável, retorna seu tipo (int, boolean, char ou nome de classe)
  if (classSymbols.find(name) != classSymbols.end()) {
    return classSymbols.at(name).type;
  } //Se não encontrar no escopo da subrotina, procura no escopo da classe, se encontrar a variável, retorna seu tipo (int, boolean, char ou nome de classe)
  return "";
}
//CONTAINER MARK: INDEXOF
int SymbolTable::indexOf(const std::string& name) const {
  if(subroutineSymbols.find(name) != subroutineSymbols.end()){
    return subroutineSymbols.at(name).index;
  }     //Procura no escopo da subrotina primeiro, se encontrar a variável, retorna seu índice dentro de sua categoria
  if (classSymbols.find(name) != classSymbols.end()) {
    return classSymbols.at(name).index;
  }     //Se não encontrar no escopo da subrotina, procura no escopo da classe, se encontrar a variável, retorna seu índice dentro de sua categoria
  return -1;
}