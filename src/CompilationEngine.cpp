#include "CompilationEngine.h"

//region MARK: CONSTRUTOR
CompilationEngine::CompilationEngine(JackTokenizer& tok, const std::string& outputFile)  //recebe como parâmetros o tokenizer (é o scanner) e o nome do arquivo de saída
  : tokenizer(tok), indent("") {                                    //Armazena o tokenizer
    out.open(outputFile);                               //Armazena o arquivo de saída
    if (!out.is_open()) throw std::runtime_error("Erro ao criar arquivo XML.");          //Se der erro ao criar o arquivo, lançamos uma exceção
    if (tokenizer.hasMoreTokens()) {                    //Se houver mais tokens, avançamos
      tokenizer.advance();
    }
  }
//endregion

//region MARK: DESTRUTOR
CompilationEngine::~CompilationEngine() {               //Destrutor
  if (out.is_open()) out.close();                       //Se a saída estiver aberta, fechamos
}
//endregion

//region MARK: ADVANCE()
void CompilationEngine::advance() {
  if (tokenizer.hasMoreTokens()) {
    tokenizer.advance();
  }
}
//endregion

//region MARK: MATCH()
bool CompilationEngine::match(TokenTypeList type) {
  return tokenizer.tokenType() == type;      //Verifica se o token atual corresponde ao esperado pelo parser
}
//endregion

//region MARK: MATCH() COM VALUE
bool CompilationEngine::match(TokenTypeList type, const std::string& value) {
  return (tokenizer.tokenType() == type) && (tokenizer.getToken() == value);  //Verifica se o token atual corresponde ao esperado pelo parser
}
//endregion

//region MARK: CONSUME()
//consume() serve para consumir o token atual e avançar para o próximo
void CompilationEngine::consume(TokenTypeList type, const std::string& expectedValue) {
  if (match(type)) {                                //Se o token atual corresponder ao esperado
    if (!expectedValue.empty() && tokenizer.getToken() != expectedValue) {  //Se o expectValue não estiver vazio e ele não for o token atual
      throw std::runtime_error("Erro: Esperava" + expectedValue); //Lançamos uma exceção
    }
    printXMLToken();                                //Escrevemos o token no XML
    advance();                                      //Avançamos para o próximo
  } else {
    throw std::runtime_error("Erro: Tipo de token inesperado.");
  }
}
//endregion

//region MARK: PRINTXMLTOKEN()
std::string CompilationEngine::escapeXML (const std::string& text) {
  if (text == "<") return "&lt;";
  if (text == ">") return "&gt;";
  if (text == "&") return "&amp;";
  return text;
}
//endregion

//region MARK: PRINTXMLTOKEN()
void CompilationEngine::printXMLToken() {
  std::string token = tokenizer.getToken();
  switch (tokenizer.tokenType()){
    case KEYWORD:         out << indent << "<keyword> " << token << " </keyword>\n"; break;
    case IDENTIFIER:      out << indent << "<identifier> " << token << " </identifier>\n"; break;
    case SYMBOL:          out << indent << "<symbol> " << escapeXML(token) << " </symbol>\n"; break;
    default: break;
  }
}
//endregion


//region MARK: PRINTNONTERMINALSTART()
void CompilationEngine::printNonTerminalStart(const std::string& tag) {
  out << indent << "<" << tag << ">\n";           //out recebe a indentação mais o inicio da tag e a quebra de linha
  indent += "  ";                                 //toda vez que abre uma tag, empurra o texto 2 espaços pra direita.               
}
//endregion

//region MARK: PRINTNONTERMINALEND()
void CompilationEngine::printNonTerminalEnd(const std::string& tag) {
  indent.resize(indent.length() - 2);             //Tira 2 espaços (volta pra esquerda)
  out << indent << "</" << tag << ">\n";          //out recebe a indentação mais o fim da tag e a quebra de linha
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE CLASSE
void CompilationEngine::compileClass() {
  printNonTerminalStart("class");                 //Abre a tag class -> identação + <class>

  consume(KEYWORD, "class");                      //Consumimos o token "class"
  consume(IDENTIFIER);                            //Consumimos o nome da classe, como pode variar, só consumimos o tipo e não o token
  consume(SYMBOL, "{");                           //Consumimos o token "{"

  while (match(KEYWORD, "static") || match(KEYWORD, "field")) {
    compileClassVarDec();                         //Chamamos a regra de classVarDec quando for lido static ou field
  }

  consume(SYMBOL, "}");                           //Consumimos o token "}"
  printNonTerminalEnd("class");                   //Fecha a tag class
}
//endregion


//region MARK: REGRAS DA GRAMÁTICA DE CLASSVARDEC
void CompilationEngine::compileClassVarDec() {
  printNonTerminalStart("classVarDec");           //Abre a tag classVarDec -> identação + <classVarDec>

  //Consome 'static' ou 'field'
  if (match(KEYWORD, "static")) {
    consume(KEYWORD, "static");
  } else {
    consume(KEYWORD, "field");
  }

  //Consome o tipo (int, char, boolean ou NomeDeClasse)
  if (match(KEYWORD, "int")) {
    consume(KEYWORD, "int");
  } else if (match(KEYWORD, "char")) {
    consume(KEYWORD, "char");
  } else if (match(KEYWORD, "boolean")) {
    consume(KEYWORD, "boolean");
  } else {
    consume(IDENTIFIER);
  }

  //Consome o nome da variável
  consume(IDENTIFIER);

  //Enquanto tiver virgula, tem mais variáveis do mesmo tipo na mesma linha!
  while (match(SYMBOL, ",")) {
    consume(SYMBOL, ",");
    consume(IDENTIFIER);          
  }

  //Finaliza com ponto e virgula
  consume(SYMBOL, ";");
  printNonTerminalEnd("classVarDec");             //Fecha a tag classVarDec
}
//endregion