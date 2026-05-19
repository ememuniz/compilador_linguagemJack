#include "CompilationEngine.h"
#include <iostream>

//CONTAINER MARK: CONSTRUTOR
CompilationEngine::CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFilename)
  : tokenizer(tokenizer), indent("") {
  outFile.open(outputFilename);
  if (!outFile.is_open()) {
    std::cerr << "Erro ao abrir o arquivo de saída: " << outputFilename << std::endl;
  }
}

//CONTAINER MARK: DESTRUTOR
CompilationEngine::~CompilationEngine() {
  if (outFile.is_open()) {
    outFile.close();
  }
}

//CONTAINER MARK: ESCAPE XML
std::string CompilationEngine::escapeXml(const std::string& value){
  if (value == "<") return "&lt;";
  if (value == ">") return "&gt;";
  if (value == "&") return "&amp;";
  if (value == "\"") return "&quot;";
  return value;
}

//CONTAINER MARK: WRITE XML
void CompilationEngine::writeXML(const std::string& text){
  outFile << indent << text << "\n";
}

//CONTAINER MARK: PROCESS
void CompilationEngine::process(TokenType2 expectedType){
  if (tokenizer.tokenType() == expectedType){
    std::string tagName;
    std::string value = escapeXml(tokenizer.getToken());

    switch(expectedType){
      case TokenType2::KEYWORD:             tagName = "keyword"; break;
      case TokenType2::SYMBOL:              tagName = "symbol"; break;
      case TokenType2::IDENTIFIER:          tagName = "identifier"; break;
      case TokenType2::INT_CONST:           tagName = "integerConstant"; break;
      case TokenType2::STRING_CONST:        tagName = "stringConstant"; break;
      default: break;
    }

    writeXML("<" + tagName + "> " + value + " </" + tagName + ">");

    if (tokenizer.hasMoreTokens()) {
      tokenizer.advance();
    }
  } else {
    std::cerr << "Erro de Sintaxe: Esperava token do tipo " << expectedType << ", mas encontrou: " << tokenizer.getToken() << std::endl;
  }
}

//CONTAINER MARK: PROCESS SYMBOL
void CompilationEngine::processSymbol(const std::string& expectedSymbol){
  if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == expectedSymbol){
    process(TokenType2::SYMBOL);
  } else {
    std::cerr << "Erro de Sintaxe: Esperava símbolo '" << expectedSymbol << "', mas encontrou: " << tokenizer.getToken() << std::endl;
  }
}

//CONTAINER MARK: PROCESS KEYWORD
void CompilationEngine::processKeyword(const std::string& expectedKeyword){
  if (tokenizer.tokenType() == TokenType2::KEYWORD && tokenizer.getToken() == expectedKeyword){
    process(TokenType2::KEYWORD);
  } else {
    std::cerr << "Erro de Sintaxe: Esperava keyword '" << expectedKeyword << "', mas encontrou: " << tokenizer.getToken() << std::endl;
  }
}

//SUBITEM MARK: COMPILE CLASS
void CompilationEngine::compileClass() {
  writeXML("<class>");
  indent += "  ";                                     // Aumenta a identação

  processKeyword("class");                            // Lê "class"
  process(TokenType2::IDENTIFIER);                    // Lê o nome da classe (identificador)

  processSymbol("{");                                 // Lê "{"

  while (tokenizer.tokenType() == TokenType2::KEYWORD && (tokenizer.getToken() == "static" || tokenizer.getToken() == "field"))
  {
    compileClassVarDec();
  }                                                   // Lê variaveis de classe, se houver

  while (tokenizer.tokenType() == TokenType2::KEYWORD && (tokenizer.getToken() == "constructor" || tokenizer.getToken() == "function" || tokenizer.getToken() == "method"))
  {
    compileSubroutine();
  }                                                   // Le subrotinas (construtores, funções e métodos), se houver

  processSymbol("}");                                 // Lê "}" para finalizar a classe

  indent.erase(indent.length() - 2);                  // Diminui a identação
  writeXML("</class>");
}
//SUBITEM MARK: COMPILE CLASSVARDEC
void CompilationEngine::compileClassVarDec() {
  writeXML("<classVarDec>");
  indent += "  ";                                     // Aumenta a identação

  process(TokenType2::KEYWORD);                       // Lê "static" ou "field"

  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
    process(TokenType2::KEYWORD);                     // Lê "int", "boolean" ou "char"
  } else {
    process(TokenType2::IDENTIFIER);                  // ou o nome de uma classe pra indicar que que ta declarando uma instancia da classe 
  }

  process(TokenType2::IDENTIFIER);                     // Lê o nome da variavel ou da instância da classe

  while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
    processSymbol(",");                               // Lê ","
    process(TokenType2::IDENTIFIER);                   // Lê o nome da próxima variável
  }                                                   // Se houver mais variáveis, continua lendo, todas separadas por vírgula

  processSymbol(";");                                 // Lê ";"

  indent.erase(indent.length() - 2);                  // Diminui a identação
  writeXML("</classVarDec>");
}
//SUBITEM MARK: COMPILE SUBROUTINE
void CompilationEngine::compileSubroutine() {
  writeXML("<subroutineDec>");
  indent += "  ";                                   // Aumenta a identação

  process(TokenType2::KEYWORD);                     // Lê "constructor", "function" ou "method"

  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
      process(TokenType2::KEYWORD);                 // Lê "void", "int", "boolean", "char" 
  } else {
      process(TokenType2::IDENTIFIER);              // ou o nome de uma classe para indicar o tipo de retorno da subrotina
  }

  process(TokenType2::IDENTIFIER);                  // Lê o nome da subrotina

  processSymbol("(");                               // Lê "("    
  compileParameterList();                           // Compila a lista de parâmetros, mesmo que esteja vazia
  processSymbol(")");                               // Lê ")" para finalizar a lista de parâmetros   

  compileSubroutineBody();                          // Compila o corpo da subrotina

  indent.erase(indent.length() - 2);                // Diminui a identação
  writeXML("</subroutineDec>");
}
//SUBITEM MARK: COMPILE PARAMETER LIST
void CompilationEngine::compileParameterList() {
  writeXML("<parameterList>");
  indent += "  ";

  // Se o próximo token não for ')', significa que a lista não está vazia
  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ")")) {

      if (tokenizer.tokenType() == TokenType2::KEYWORD) {
          process(TokenType2::KEYWORD);                      // Primeiro parâmetro: Tipo
      } else {
          process(TokenType2::IDENTIFIER);                   // Primeiro parâmetro: Classe
      }
      
      process(TokenType2::IDENTIFIER);                       // Primeiro parâmetro: Nome

      // Outros parâmetros separados por vírgula
      while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
          processSymbol(",");
          // Próximo tipo
          if (tokenizer.tokenType() == TokenType2::KEYWORD) {
              process(TokenType2::KEYWORD);
          } else {
              process(TokenType2::IDENTIFIER);
          }
          // Próximo nome
          process(TokenType2::IDENTIFIER);
      }
  }

  indent.erase(indent.length() - 2);
  writeXML("</parameterList>");
}
//SUBITEM MARK: COMPILE SUBROUTINE BODY
void CompilationEngine::compileSubroutineBody() {
  writeXML("<subroutineBody>");
  indent += "  ";

  processSymbol("{");                                       // Abre chaves 

  // Processa todas as declarações de variáveis locais 'var' se houverem
  while (tokenizer.tokenType() == TokenType2::KEYWORD && tokenizer.getToken() == "var") {
      compileVarDec();
  }

  // Processa os comandos (statements)
  compileStatements();

  // Fecha chaves
  processSymbol("}");

  indent.erase(indent.length() - 2);
  writeXML("</subroutineBody>");
}
//SUBITEM MARK: COMPILE VAR DEC
void CompilationEngine::compileVarDec() {
  writeXML("<varDec>");
  indent += "  ";

  
  processKeyword("var");                        // Processa a palavra-chave 'var'

  // 2. Processa o Tipo ('int', 'char', 'boolean' ou nome de classe)
  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
      process(TokenType2::KEYWORD);             // Processa tipos primitivos
  } else {
      process(TokenType2::IDENTIFIER);          // ou um objeto de classe
  }

  process(TokenType2::IDENTIFIER);              // Processa o primeiro nome da variável

  // 4. Se houver mais variáveis na mesma linha separadas por vírgula: (',' varName)*
  while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
      processSymbol(",");
      process(TokenType2::IDENTIFIER);
  }

  // 5. Fecha com ponto e vírgula
  processSymbol(";");

  indent.erase(indent.length() - 2);
  writeXML("</varDec>");

}
//SUBITEM MARK: COMPILE STATEMENTS
void CompilationEngine::compileStatements() {
  writeXML("<statements>");
  indent += "  ";

  // Enquanto o próximo token for uma palavra-chave de comando, continuamos processando
  while (tokenizer.tokenType() == TokenType2::KEYWORD) {
      std::string cmd = tokenizer.getToken();
      
      if (cmd == "let") {
          compileLet();                              // Compila uma declaração de atribuição
      } else if (cmd == "do") {
          compileDo();                               // Compila uma declaração de chamada de subrotina
      } else if (cmd == "if") {
          compileIf();                               // Compila uma declaração de if
      } else if (cmd == "while") {
          compileWhile();                            // Compila uma declaração de while
      } else if (cmd == "return") {
          compileReturn();                           // Compila uma declaração de return
      } else {
          break; // Se for outra palavra-chave (como 'var' ou '}'), sai do loop
      }
  }

  indent.erase(indent.length() - 2);
  writeXML("</statements>");
}
//SUBITEM MARK: COMPILE LET
void CompilationEngine::compileLet() {
  writeXML("<letStatement>");
  indent += "  ";
  processKeyword("let");
  process(TokenType2::IDENTIFIER);

  if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "[") {
      processSymbol("[");
      compileExpression();
      processSymbol("]");
  }

  processSymbol("=");
  compileExpression();
  processSymbol(";");
  indent.erase(indent.length() - 2);
  writeXML("</letStatement>");
}
//SUBITEM MARK: COMPILE IF
void CompilationEngine::compileIf() {
  writeXML("<ifStatement>");
  indent += "  ";
  processKeyword("if");
  processSymbol("(");
  compileExpression();
  processSymbol(")");
  processSymbol("{");
  compileStatements();
  processSymbol("}");

  if (tokenizer.tokenType() == TokenType2::KEYWORD && tokenizer.getToken() == "else") {
      processKeyword("else");
      processSymbol("{");
      compileStatements();
      processSymbol("}");
  }
  indent.erase(indent.length() - 2);
  writeXML("</ifStatement>");
}
//SUBITEM MARK: COMPILE WHILE
void CompilationEngine::compileWhile() {
  writeXML("<whileStatement>");
  indent += "  ";
  processKeyword("while");
  processSymbol("(");
  compileExpression();
  processSymbol(")");
  processSymbol("{");
  compileStatements();
  processSymbol("}");
  indent.erase(indent.length() - 2);
  writeXML("</whileStatement>");
}
//SUBITEM MARK: COMPILE DO
void CompilationEngine::compileDo() {
  writeXML("<doStatement>");
  indent += "  ";
  processKeyword("do");
  
  process(TokenType2::IDENTIFIER); // Nome da função ou classe/objeto
  if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ".") {
      processSymbol(".");
      process(TokenType2::IDENTIFIER);
  }
  
  processSymbol("(");
  compileExpressionList();
  processSymbol(")");
  processSymbol(";");

  indent.erase(indent.length() - 2);
  writeXML("</doStatement>");
}
//SUBITEM MARK: COMPILE RETURN
void CompilationEngine::compileReturn() {
  writeXML("<returnStatement>");
  indent += "  ";

  // 1. Processa a palavra-chave 'return'
  processKeyword("return");

  // 2. Se o próximo token NÃO for ';', significa que há uma expressão de retorno (ex: return i;)
  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ";")) {
      compileExpression();
  }

  // 3. Processa o ponto e vírgula ';'
  processSymbol(";");

  indent.erase(indent.length() - 2);
  writeXML("</returnStatement>");
}
//SUBITEM MARK: COMPILE EXPRESSION
void CompilationEngine::compileExpression() {
  writeXML("<expression>");
  indent += "  ";
  compileTerm();

  while (tokenizer.tokenType() == TokenType2::SYMBOL && 
        (tokenizer.getToken() == "+" || tokenizer.getToken() == "-" || 
          tokenizer.getToken() == "*" || tokenizer.getToken() == "/" ||
          tokenizer.getToken() == "&" || tokenizer.getToken() == "|" ||
          tokenizer.getToken() == "<" || tokenizer.getToken() == ">" ||
          tokenizer.getToken() == "=")) {
      process(TokenType2::SYMBOL);
      compileTerm();
  }

  indent.erase(indent.length() - 2);
  writeXML("</expression>");
}
//SUBITEM MARK: COMPILE TERM
void CompilationEngine::compileTerm() {
  writeXML("<term>");
  indent += "  ";

  if (tokenizer.tokenType() == TokenType2::INT_CONST) {
      process(TokenType2::INT_CONST);
  } else if (tokenizer.tokenType() == TokenType2::STRING_CONST) {
      process(TokenType2::STRING_CONST);
  } else if (tokenizer.tokenType() == TokenType2::KEYWORD && 
            (tokenizer.getToken() == "true" || tokenizer.getToken() == "false" || 
              tokenizer.getToken() == "null" || tokenizer.getToken() == "this")) {
      process(TokenType2::KEYWORD);
  } else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "(") {
      processSymbol("(");
      compileExpression();
      processSymbol(")");
  } else if (tokenizer.tokenType() == TokenType2::SYMBOL && 
            (tokenizer.getToken() == "-" || tokenizer.getToken() == "~")) {
      process(TokenType2::SYMBOL);
      compileTerm();
  } else if (tokenizer.tokenType() == TokenType2::IDENTIFIER) {
      process(TokenType2::IDENTIFIER);

      if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "[") {
          processSymbol("[");
          compileExpression();
          processSymbol("]");
      } else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ".") {
          processSymbol(".");
          process(TokenType2::IDENTIFIER);
          processSymbol("(");
          compileExpressionList();
          processSymbol(")");
      } else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "(") {
          processSymbol("(");
          compileExpressionList();
          processSymbol(")");
      }
  }

  indent.erase(indent.length() - 2);
  writeXML("</term>");
}
//SUBITEM MARK: COMPILE EXPRESSION LIST
void CompilationEngine::compileExpressionList() {
  writeXML("<expressionList>");
  indent += "  ";

  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ")")) {
      compileExpression();
      while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
          processSymbol(",");
          compileExpression();
      }
  }

  indent.erase(indent.length() - 2);
  writeXML("</expressionList>");
}