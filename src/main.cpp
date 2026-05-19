#include <iostream>
#include <string>
#include "JackTokenizer.h"
#include "CompilationEngine.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo.jack>" << std::endl;
        return 1;
    }

    std::string arquivoEntrada = argv[1];
    
    // Validação simples de extensão
    if (arquivoEntrada.substr(arquivoEntrada.find_last_of(".") + 1) != "jack") {
        std::cerr << "Erro: O arquivo precisa ter a extensao .jack" << std::endl;
        return 1;
    }

    // Define o nome do arquivo de saída mudando .jack para .xml
    std::string arquivoSaida = arquivoEntrada.substr(0, arquivoEntrada.find_last_of(".")) + ".xml";

    std::cout << "Compilando: " << arquivoEntrada << " -> " << arquivoSaida << "..." << std::endl;

    try {
        // 1. Inicializa o Analisador Léxico
        JackTokenizer tokenizer(arquivoEntrada);

        // 2. Inicializa o Analisador Sintático
        CompilationEngine engine(tokenizer, arquivoSaida);

        // 3. Começa a tradução a partir da regra raiz (class)
        engine.compileClass();

        std::cout << "Compilacao concluida com sucesso!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erro durante a compilacao: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}