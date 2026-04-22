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
  if (match(type)) {                                    //Se o token atual corresponder ao esperado
    if (!expectedValue.empty() && tokenizer.getToken() != expectedValue) {  
      // Se não for o texto exato que esperávamos
      throw std::runtime_error("Erro: Esperava '" + expectedValue + "' mas encontrou '" + tokenizer.getToken() + "'"); 
    }
    printXMLToken();                                    //Escrevemos o token no XML
    advance();                                          //Avançamos para o próximo
  } else {
    // Melhoria: Agora o erro diz exatamente QUAL token causou o problema
    throw std::runtime_error("Erro: Tipo de token inesperado. Token atual: '" + tokenizer.getToken() + "'");
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
    case INT_CONST:       out << indent << "<integerConstant> " << token << " </integerConstant>\n"; break;
    case STRING_CONST:    out << indent << "<stringConstant> " << token << " </stringConstant>\n"; break;
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

  while (match(KEYWORD, "var")) {
    compileVarDec();
  }

  compileStatements();

  consume(SYMBOL, "}");

  printNonTerminalEnd("subroutineBody");          //Fecha a tag subroutineBody
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE DEC DE VARIAVEIS
void CompilationEngine::compileVarDec() {
  printNonTerminalStart("varDec");                //Abre a tag varDec -> identação + <varDec>

  //Consome a declaração
  consume(KEYWORD, "var");

  //Consome o tipo
  if (match(KEYWORD, "int")) consume(KEYWORD, "int");
  else if (match(KEYWORD, "char")) consume(KEYWORD, "char");
  else if (match(KEYWORD, "boolean")) consume(KEYWORD, "boolean");
  else consume(IDENTIFIER);

  //Consome o nome da variável
  consume(IDENTIFIER);

  //Enquanto tiver virgula, tem mais variáveis do mesmo tipo na mesma linha!
  while (match(SYMBOL, ",")) {
    consume(SYMBOL, ",");
    consume(IDENTIFIER);
  }

  consume(SYMBOL, ";");                           //Consome o token ";"

  printNonTerminalEnd("varDec");                  //Fecha a tag varDec
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE STATEMENTS
void CompilationEngine::compileStatements() {
  printNonTerminalStart("statements");

  // O loop continua enquanto o token atual for uma das 5 palavras-chave de comandos
  while (match(KEYWORD, "let") || match(KEYWORD, "if") || match(KEYWORD, "while") || match(KEYWORD, "do") || match(KEYWORD, "return")) {
    
    std::string kw = tokenizer.getToken();
    
    // Roteador de comandos
    if (kw == "let") {
      compileLet(); 
    } else if (kw == "if") {
      compileIf();
    } else if (kw == "while") {
      compileWhile();
    } else if (kw == "do") {
      compileDo(); 
    } else if (kw == "return") {
      compileReturn();
    }
  }

  printNonTerminalEnd("statements");
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE RETURN
void CompilationEngine::compileReturn() {
  printNonTerminalStart("returnStatement");       //Abre a tag returnStatement -> identação + <returnStatement>
  
  //Consome a declaração de return
  consume(KEYWORD, "return");

  //Se não for ponto e virgula,  o return tem uma expressão (ex: return 1 + 2;)
  if (!match(SYMBOL, ";")) {
    compileExpression();
  }

  //Consome o ponto e virgula
  consume(SYMBOL, ";");

  printNonTerminalEnd("returnStatement");         //Fecha a tag returnStatement
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE COMANDO IF
void CompilationEngine::compileIf() {
  printNonTerminalStart("ifStatement");           //Abre a tag ifStatement -> identação + <ifStatement>

  consume(KEYWORD, "if");                         //Consome a declaração de if
  consume(SYMBOL, "(");                           //Consome o parenteses de abertura
  compileExpression();                            
  consume(SYMBOL, ")");                           //Consome o parenteses de fechamento

  consume(SYMBOL, "{");
  compileStatements();                            //RECURSIVIDADE - chama compileStatements()
  consume(SYMBOL, "}");

  //O else é opcional
  if (match(KEYWORD, "else")) {
    consume(KEYWORD, "else");
    consume(SYMBOL, "{");
    compileStatements();                          //RECURSIVIDADE - chama compileStatements()
    consume(SYMBOL, "}");
  }

  printNonTerminalEnd("ifStatement");             //Fecha a tag ifStatement
}
//endregion

//region MARK: REGRAS DA GRAMÁTICA DE COMANDO WHILE
void CompilationEngine::compileWhile() {
  printNonTerminalStart("whileStatement");        //Abre a tag whileStatement -> identação + <whileStatement>

  consume(KEYWORD, "while");                      //Consome a declaração de while
  consume(SYMBOL, "(");                           //Consome o parenteses de abertura
  compileExpression();                            
  consume(SYMBOL, ")");                           //Consome o parenteses de fechamento

  consume(SYMBOL, "{");
  compileStatements();                            //RECURSIVIDADE - chama compileStatements()
  consume(SYMBOL, "}");

  printNonTerminalEnd("whileStatement");          //Fecha a tag whileStatement
}
//endregion

//region MARK: REGRAS DE GRAMÁTICA DE EXPRESSÕES
void CompilationEngine::compileExpression() {
  printNonTerminalStart("expression");            //Abre a tag expression -> identação + <expression>
  
  compileTerm();
  
  // Enquanto o token atual for um operador (+, -, *, /, &, |, <, >, =)
  while (match(SYMBOL, "+") || match(SYMBOL, "-") || match(SYMBOL, "*") || match(SYMBOL, "/") || match(SYMBOL, "&") || match(SYMBOL, "|") || match(SYMBOL, "<") || match(SYMBOL, ">") || match(SYMBOL, "=")) {
    consume(SYMBOL);
    compileTerm();
  }

  printNonTerminalEnd("expression");              //Fecha a tag expression
}
//endregion

//region MARK: REGRAS DE GRAMÁTICA DE TERMOS
void CompilationEngine::compileTerm() {
  printNonTerminalStart("term");                  //Abre a tag term -> identação + <term>

  if (match(INT_CONST)) {
    consume(INT_CONST);                           //Se for um número inteiro, consuma com o tipo INT_CONST
  }
  else if (match(STRING_CONST)){
    consume(STRING_CONST);                        //Se for uma string, consuma com o tipo STRING_CONST
  }
  else if (match(KEYWORD, "true") || match(KEYWORD, "false") || match(KEYWORD, "null") || match(KEYWORD, "this")) {
    consume(KEYWORD);                             //Se for uma keyword, consuma com o tipo KEYWORD
  }                                               //Um termo so pode ser keyword se for true, false, null ou this
  else if (match(SYMBOL, "-") || match(SYMBOL, "~")) {
    consume(SYMBOL);                              //Se for um sinal, consuma com o tipo SYMBOL
    compileTerm();                                //RECURSIVIDADE - necessaria pq é um operador unário
  }
  else if (match(SYMBOL, "(")) {
    consume(SYMBOL, "(");                         //Se for um parenteses de abertura, consuma com o tipo SYMBOL
    compileExpression();                          
    consume(SYMBOL, ")");                         //Consome o parenteses de fechamento
  }
  else if (match(IDENTIFIER)) {
    consume(IDENTIFIER);                          //Se for um identificador, consuma com o tipo IDENTIFIER

    //Se for um identificado , ele pode ser de um array (varName[expression]), chamada de uma função (funcName(expressionList)), chamada de método de classe (className.methodName(expressionList)) ou de uma variável (varName)

    //Se for um Array
    if (match(SYMBOL, "[")) {
      consume(SYMBOL, "[");
      compileExpression();
      consume(SYMBOL, "]");
    }

    //Se for uma Função
    else if (match(SYMBOL, "(")) {
      consume(SYMBOL, "(");
      compileExpressionList();
      consume(SYMBOL, ")");
    }

    //Se for um Método de classe 
    else if (match(SYMBOL, ".")) {
      consume(SYMBOL, ".");
      consume(IDENTIFIER);
      consume(SYMBOL, "(");
      compileExpressionList();
      consume(SYMBOL, ")");
    }
    //Se não for nada das anteriores, significa que é uma variável, o identificador é suficiente
  }

  printNonTerminalEnd("term");                    //Fecha a tag term
}
//endregion

//region MARK: REGRAS DE GRAMÁTICA DE ATRIBUIÇÃO DE VARIAVEL
void CompilationEngine::compileLet() {
  printNonTerminalStart("letStatement"); 

  consume(KEYWORD, "let");                                //Consome a declaração de let
  consume(IDENTIFIER);

  //Se é uma atribuição de um array
  if (match(SYMBOL, "[")) {
    consume(SYMBOL, "[");
    compileExpression();
    consume(SYMBOL, "]");
  }

  consume(SYMBOL, "=");
  compileExpression();
  consume(SYMBOL, ";");

  printNonTerminalEnd("letStatement");
}
//endregion

//region MARK: REGRAS DE GRAMÁTICA DE COMANDO DO
void CompilationEngine::compileDo() {
  printNonTerminalStart("doStatement");                     //Abre a tag doStatement -> identação + <doStatement>

  consume(KEYWORD, "do");                                   //Consome a declaração de do
  
  consume(IDENTIFIER); 

  if (match(SYMBOL, ".")) {
    consume(SYMBOL, ".");
    consume(IDENTIFIER);
  }

  consume(SYMBOL, "(");
  compileExpressionList();
  consume(SYMBOL, ")");

  consume(SYMBOL, ";");

  printNonTerminalEnd("doStatement");                       //Fecha a tag doStatement
}
//endregion

//region MARK: REGRAS DE GRAMÁTICA DE EXPRESSIONLIST
void CompilationEngine::compileExpressionList() {
  printNonTerminalStart("expressionList");            //Abre a tag expressionList -> identação + <expressionList>

  //Se o próximo token não for um parenteses de fechamento, significa que temos argumentos
  if (!match(SYMBOL, ")")) {
    compileExpression();                             

    while (match(SYMBOL, ",")) {
      consume(SYMBOL, ",");
      compileExpression();
    }
  }

  printNonTerminalEnd("expressionList");              //Fecha a tag expressionList
}
//endregion
