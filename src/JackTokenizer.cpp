#include "JackTokenizer.h"
#include <fstream> //Biblioteca que le o arquivo
#include <sstream> //Biblioteca que manipula blocos de texto 
#include <cctype>  //Biblioteca que trabalha com caracteres e tem funcoes para trabalhar com eles
#include <stdexcept> //Biblioteca para lançar exceções
#include <algorithm> //Biblioteca para usar funções de manipulação de strings


//CONTAINER MARK: CONSTRUTOR JACKTOKENIZER
JackTokenizer::JackTokenizer(const std::string& filename) {
  currentTokenIndex = 0;                                                  //O token atual começa na posição 0
  std::ifstream file(filename);                                           //Abre o arquivo
  if (!file.is_open()){
    throw std::runtime_error("ERRO: Não foi possível abrir o arquivo " + filename); 
  }                                                                       //Lança um erro se o arquivo não puder ser aberto
  std::stringstream buffer;                                               //Cria um buffer para armazenar o conteudo do arquivo
  buffer << file.rdbuf();                                                 //Armazena o conteudo do arquivo no buffer
  std::string sourceCode = buffer.str();                                  //Transforma o buffer em uma string e salva
  file.close();                                                           //Fecha o arquivo
  this->input_code = sourceCode;                                          //Armazena o código do arquivo na variável input_code

  tokenize(sourceCode);                                                   //Chama a função para tokenizar o código do arquivo
}

//CONTAINER MARK: HASMORETOKENS
bool JackTokenizer::hasMoreTokens() const {
  return currentTokenIndex < tokens.size();
}                                                                         //Verifica se ainda há tokens para serem lidos

//CONTAINER MARK: ADVANCE
void JackTokenizer::advance(){
  if (hasMoreTokens()) {
    currentTokenIndex++;                                                  //Avança para o próximo token
  }
}                                                                         //Avança para o próximo token, se houver mais tokens

//CONTAINER MARK: TOKENTYPE
TokenType2 JackTokenizer::tokenType() const {
  if (currentTokenIndex < tokens.size()) {
    return tokens[currentTokenIndex].type;                                //Retorna o tipo do token atual
  }
  return NONE;                                                            //Se não houver mais tokens, retorna NONE
}                                                                         //Retorna o tipo do token atual se houver mais tokens

//CONTAINER MARK: GETTOKEN
std::string JackTokenizer::getToken() const {
  if (currentTokenIndex < tokens.size()) {
    return tokens[currentTokenIndex].lexeme;                             //Retorna o texto do token atual
  }
  return "";                                                             //Se não houver mais tokens, retorna uma string vazia
}                                                                        //Retorna o texto do token atual se houver mais tokens  

//CONTAINER MARK: PEEK
Token JackTokenizer::peek() const {
  if (currentTokenIndex + 1 < tokens.size()) {
    return tokens[currentTokenIndex + 1];                                 //Retorna o token seguinte ao atual
  }
  return {NONE, ""};                                                      //Se não houver mais tokens, retorna um token vazio
}

//CONTAINER MARK: ISKEYWORD
bool JackTokenizer::isKeyword(const std::string& lexeme) {
  static const std::vector<std::string> keywords = {
    "class", "constructor", "function", "method", "field", "static",
    "var", "int", "char", "boolean", "void", "true", "false",
    "null", "this", "let", "do", "if", "else", "while", "return"
  };
  return std::find(keywords.begin(), keywords.end(), lexeme) != keywords.end();
}                                                                        //Verifica se o texto do token é uma keyword, comparando com a lista de keywords pré-definida e retorna true se for uma keyword e false caso contrário;

//CONTAINER MARK: TOKENIZE
void JackTokenizer::tokenize(const std::string& sourceCode) {
  size_t i = 0;                                                         //Índice para percorrer o código fonte
  size_t length = sourceCode.length();                                  //Comprimento do código fonte

  while (i < length) {
    char ch = sourceCode[i];                                            //Armazena o caractere atual


    //SUBITEM RETIRA ESPAÇO EM BRANCO
    if (std::isspace(ch)) {
      i++;
      continue;
    }                                                                   //Ignora espaços em branco, quebras de linha e tabs

    //SUBITEM RETIRA COMENTÁRIOS
    if (ch == '/') {

      if ( i + 1 < length && sourceCode[i + 1] == '/') {
        i += 2;
        while (i < length && sourceCode[i] != '\n' && sourceCode[i] != '\r'){
          i++;
        }
        continue;
      }                                                                 //Se achar um '/' e o próximo caractere for outro '/' e não for o fim do código, ele passa pra frente da segunda linha e enquanto o caractere atual for diferente de '\n' e '\r', ele continua entendendo como comentário, ou seja, ignora tudo até o final da linha.  Serve para comentários do tipo '// ...'  
      
      else if (i + 1 < length && sourceCode[i + 1] == '*') {
        i += 2;
        while (i + 1 < length && !(sourceCode[i] == '*' && sourceCode[i + 1] == '/')) {
          i++;
        }
        i += 2;                                                         //Pula o '*/'
        continue;
      }                                                                 //Se achar um '/' e o próximo caractere for '*', ele passa pra frente do '*' e enquanto o caractere atual for diferente de '*' seguido de '/', ele continua entendendo como comentário, ou seja, ignora tudo até achar o '*/'. Serve para comentários do tipo '/* ... */'
    }

    //SUBITEM IDENTIFICAÇÃO DE SÍMBOLOS
    std::string symbols = "{}()[].,;+-*/&|<>=~";
    if (symbols.find(ch) != std::string::npos) {
      std::string lexeme(1, ch);                                       //Cria uma string com o símbolo atual
      tokens.push_back({SYMBOL, lexeme});                              //adiciona ao vetor de tokens como tokentype SYMBOL E O lexame o simbolo no caractere atual
      i++;
      continue;
    }                                                                  //Se o caractere atual for um simbolo em symbols,  ele adiciona o token na lista de token com tipo de token SYMBOL e o lexame o caractere atual e depois avança e recomeça o loop

    //SUBITEM IDENTIFICAÇÃO DE STRINGS
    if (ch == '"') {
      std::string lexeme = "";
      i++;
      while (i < length && sourceCode[i] != '"')
      {
        lexeme += sourceCode[i];
        i++;
      }
      i++;
      tokens.push_back({STRING_CONST, lexeme});
      continue;
    }                                                                  //Se o caractere atual for uma aspas, cria-se um lexeme vazio e avança para o próximo caractere. Enquanto o caractere atual não for o final da string e não for outra aspas, o caractere atual é adicionado ao lexeme. Quando chegar nas aspas, o token é adicionado à lista de tokens com tipo STRING_CONST e o lexame é o conteúdo da string, ou seja, o texto entre as aspas. Depois avança e recomeça o loop

    //SUBITEM IDENTIFICAÇÃO DE NÚMEROS
    if (std::isdigit(ch)) {
      std::string lexeme = "";
      while (i < length && std::isdigit(sourceCode[i])){
        lexeme += sourceCode[i];
        i++;
      }
      tokens.push_back({INT_CONST, lexeme});
      continue;
    }                                                                  //Se o caractere atual for um dígito, cria-se um lexeme vazio. Enquanto o caractere atual for um dígito, ele é adicionado ao lexeme. Quando chegar em um caractere que não seja um dígito, o token é adicionado à lista de tokens com tipo INT_CONST e o lexame é o número inteiro lido. Depois recomeça o loop

    //SUBITEM IDENTIFICAÇÃO DE IDENTIFICADORES
    if (std::isalpha(ch) || ch == '_') {
      std::string lexeme = "";
      while (i < length && (std::isalnum(sourceCode[i]) || sourceCode[i] == '_')) {
        lexeme += sourceCode[i];
        i++;
      }
      if (isKeyword(lexeme)) {
        tokens.push_back({KEYWORD, lexeme});
      } else {
        tokens.push_back({IDENTIFIER, lexeme});
      }
      continue;
    }                                                                  //Se o caractere atual for uma letra ou um underscore, cria-se um lexeme vazio. Enquanto o caractere atual for uma letra, um digito ou um underscore, ele é adicionado ao lexeme. Quando chegar em um caractere diferente, geralmente espaço ou simbolo, o token é verificado se esta na lista de keywords, se estiver, o token é adicionado à lista de tokens com tipo KEYWORD e o lexame é o texto lido. Se não estiver, o token é adicionado à lista de tokens com tipo IDENTIFIER e o lexame é o texto lido. Depois recomeça o loop

    i++;
  }
}

//CONTAINER MARK: GENERATETOKENXML7
void JackTokenizer::generateTokenXML(const std::string& outputFilename) const {
  std::ofstream outFile(outputFilename);
  std::string space = " ";
  if (!outFile.is_open()) {
    return;
  }

  outFile << "<tokens>\n";
  
  for (const auto& token : tokens) {
    std::string tagName;
    std::string value = token.lexeme;

    switch (token.type) {
      case KEYWORD:
        tagName = "keyword";
        break;
      case SYMBOL:
        tagName = "symbol";
        if (value == "<") value = "&lt;";
        else if (value == ">") value = "&gt;";
        else if (value == "&") value = "&amp;";
        break;
      case IDENTIFIER:
        tagName = "identifier";
        break;
      case INT_CONST:
        tagName = "integerConstant";
        break;
      case STRING_CONST:
        tagName = "stringConstant";
        break;
      default:
        continue;
    }

    


    outFile << "<" << tagName << "> " << value << " </" << tagName << ">\n";
  }
  outFile << "</tokens>\n";
  
}