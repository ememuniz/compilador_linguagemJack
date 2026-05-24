#include "CompilationEngine.h"
#include <iostream>

//CONTAINER MARK: CONSTRUTOR
CompilationEngine::CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFilename)
  : tokenizer(tokenizer), indent("") {
  
  // CORREÇÃO: Mudamos o nome do arquivo XML para evitar que ele dispute/sobrescreva o arquivo do VMWriter
  std::string xmlFilename = outputFilename.substr(0, outputFilename.find_last_of('.')) + ".xml";
  outFile.open(xmlFilename); 
  
  if (!outFile.is_open()) {
    std::cerr << "Erro ao abrir o arquivo de saída XML: " << xmlFilename << std::endl;
  }
  
  // Inicializa o VMWriter com o arquivo original (geralmente terminado em .vm)
  vmWriter = std::make_unique<VMWriter>(outputFilename); 
}

//CONTAINER MARK: DESTRUTOR
CompilationEngine::~CompilationEngine() {
  if (outFile.is_open()) {
    outFile.close();
  }
}

//CONTAINER MARK: kindToSegment
static Segment kindToSegment(Kind kind) {
  switch (kind) {
    case Kind::STATIC: return Segment::STATIC;
    case Kind::FIELD:  return Segment::THIS;
    case Kind::ARG:    return Segment::ARGUMENT;
    case Kind::LOCAL:  return Segment::LOCAL;
    default:           return Segment::CONSTANT;
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
      case TokenType2::KEYWORD:     tagName = "keyword"; break;
      case TokenType2::SYMBOL:      tagName = "symbol"; break;
      case TokenType2::IDENTIFIER:  tagName = "identifier"; break;
      case TokenType2::INT_CONST:   tagName = "integerConstant"; break;
      case TokenType2::STRING_CONST:tagName = "stringConstant"; break;
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
  writeXML("<class>");                                //CODXML
  indent += "  ";                                     //CODXML Aumenta a identação

  processKeyword("class");                            // Lê "class"
  className = tokenizer.getToken();                   // Armazena o nome da classe para uso futuro
  process(TokenType2::IDENTIFIER);                    // Lê o nome da classe (identificador)

  processSymbol("{");                                 // Lê "{"

  while (tokenizer.tokenType() == TokenType2::KEYWORD && (tokenizer.getToken() == "static" || tokenizer.getToken() == "field"))
  {
    compileClassVarDec();
  }                                                   // Lê variaveis de classe, se houver

  while (tokenizer.tokenType() == TokenType2::KEYWORD && (tokenizer.getToken() == "constructor" || tokenizer.getToken() == "function" || tokenizer.getToken() == "method"))
  {
    compileSubroutine();
  }                                                   // Le subrotinas (construtores, funções e métodos), se houver

  processSymbol("}");                                 // Lê "}" para finalizar a classe

  indent.erase(indent.length() - 2);                  //CODXML Diminui a identação
  writeXML("</class>");                               //CODXML
}

//SUBITEM MARK: COMPILE CLASSVARDEC
void CompilationEngine::compileClassVarDec() {
  writeXML("<classVarDec>");                          //CODXML
  indent += "  ";                                     //CODXML Aumenta a identação

  //NOTE lê o tipo de variavel - static ou field
  std::string varKind = tokenizer.getToken();         
  Kind kind = (varKind == "static") ? Kind::STATIC : Kind::FIELD;  
  process(TokenType2::KEYWORD);                       // Lê "static" ou "field"

  //NOTE lê o tipo de dado da variavel - int, boolean, char ou nome de classe
  std::string type = tokenizer.getToken();            
  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
    process(TokenType2::KEYWORD);                     // Lê "int", "boolean" ou "char"
  } else {
    process(TokenType2::IDENTIFIER);                  // ou o nome de uma classe
  }

  //NOTE lê o nome da variavel e insere na tabela de simbolos
  std::string name = tokenizer.getToken();            
  process(TokenType2::IDENTIFIER);                    // Lê o nome da variavel
  symbolTable.define(name, type, kind);               // Define a variável na tabela de símbolos

  //NOTE Se houver mais variáveis na mesma linha, continua lendo, todas separadas por vírgula
  while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
    processSymbol(",");                               // Lê ","
    name = tokenizer.getToken();                      // Armazena o nome da próxima variável
    process(TokenType2::IDENTIFIER);                  // Lê o nome da próxima variável
    symbolTable.define(name, type, kind);             // Define a variável na tabela de símbolos
  }                                                   

  processSymbol(";");                                 // Lê ";"

  indent.erase(indent.length() - 2);                  //CODXML Diminui a identação
  writeXML("</classVarDec>");                         //CODXML
}

//SUBITEM MARK: COMPILE SUBROUTINE
void CompilationEngine::compileSubroutine() {
  writeXML("<subroutineDec>");                      //CODXML
  indent += "  ";                                   //CODXML Aumenta a identação

  symbolTable.startSubroutine();                    

  //NOTE lê o tipo da subrotina - constructor, function ou method
  std::string keyword = tokenizer.getToken();       
  process(TokenType2::KEYWORD);                     // Lê "constructor", "function" ou "method"

  //NOTE lê o this como primeiro argumento implícito para métodos
  if (keyword == "method") {
    symbolTable.define("this", className, Kind::ARG);
  }

  //NOTE lê o tipo de retorno da subrotina
  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
      process(TokenType2::KEYWORD);                 // Lê "void", "int", "boolean", "char" 
  } else {
      process(TokenType2::IDENTIFIER);              // ou o nome de uma classe
  }

  //NOTE lê o nome da subrotina e insere na tabela de simbolos
  std::string subroutineName = tokenizer.getToken();
  process(TokenType2::IDENTIFIER);                  // Lê o nome da subrotina

  processSymbol("(");                               // Lê "("    
  compileParameterList();                           // Compila a lista de parâmetros
  processSymbol(")");                               // Lê ")"    

  //NOTE compila o corpo da subrotina passando o nome da subrotina e o tipo de retorno
  compileSubroutineBody(subroutineName, keyword);   

  indent.erase(indent.length() - 2);                //CODXML Diminui a identação
  writeXML("</subroutineDec>");                     //CODXML
}

//SUBITEM MARK: COMPILE PARAMETER LIST
void CompilationEngine::compileParameterList() {
  writeXML("<parameterList>");                      //CODXML
  indent += "  ";                                   //CODXML

  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ")")) {
    //NOTE lê o tipo ou o nome da classe do primeiro parâmetro
    std::string type = tokenizer.getToken();               
    if (tokenizer.tokenType() == TokenType2::KEYWORD) {
        process(TokenType2::KEYWORD);                      
    } else {
        process(TokenType2::IDENTIFIER);                   
    }

    //NOTE lê o nome do parâmetro
    std::string name = tokenizer.getToken();               
    process(TokenType2::IDENTIFIER);                       
    symbolTable.define(name, type, Kind::ARG);             

    //NOTE Outros parâmetros separados por vírgula
    while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
        processSymbol(",");
        //NOTE Próximo tipo
        type = tokenizer.getToken();
        if (tokenizer.tokenType() == TokenType2::KEYWORD) {
            process(TokenType2::KEYWORD);
        } else {
            process(TokenType2::IDENTIFIER);
        }

        //NOTE Próximo nome
        name = tokenizer.getToken();
        process(TokenType2::IDENTIFIER);
        symbolTable.define(name, type, Kind::ARG);           
    }
  }

  indent.erase(indent.length() - 2);                         //CODXML
  writeXML("</parameterList>");                              //CODXML
}

//SUBITEM MARK: COMPILE SUBROUTINE BODY
void CompilationEngine::compileSubroutineBody(const std::string& subroutineName, const std::string& keyword) {
  writeXML("<subroutineBody>");                              //CODXML
  indent += "  ";                                            //CODXML

  processSymbol("{");                                       // Abre chaves 

  //NOTE Processa todas as declarações de variáveis locais 'var'
  while (tokenizer.tokenType() == TokenType2::KEYWORD && tokenizer.getToken() == "var") {
      compileVarDec();
  }

  //NOTE Agora os varDec terminaram, então a tabela sabe o número exato de variáveis locais
  int nLocals = symbolTable.varCount(Kind::LOCAL);          
  vmWriter->writeFunction(className + "." + subroutineName, nLocals);     
  
  if(keyword == "constructor"){
    int nFields = symbolTable.varCount(Kind::FIELD);        
    vmWriter->writePush(Segment::CONSTANT, nFields);        
    vmWriter->writeCall("Memory.alloc", 1);                 
    vmWriter->writePop(Segment::POINTER, 0);                     
  }
  else if (keyword == "method") {
    vmWriter->writePush(Segment::ARGUMENT, 0);              
    vmWriter->writePop(Segment::POINTER, 0);                
  }

  //NOTE Processa os comandos (statements)
  compileStatements();

  // Fecha chaves
  processSymbol("}");

  indent.erase(indent.length() - 2);                         //CODXML
  writeXML("</subroutineBody>");                             //CODXML
}

//SUBITEM MARK: COMPILE VAR DEC
void CompilationEngine::compileVarDec() {
  writeXML("<varDec>");                                      //CODXML
  indent += "  ";                                            //CODXML

  processKeyword("var");                        
  
  //NOTE Processa o Tipo
  std::string type = tokenizer.getToken();      
  if (tokenizer.tokenType() == TokenType2::KEYWORD) {
      process(TokenType2::KEYWORD);             
  } else {
      process(TokenType2::IDENTIFIER);          
  }

  //NOTE lê o nome da variável e insere na tabela de simbolos
  std::string name = tokenizer.getToken();      
  process(TokenType2::IDENTIFIER);              
  symbolTable.define(name, type, Kind::LOCAL);  

  //NOTE Se houver mais variáveis na mesma linha separadas por vírgula
  while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
      processSymbol(",");
      name = tokenizer.getToken();
      process(TokenType2::IDENTIFIER);
      symbolTable.define(name, type, Kind::LOCAL);
  }
  // Fecha com ponto e vírgula
  processSymbol(";");

  indent.erase(indent.length() - 2);                 //CODXML
  writeXML("</varDec>");                             //CODXML
}

//SUBITEM MARK: COMPILE STATEMENTS
void CompilationEngine::compileStatements() {
  writeXML("<statements>");
  indent += "  ";

  while (tokenizer.tokenType() == TokenType2::KEYWORD) {
      std::string cmd = tokenizer.getToken();
      
      if (cmd == "let") {
          compileLet();                              
      } else if (cmd == "do") {
          compileDo();                               
      } else if (cmd == "if") {
          compileIf();                               
      } else if (cmd == "while") {
          compileWhile();                            
      } else if (cmd == "return") {
          compileReturn();                           
      } else {
          break; 
      }
  }

  indent.erase(indent.length() - 2);
  writeXML("</statements>");
}

//SUBITEM MARK: COMPILE LET
void CompilationEngine::compileLet() {
  writeXML("<letStatement>");                             //CODXML
  indent += "  ";                                         //CODXML
  processKeyword("let");

  std::string name = tokenizer.getToken();
  process(TokenType2::IDENTIFIER);

  Kind kind = symbolTable.kindOf(name);
  int idx = symbolTable.indexOf(name);
  bool isArray = false;

  // --- TRATAMENTO SE FOR UM VETOR: let arr[i] = ... ---
  if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "[") {
      isArray = true;
      processSymbol("[");
      
      vmWriter->writePush(kindToSegment(kind), idx);
      compileExpression();
      processSymbol("]");

      vmWriter->writeArithmetic(Command::ADD);
  }

  processSymbol("=");

  // --- AVALIA A EXPRESSÃO DA DIREITA ---
  compileExpression(); 
  processSymbol(";");

  // --- DESCARREGA O VALOR NA MEMÓRIA ---
  if (isArray) {
      vmWriter->writePop(Segment::TEMP, 0);    
      vmWriter->writePop(Segment::POINTER, 1); 
      vmWriter->writePush(Segment::TEMP, 0);   
      vmWriter->writePop(Segment::THAT, 0);    
  } else {
      if (kind != Kind::NONE) {
          vmWriter->writePop(kindToSegment(kind), idx);
      }
  }
  indent.erase(indent.length() - 2);               //CODXML
  writeXML("</letStatement>");                     //CODXML
}

//SUBITEM MARK: COMPILE IF
void CompilationEngine::compileIf() {
  writeXML("<ifStatement>");                       //CODXML
  indent += "  ";                                  //CODXML
  int labelNum = labelIndex++; 
  std::string labelFalse = "IF_FALSE" + std::to_string(labelNum);
  std::string labelEnd = "IF_END" + std::to_string(labelNum);

  processKeyword("if");
  
  processSymbol("(");
  compileExpression(); 
  processSymbol(")");

  vmWriter->writeArithmetic(Command::NOT);
  vmWriter->writeIf(labelFalse);

  processSymbol("{");
  compileStatements(); 
  processSymbol("}");
  
  vmWriter->writeGoto(labelEnd);
  vmWriter->writeLabel(labelFalse);

  if (tokenizer.tokenType() == TokenType2::KEYWORD && tokenizer.getToken() == "else") {
      processKeyword("else");
      processSymbol("{");
      compileStatements(); 
      processSymbol("}");
  }

  vmWriter->writeLabel(labelEnd);
  indent.erase(indent.length() - 2);               //CODXML
  writeXML("</ifStatement>");                      //CODXML
}

//SUBITEM MARK: COMPILE WHILE
void CompilationEngine::compileWhile() {
  writeXML("<whileStatement>");           //CODXML
  indent += "  ";                         //CODXML
  int labelNum = labelIndex++; 
  std::string labelExp = "WHILE_EXP" + std::to_string(labelNum);
  std::string labelEnd = "WHILE_END" + std::to_string(labelNum);

  processKeyword("while");
  vmWriter->writeLabel(labelExp);

  processSymbol("(");
  compileExpression(); 
  processSymbol(")");

  vmWriter->writeArithmetic(Command::NOT);
  vmWriter->writeIf(labelEnd); 

  processSymbol("{");
  compileStatements(); 
  processSymbol("}");

  vmWriter->writeGoto(labelExp);
  vmWriter->writeLabel(labelEnd);
  
  indent.erase(indent.length() - 2);      //CODXML
  writeXML("</whileStatement>");          //CODXML
}

//SUBITEM MARK: COMPILE DO
void CompilationEngine::compileDo() {
  writeXML("<doStatement>");                //CODXML
  indent += "  ";                           //CODXML
  processKeyword("do");
    
  std::string name = tokenizer.getToken();
  process(TokenType2::IDENTIFIER);

  int nArgs = 0;
  std::string fullName = "";

  if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ".") {
      processSymbol(".");
      std::string subName = tokenizer.getToken();
      process(TokenType2::IDENTIFIER);

      Kind kind = symbolTable.kindOf(name);
      if (kind != Kind::NONE) {
          vmWriter->writePush(kindToSegment(kind), symbolTable.indexOf(name));
          nArgs = 1;
          fullName = symbolTable.typeOf(name) + "." + subName;
      } else {
          fullName = name + "." + subName;
      }
  } 
  else {
      vmWriter->writePush(Segment::POINTER, 0);
      nArgs = 1;
      fullName = className + "." + name;
  }

  processSymbol("(");
  nArgs += compileExpressionList2(); 
  processSymbol(")");
  processSymbol(";");

  vmWriter->writeCall(fullName, nArgs);
  vmWriter->writePop(Segment::TEMP, 0);

  indent.erase(indent.length() - 2);         //CODXML
  writeXML("</doStatement>");                //CODXML
}

//SUBITEM MARK: COMPILE RETURN
void CompilationEngine::compileReturn() {
  writeXML("<returnStatement>");               //CODXML
  indent += "  ";                              //CODXML

  processKeyword("return");

  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ";")) {
    compileExpression();
  } else {
    vmWriter->writePush(Segment::CONSTANT, 0);
  }

  processSymbol(";");
  vmWriter->writeReturn();

  indent.erase(indent.length() - 2);           //CODXML
  writeXML("</returnStatement>");              //CODXML
}

//SUBITEM MARK: COMPILE EXPRESSION
void CompilationEngine::compileExpression() {
  writeXML("<expression>");                               //CODXML
  indent += "  ";                                         //CODXML
  compileTerm();
  while (tokenizer.tokenType() == TokenType2::SYMBOL && 
        (tokenizer.getToken() == "+" || tokenizer.getToken() == "-" || 
          tokenizer.getToken() == "*" || tokenizer.getToken() == "/" ||
          tokenizer.getToken() == "&" || tokenizer.getToken() == "|" ||
          tokenizer.getToken() == "<" || tokenizer.getToken() == ">" ||
          tokenizer.getToken() == "=")) {
      std::string op = tokenizer.getToken();
      process(TokenType2::SYMBOL);
      compileTerm();

      if (op == "+") vmWriter->writeArithmetic(Command::ADD);
      else if (op == "-") vmWriter->writeArithmetic(Command::SUB);
      else if (op == "=") vmWriter->writeArithmetic(Command::EQ);
      else if (op == ">") vmWriter->writeArithmetic(Command::GT);
      else if (op == "<") vmWriter->writeArithmetic(Command::LT);
      else if (op == "&") vmWriter->writeArithmetic(Command::AND);
      else if (op == "|") vmWriter->writeArithmetic(Command::OR);
      else if (op == "*") vmWriter->writeCall("Math.multiply", 2);
      else if (op == "/") vmWriter->writeCall("Math.divide", 2); 
  }

  indent.erase(indent.length() - 2);                                   //CODXML
  writeXML("</expression>");                                           //CODXML
}

//SUBITEM MARK: COMPILE TERM
void CompilationEngine::compileTerm() {
  writeXML("<term>");                         //CODXML               
  indent += "  ";                             //CODXML
  
  //NOTE cenário 1: constante
  if (tokenizer.tokenType() == TokenType2::INT_CONST) {
    int val = std::stoi(tokenizer.getToken());                      
    vmWriter->writePush(Segment::CONSTANT, val);
    process(TokenType2::INT_CONST);
  } 
  //NOTE cenário 2: constante de texto / string
  else if (tokenizer.tokenType() == TokenType2::STRING_CONST) {
    std::string str = tokenizer.getToken();
    process(TokenType2::STRING_CONST);

    vmWriter->writePush(Segment::CONSTANT, str.length());
    vmWriter->writeCall("String.new", 1);
    for (char c : str) {
      vmWriter->writePush(Segment::CONSTANT, static_cast<int>(c));  
      vmWriter->writeCall("String.appendChar", 2);
    }
  } 
  //NOTE cenário 3: Palavras-chave especiais -> true, false, null, this
  else if (tokenizer.tokenType() == TokenType2::KEYWORD && 
            (tokenizer.getToken() == "true" || tokenizer.getToken() == "false" || 
              tokenizer.getToken() == "null" || tokenizer.getToken() == "this")) {
      std::string kw = tokenizer.getToken();
      process(TokenType2::KEYWORD);
      if (kw == "false" || kw == "null") vmWriter->writePush(Segment::CONSTANT, 0);
      else if (kw == "true") {
        vmWriter->writePush(Segment::CONSTANT, 0);
        vmWriter->writeArithmetic(Command::NOT);      
      }
      // CORREÇÃO: Removido o writePop que limpava e destruía o endereço do objeto atual
      else if (kw == "this") {
        vmWriter->writePush(Segment::POINTER, 0);      
      }
  } 
  //NOTE cenário 4: Expressão entre parênteses
  else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "(") {
      processSymbol("(");
      compileExpression(); 
      processSymbol(")");
  } 
  //NOTE cenário 5: Operador unário
  else if (tokenizer.tokenType() == TokenType2::SYMBOL && 
            (tokenizer.getToken() == "-" || tokenizer.getToken() == "~")) {
      std::string unaryOp = tokenizer.getToken();
      process(TokenType2::SYMBOL);
      compileTerm();

      if (unaryOp == "-") vmWriter->writeArithmetic(Command::NEG);
      else if (unaryOp == "~") vmWriter->writeArithmetic(Command::NOT);
  } 
  //NOTE cenário 6: Identificador
  else if (tokenizer.tokenType() == TokenType2::IDENTIFIER) {
    std::string name = tokenizer.getToken();
    process(TokenType2::IDENTIFIER);

    //NOTE cenário 6.1: É um acesso a Vetor/Array 
    if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "[") {
      processSymbol("[");

      Kind kind = symbolTable.kindOf(name);
      int idx = symbolTable.indexOf(name);
      vmWriter->writePush(kindToSegment(kind), idx);

      compileExpression();
      processSymbol("]");
      
      vmWriter->writeArithmetic(Command::ADD);                  
      vmWriter->writePop(Segment::POINTER, 1);                  
      vmWriter->writePush(Segment::THAT, 0);                    
    } 

    //NOTE cenário 6.2: é um acesso a Função externa / Método de objeto
    else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ".") {
      processSymbol(".");
      std::string subName = tokenizer.getToken();
      process(TokenType2::IDENTIFIER);

      processSymbol("(");

      int nArgs = 0;
      Kind kind = symbolTable.kindOf(name);

      // CORREÇÃO: A lógica correta aqui deve verificar se o identificador NÃO é estático (!= Kind::NONE)
      if (kind != Kind::NONE) {
        vmWriter->writePush(kindToSegment(kind), symbolTable.indexOf(name));
        nArgs = 1;
        name = symbolTable.typeOf(name);
      }

      nArgs += compileExpressionList2();
      processSymbol(")");

      vmWriter->writeCall(name + "." + subName, nArgs);
    } 
    //NOTE cenário 6.3: Função/Method local da própria classe
    else if (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == "(") {
      processSymbol("(");
      vmWriter->writePush(Segment::POINTER, 0);
      int nArgs = 1 + compileExpressionList2();
      processSymbol(")");

      vmWriter->writeCall(className + "." + name, nArgs);
    }
    //NOTE cenário 6.4: É apenas uma variável simples
    else {
      Kind kind = symbolTable.kindOf(name);
      int idx = symbolTable.indexOf(name);
      if (kind != Kind::NONE) {
        vmWriter->writePush(kindToSegment(kind), idx);
      }
    }
  }

  indent.erase(indent.length() - 2);                                   //CODXML
  writeXML("</term>");                                                 //CODXML
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

//SUBITEM MARK: COMPILE EXPRESSION LIST 2
int CompilationEngine::compileExpressionList2() {
  int count = 0;
  if (!(tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ")")) {
      compileExpression();
      count++;
      while (tokenizer.tokenType() == TokenType2::SYMBOL && tokenizer.getToken() == ",") {
          processSymbol(",");
          compileExpression();
          count++;
      }
  }
  return count;
}