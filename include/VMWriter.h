#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>

//CONTAINER MARK: ENUM SEGMENT 
enum class Segment {
  CONSTANT,
  ARGUMENT,
  LOCAL,
  STATIC,
  THIS,
  THAT,
  POINTER,
  TEMP
};
//CONTAINER MARK: ENUM COMMAND
enum class Command {
  ADD,
  SUB,
  NEG,
  EQ,
  GT,
  LT,
  AND,
  OR,
  NOT
};
//CONTAINER MARK: VMWRITER
class VMWriter {
  //SUBITEM VARIÁVEIS PRIVADAS
  private:
    std::ofstream out;
    std::string segmentToString(Segment seg);                     // Função para converter o enum Segment em string
    std::string commandToString(Command cmd);                     // Função para converter o enum Command em string
  //SUBITEM VARIÁVEIS PÚBLICAS
  public:
    VMWriter(const std::string& outputFile);                      // Construtor que recebe o nome do arquivo de saída
    ~VMWriter();                                                  // Destrutor para fechar o arquivo

    void writePush(Segment segment, int index);                   // Escreve um comando push no arquivo de saída
    void writePop(Segment segment, int index);                    // Escreve um comando pop no arquivo de saída
    void writeArithmetic(Command command);                        // Escreve um comando aritmético no arquivo de saída
    void writeLabel(const std::string& label);                    // Escreve um comando de label no arquivo de saída
    void writeGoto(const std::string& label);                     // Escreve um comando de goto no arquivo de saída
    void writeIf(const std::string& label);                       // Escreve um comando de if-goto no arquivo de saída
    void writeCall(const std::string& name, int nArgs);           // Escreve um comando de chamada de função no arquivo de saída
    void writeFunction(const std::string& name, int nLocals);     // Escreve um comando de declaração de função no arquivo de saída
    void writeReturn();                                           // Escreve um comando de retorno no arquivo de saída

    void close();                                                 // Fecha o arquivo de saída

};


#endif