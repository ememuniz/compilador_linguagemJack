#include "JackTokenizer.h"
#include <fstream> //Biblioteca que le o arquivo
#include <sstream> //Biblioteca que manipula blocos de texto 

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
  std::string symbols = "{}()[].,;+-*/&|<>=~";    //Armazena os simbolos

  if (symbols.find(c) != std::string::npos){     //Se o caractere atual for um simbolo
    currentToken = c;                            //Adiciona o caractere ao token atual
    currentTokenType = SYMBOL;                   //Define o tipo de token atual como simbolo
    position++;                                  //Avance para o proximo caractere
    return;                                      //Finaliza a funcao
  }

  //Temporario para não ficar em looping
  position++;
}

//Função de teste
std::string JackTokenizer::getCode(){return input_code;}           //Retorna o conteudo do arquivo
TokenTypeList JackTokenizer::tokenType(){return currentTokenType;} //Retorna o tipo de token atual
std::string JackTokenizer::getToken(){return currentToken;}        //Retorna o token atual

