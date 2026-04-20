#include "CompilationEngine.h"

//region MARK: CONSTRUTOR
CompilationEngine::CompilationEngine(JackTokenizer& tok, const std::string& outputFile)  //recebe como parâmetros o tokenizer (é o scanner) e o nome do arquivo de saída
  : tokenizer(tok) {                                    //Armazena o tokenizer
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
    case KEYWORD:         out << "<keyword> " << token << " </keyword>\n"; break;
    case IDENTIFIER:      out << "<identifier> " << token << " </identifier>\n"; break;
    case SYMBOL:          out << "<symbol> " << escapeXML(token) << " </symbol>\n"; break;
  }
}
//endregion