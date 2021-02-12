//Inclui o Arquivo de Cabeçalho
#include "thumblator.hpp"

/*./<executavel> <arquivo_entrada>.in <arquivo_saida[opcional]>.out

argc = quantidade de argumentos
argv[0] = ./<executavel>
argv[1] = <arquivo_entrada>.in
argv[2] = <arquivo_saida[opcional]>.out
*/
int main(int argc, char* argv[]){ 
    
    //Mais de 3 argumentos -> ERRO: ./<executavel> <arquivo_entrada>.in <arquivo_saida_1[opcional]>.out<arquivo_saida_2[opcional]>.out ?
    if(argc > 3){
        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: too much parameters.\nAborting.\n";
        return 0;
    }
    //Menos de 2 argumentos -> ERRO: ./<executavel> ?
    else if(argc < 2){
        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: no input file.\nAborting.\n";
        return 0;
    }

    //Abrir arquivo <entrada>.in e verificação
    ifstream input_file(argv[1]);
    if(!(input_file.is_open())){
        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: file doesn't exist.\nAborting.\n";
        return 0;
    }

    //Verificação da extensão do arquivo de entrada
    string input_file_name;
    input_file_name = argv[1];
    if (input_file_name.find(".in") == input_file_name.npos){
        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: invalid extension.\nAborting.\n";
        return 0;
    }

    //Passagem dos elementos do arquivo para a string 'source_code' por meio de uma stringstream
    stringstream auxiliary_buffer;
    auxiliary_buffer << input_file.rdbuf();
    string source_code;
    source_code = auxiliary_buffer.str();

    //v~ Processo de tradução ~v
    
    //p/ percorrer o vetor
    long unsigned int step = 0;
    
    //numero da linha atual
    short line_number = 0;
    
    //opcode de cada linha em formato numerico
    long int numeric_opcode = 0;
    
    //opcode de cada linha em formato de texto
    string opcode;

    //Retirar espaços
    while (source_code.find(' ') != source_code.npos)
        source_code.erase(source_code.find(' '), 1);

    //Decodificação OPCODE -> MNEMONICO
    //enquanto a posição atual(step) for menor que o tamanho da string, faça
    while (step < source_code.size()){
        //enquanto o caractere na posição não for uma quebra de linha OU enquanto não estiver no fim passe o opcode para a string 
        while (source_code[step] != '\n' || step == source_code.size())
            opcode += source_code[step++];
        //Passa o opcode em hexa para um inteiro
        numeric_opcode = strtol(opcode.c_str(), NULL, 16);
        //decodifica as instruções em uma string
        opcode = decodeInstructions(numeric_opcode, ++line_number);
        //se opcode estiver vazio, então houve um erro 
        if (opcode.empty()) return 0;
        //monta a string decodificada ao final da linha original -> substitui o '\n' por ":  <mnemonico>\n"
        source_code.replace(step, 1, ":  " + opcode + "\n");
        //recalcula o passo para a próxima linha
        step += opcode.size() + 4;
        //limpa opcode para a próxima iteração
        opcode.clear();
    }

    ofstream output_file;
    //se um arquivo de saida não foi especificado, use nome_do_arquivo_entrada.out
    if (argc == 2)
        output_file.open(input_file_name.replace(input_file_name.find(".in"), 3, ".out"));
    //se um arquivo de saida foi especificado
    else
        output_file.open(argv[2]);
    //Passa a string decodificada para o arquivo
    output_file << source_code;

    //Fecha os arquivos de entrada/saída abertos
    output_file.close();
    input_file.close();
    return 0;
} 