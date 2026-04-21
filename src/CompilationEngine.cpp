#include "CompilationEngine.h"

//region MARK: CONSTRUTOR
CompilationEngine::CompilationEngine(JackTokenizer& tok, const std::string& outputFile)  
//recebe como parâmetros o tokenizer (é o scanner) e o nome do arquivo de saída
  : tokenizer(tok), indent("") {                                    //Armazena o tokenizer
    out.open(outputFile);                                           //Armazena o arquivo de saída
    //Se der erro ao criar o arquivo, lançamos uma exceção
    if (!out.is_open()) throw std::runtime_error("Erro ao criar arquivo XML.");
    //Se houver mais tokens, avançamos          
    if (tokenizer.hasMoreTokens()) {                    
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

  while (match(KEYWORD, "constructor") || match(KEYWORD, "function") || match(KEYWORD, "method")) {
    compileSubroutine();                       //Chamamos a regra de SubroutineDec quando for lido constructor, function ou method
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

//region MARK: REGRAS DA GRAMÁTICA DE SUBROUTINEDEC
void CompilationEngine::compileSubroutine() {
  printNonTerminalStart("subroutineDec");         //Abre a tag subroutineDec -> identação + <subroutineDec>

  //Consome 'constructor', 'function' ou 'method'
  if (match(KEYWORD, "constructor")) consume(KEYWORD, "constructor");
  else if (match(KEYWORD, "function")) consume(KEYWORD, "function");
  else consume(KEYWORD, "method");

  //Consome o tipo de retorno (void ou tipo normal)
  if (match(KEYWORD, "void")) consume(KEYWORD, "void");
  else if (match(KEYWORD, "int")) consume(KEYWORD, "int");
  else if (match(KEYWORD, "char")) consume(KEYWORD, "char");
  else if (match(KEYWORD, "boolean")) consume(KEYWORD, "boolean");
  else consume(IDENTIFIER);

  //Consome o nome da Subrotina
  consume(IDENTIFIER);

  //Consome Parenteses e parametros
  consume(SYMBOL, "(");
  compileParameterList();
  consume(SYMBOL, ")");

  //Consome o corpo da função e finaliza
  compileSubroutineBody();
  printNonTerminalEnd("subroutineDec");           //Fecha a tag subroutineDec
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE PARAMETERLIST
void CompilationEngine::compileParameterList() {
  printNonTerminalStart("parameterList");         //Abre a tag parameterList -> identação + <parameterList>

  //Começa verificando se a lista tá vazia
  if (!match(SYMBOL, ")")) {

    //Consome o tipo do primeiro parametro
    if (match(KEYWORD, "int")) consume(KEYWORD, "int");
    else if (match(KEYWORD, "char")) consume(KEYWORD, "char");
    else if (match(KEYWORD, "boolean")) consume(KEYWORD, "boolean");
    else consume(IDENTIFIER);

    //Consome o nome do primeiro parametro
    consume(IDENTIFIER);

    //Enquanto tiver vírgula, tem mais parâmetros
    while (match(SYMBOL, ",")) {
      consume(SYMBOL, ",");

      //Consome o tipo do próximo parâmetro
      if (match(KEYWORD, "int")) consume(KEYWORD, "int");
      else if (match(KEYWORD, "char")) consume(KEYWORD, "char");
      else if (match(KEYWORD, "boolean")) consume(KEYWORD, "boolean");
      else consume(IDENTIFIER);

      //Consome o nome do proximo parâmetro
      consume(IDENTIFIER);
    }
  }

  printNonTerminalEnd("parameterList");           //Fecha a tag parameterList
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE SUBROUTINEBODY
void CompilationEngine::compileSubroutineBody() {
  printNonTerminalStart("subroutineBody");        //Abre a tag subroutineBody -> identação + <subroutineBody>

  consume(SYMBOL, "{");

  //TODO VARIAVEIS LOCAIS E COMANDOS AQUI

  consume(SYMBOL, "}");
  
  printNonTerminalEnd("subroutineBody");          //Fecha a tag subroutineBody
}
