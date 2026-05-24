#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

namespace fs = std::filesystem;

//CONTAINER MARK: COMPILAR FICHEIRO
void compilarFicheiro(const std::string& caminhoJack) {
  fs::path p(caminhoJack);                                     // Cria um objeto path a partir do caminho do arquivo .jack
  fs::path caminhoVm = p.parent_path() / (p.stem().string() + ".vm");   // Cria um caminho para o arquivo .vm,  se caminhoJack for "results/teste.Jack", caminhoVm será "results/teste.vm"

  std::cout << "A processar: " << p.filename().string() << " -> " << caminhoVm.filename().string() << std::endl;
  
  try
  {
    JackTokenizer tokenizer(caminhoJack);

    CompilationEngine engine(tokenizer, caminhoVm.string());

    engine.compileClass();
  }
  catch(const std::exception& e)
  {
    std::cerr << "Erro ao compilar " << p.filename().string() << ": " << e.what() << std::endl;
  }
}

//CONTAINER MARK: MAIN
int main(int argc, char* argv[]) {
  if (argc < 2) {
      std::cerr << "Uso: " << argv[0] << " <ficheiro.jack ou diretorio/>" << std::endl;
      return 1;
  }

  std::string caminhoEntrada = argv[1];
  std::vector<std::string> ficheirosParaCompilar;

  // Verifica se o caminho existe
  if (!fs::exists(caminhoEntrada)) {
      std::cerr << "Erro: O caminho especificado nao existe: " << caminhoEntrada << std::endl;
      return 1;
  }

  // Cenário A: O argumento é um diretório/pasta
  if (fs::is_directory(caminhoEntrada)) {
      std::cout << "Diretorio detetado. Procurando ficheiros .jack em: " << caminhoEntrada << std::endl;
      
      for (const auto& entrada : fs::directory_iterator(caminhoEntrada)) {
          if (entrada.is_regular_file() && entrada.path().extension() == ".jack") {
              ficheirosParaCompilar.push_back(entrada.path().string());
          }
      }
      
      if (ficheirosParaCompilar.empty()) {
          std::cout << "Nenhum ficheiro .jack encontrado neste diretorio." << std::endl;
          return 0;
      }
  } 
  // Cenário B: O argumento é um ficheiro único
  else if (fs::is_regular_file(caminhoEntrada)) {
      if (fs::path(caminhoEntrada).extension() != ".jack") {
          std::cerr << "Erro: O ficheiro configurado nao possui a extensao .jack" << std::endl;
          return 1;
      }
      ficheirosParaCompilar.push_back(caminhoEntrada);
  }

  // Executa a compilação em lote para cada ficheiro encontrado
  std::cout << "Total de ficheiros encontrados: " << ficheirosParaCompilar.size() << "\n" << std::endl;
  for (const auto& ficheiro : ficheirosParaCompilar) {
      compilarFicheiro(ficheiro);
  }

  std::cout << "\nProcessamento de lote concluido!" << std::endl;
  return 0;
}