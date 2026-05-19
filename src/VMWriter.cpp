#include "VMWriter.h"
#include <stdexcept>

//CONTAINER MARK: CONSTRUTOR
VMWriter::VMWriter(const std::string& outputFile) {
  out.open(outputFile);
  if (!out.is_open()) {
    throw std::runtime_error("Erro ao abrir o arquivo de saída: " + outputFile);
  }
};
//CONTAINER MARK: DESTRUTOR
VMWriter::~VMWriter() { out.close(); }   //CORRIGIR ERRO, TALVEZ,  EU NAO SEI SE TEM ESSE OUT ANTES

//CONTAINER MARK: SEGMENT TO STRING
std::string VMWriter::segmentToString(Segment seg) {
  switch (seg) {
    case Segment::CONSTANT: return "constant";
    case Segment::ARGUMENT: return "argument";
    case Segment::LOCAL: return "local";
    case Segment::STATIC: return "static";
    case Segment::THIS: return "this";
    case Segment::THAT: return "that";
    case Segment::POINTER: return "pointer";
    case Segment::TEMP: return "temp";
  }
  return "";
}
//CONTAINER MARK: COMMAND TO STRING
std::string VMWriter::commandToString(Command cmd) {
  switch (cmd) {
    case Command::ADD: return "add";
    case Command::SUB: return "sub";
    case Command::NEG: return "neg";
    case Command::EQ: return "eq";
    case Command::GT: return "gt";
    case Command::LT: return "lt";
    case Command::AND: return "and";
    case Command::OR: return "or";
    case Command::NOT: return "not";
  }
  return "";
}
//CONTAINER MARK: WRITE PUSH
void VMWriter::writePush(Segment segment, int index) {
  out << "push " << segmentToString(segment) << " " << index << "\n";
}
//CONTAINER MARK: WRITE POP
void VMWriter::writePop(Segment segment, int index) {
  out << "pop " << segmentToString(segment) << " " << index << "\n";
}
//CONTAINER MARK: WRITE ARITHMETIC
void VMWriter::writeArithmetic(Command command) {
  out << commandToString(command) << "\n";
}
//CONTAINER MARK: WRITE LABEL
void VMWriter::writeLabel(const std::string& label) {
  out << "label " << label << "\n";
}
//CONTAINER MARK: WRITE GOTO
void VMWriter::writeGoto(const std::string& label) {
  out << "goto " << label << "\n";
}
//CONTAINER MARK: WRITE IF
void VMWriter::writeIf(const std::string& label) {
  out << "if-goto " << label << "\n";
}
//CONTAINER MARK: WRITE CALL
void VMWriter::writeCall(const std::string& name, int nArgs) {
  out << "call " << name << " " << nArgs << "\n";
}
//CONTAINER MARK: WRITE FUNCTION
void VMWriter::writeFunction(const std::string& name, int nLocals) {
  out << "function " << name << " " << nLocals << "\n";
}
//CONTAINER MARK: WRITE RETURN
void VMWriter::writeReturn() {
  out << "return\n";
}
//CONTAINER MARK: CLOSE
void VMWriter::close() {
  if (out.is_open()){
    out.close();
  }
}