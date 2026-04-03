#include "JackTokenizer.h"
#include <fstream> //Biblioteca que le o arquivo
#include <sstream> //Biblioteca que manipula blocos de texto 
#include <cctype>  //Biblioteca que trabalha com caracteres e tem funcoes para trabalhar com eles

//Construtor da classe JackTokenizer
JackTokenizer::JackTokenizer(std::string filename){
  std::ifstream file(filename); //Abre o arquivo

  if (file.is_open()){
    std::stringstream buffer; //Cria um buffer para armazenar o conteudo do arquivo
    buffer << file.rdbuf(); //Armazena o conteudo do arquivo no buffer
    input_code = buffer.str(); //Transforma o buffer em uma string e salva
    file.close(); //Fecha o arquivo
  } else {
    input_code = ""; // Se der erro, fica vazio
  }
  removeComments();

  //variaveis
  position = 0;               //Armazena a posicao atual do arquivo, caractere a caractere
  currentToken = "";          //Armazena o token atual
  currentTokenType = NONE;    //Armazena o tipo de token atual
}

//Função para verificar se leu o arquivo todo
bool JackTokenizer::hasMoreTokens(){return position < input_code.length();} //Retorna True se a posição atual for menor que o tamanho total do arquivo. Se nao, retorna False


void JackTokenizer::advance(){
  currentToken = "";          //Limpa o token atual

  while(position < input_code.length() && input_code[position] == ' '){
    position++;
  }                           //Enquanto ainda tiver caracteres a serem lidos e for um espaço, avance.

  if (!hasMoreTokens()) return; //Se ja estiver terminado o texto do arquivo, finalize.

  //Variáveis de armazenamento
  char c = input_code[position];                  //Armazena o caractere na posicao atual

  // SIMBOLOS
  std::string symbols = "{}()[].,;+-*/&|<>=~";    //Armazena os simbolos
  if (symbols.find(c) != std::string::npos){     //Se o caractere atual for um simbolo
    currentToken = c;                            //Adiciona o caractere ao token atual
    currentTokenType = SYMBOL;                   //Define o tipo de token atual como simbolo
    position++;                                  //Avance para o proximo caractere
    return;                                      //Finaliza a funcao
  }

  //NUMEROS
  if(isdigit(c)){                    //Se o caractere atual for um digito
    while (position < input_code.length() && isdigit(input_code[position])){
      currentToken += input_code[position]; 
      position++;
    }                                //Enquanto ainda tiver caracteres a serem lidos e for um digito, adicione ao token atual e avance.
    currentTokenType = INT_CONST;    //Define o tipo de token atual como inteiro
    return;                          //Finaliza a funcao
  }

  //STRING
  if (c == '"'){                        //Se o caractere atual for uma aspas duplas
    position++;                         //Avance para o proximo caractere
    while (position < input_code.length() && input_code[position] != '"'){
      currentToken += input_code[position]; 
      position++;
    }                                   //Enquanto ainda tiver caracteres a serem lidos e nao for uma aspas duplas, adicione ao token atual e avance.
    position++;                         //Avance para o proximo caractere
    currentTokenType = STRING_CONST;    //Define o tipo de token atual como string
    return;                             //Finaliza a funcao
  }

  //IDENTIFICADORES E KEYWORDS
  if (isalpha(c) || c == '_'){            //Se o caractere atual for uma letra ou underline

    //Identificadores
    while(position < input_code.length() && (isalnum(input_code[position]) || input_code[position] == '_')){
      currentToken += input_code[position]; 
      position++;
    }    //enquanto ainda tiver caracteres a serem lidos e for uma letra ou underline, adicione ao token atual e avance.

    //keywords
    std::string keywords[] = {
      "class",
      "constructor",
      "function",
      "method",
      "field",
      "static",
      "var",
      "int",
      "char",
      "boolean",
      "void",
      "true",
      "false",
      "null",
      "this",
      "let",
      "do",
      "if",
      "else",
      "while",
      "return", 
    };

    currentTokenType = IDENTIFIER;       //Define o tipo de token atual como identificador
    for(const std::string& kw : keywords){
      if(currentToken == kw){
        currentTokenType = KEYWORD;       //Se o token atual for uma keyword, define o tipo de token atual como keyword
        break;
      }
    } //veifica se o token atual é uma das palavras dentro da lista de palavras-chave 

    return; //Achou o token, sai da função
  }
  //Temporario para não ficar em looping
  position++;
  advance();  //Evita caracteres indesejados
}

void JackTokenizer::removeComments(){
  std::string cleaned_code = "";
  bool inBlockComment = false; //Memória para saber se estamos dentro de um comentário de bloco /* . . . */

  for (size_t i = 0; i < input_code.length(); i++) { 
    // Comentário de blocos 
    if (!inBlockComment && input_code[i] == '/' && i + 1 < input_code.length() && input_code[i + 1] == '*'){
      inBlockComment = true;
      i++; //Pulamos o asterisco
      continue;
    }  //Se não estamos em um comentário de blocos e o caractere atual for uma barra e o caractere seguinte for um asterisco, então iniciamos um comentario de blocos, e avancemos para o proximo caracteree continuamos o loop.

    if (inBlockComment && input_code[i] == '*' && i + 1 < input_code.length() && input_code[i + 1] == '/'){
      inBlockComment = false;
      i++; //Pulamos a barra
      continue;
    }

    //Se estamos dentro de um bloco, ignora a letra atual e vai pra próxima
    if (inBlockComment) continue;

    //Comentário de linha (//)
    if (input_code[i] == '/' && i + 1 < input_code.length() && input_code[i + 1] == '/'){
      while(i < input_code.length() && input_code[i] != '\n'){
        i++;
      }

      cleaned_code += '\n';  //Ao achar a quebra de linha, mantemos para o cód não grudar
      continue;
    }

    cleaned_code += input_code[i];
  } //vai de zero até o tamanho do input_code que é o tamanho do arquivo, então vai percorrer ele todo

  input_code = cleaned_code; //Substitui o código sujo pelo código limpo. 
}

//Função de teste
std::string JackTokenizer::getCode(){return input_code;}           //Retorna o conteudo do arquivo
TokenTypeList JackTokenizer::tokenType(){return currentTokenType;} //Retorna o tipo de token atual
std::string JackTokenizer::getToken(){return currentToken;}        //Retorna o token atual

