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
}



//Função de teste
std::string JackTokenizer::getCode(){return input_code;} //Retorna o conteudo do arquivo

