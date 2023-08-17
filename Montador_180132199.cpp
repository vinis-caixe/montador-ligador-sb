/*
Trabalho 1 - Software Básico - Montador
Vinícius Caixeta de Souza
180132199
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string.h>
#include <algorithm>

using namespace std;

struct linhaPreProcesso{
    int linha;
    int token;
};

struct tabelaEQU{
    int numero;
    string label;
};

struct tabelaInst{
    string nome;
    string opcode;
    int tamanho;
};

struct tabelaSimbolos{
    string nome;
    int valor;
    bool externo;
};

struct tabelaDef{
    string simbolo;
    int valor;
};

struct tabelaUso{
    string simbolo;
    int endereco;
};

bool bothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

// Determina em qual linha do arquivo original ocorreu o erro
int linhaErro(int token, vector<linhaPreProcesso> linhas){
    int i;

    for(i = 0; i < linhas.size(); i++){
        if(token <= linhas[i].token){
            return linhas[i].linha;
        }
    }

    return -1;
}

int main(int argc, char *argv[]){
    int i, j;
    bool pular = false;
    string linha;
    string token;
    vector<string> tokens;
    vector<tabelaEQU> EQU;

    vector<tabelaInst> inst;

    int linhas = 0;
    int tokensTotal = 0;
    vector<linhaPreProcesso> linhaPre;

    inst.push_back(tabelaInst{"ADD", "1", 2}); inst.push_back(tabelaInst{"SUB", "2", 2}); inst.push_back(tabelaInst{"MULT", "3", 2}); inst.push_back(tabelaInst{"DIV", "4", 2});
    inst.push_back(tabelaInst{"JMP", "5", 2}); inst.push_back(tabelaInst{"JMPN", "6", 2}); inst.push_back(tabelaInst{"JMPP", "7", 2}); inst.push_back(tabelaInst{"JMPZ", "8", 2});
    inst.push_back(tabelaInst{"COPY", "9", 3}); inst.push_back(tabelaInst{"LOAD", "10", 2}); inst.push_back(tabelaInst{"STORE", "11", 2}); inst.push_back(tabelaInst{"INPUT", "12", 2});
    inst.push_back(tabelaInst{"OUTPUT", "13", 2}); inst.push_back(tabelaInst{"STOP", "14", 1});

    ifstream arquivoAsm(argv[2]);
    ofstream arquivoAsmPre("ArquivoPre.asm");

    /*========= Pre-Processamento =========*/
    while(getline(arquivoAsm, linha)){
        linhas++;

        if(linha.empty()){ continue; }

        // Pula linha se IF for verdadeiro
        if(pular == true){
            pular = false;
            continue;
        }

        // Remove espaços desnecessários
        string::iterator new_end = unique(linha.begin(), linha.end(), bothAreSpaces);
        linha.erase(new_end, linha.end());
        linha.erase(remove(linha.begin(), linha.end(), '\t'), linha.end());

        // Transforma toda linha em maiúscula
        transform(linha.begin(), linha.end(), linha.begin(), ::toupper);

        istringstream iss(linha);

        while(getline(iss, token, ' ')){
            if(token.empty()){ continue; }
            
            // Não coloca comentários
            if(token[0] == ';'){ break; }

            // Substitui a label se for relacionada a EQU
            for(i = 0; i < EQU.size(); i++){
                if(token == EQU[i].label){
                    tokens.push_back(to_string(EQU[i].numero));
                    goto cnt;
                }
            }

            tokens.push_back(token);
            cnt:;
        }

        if(tokens.size() > 1){
            if(tokens[1] == "EQU"){
                // Adiciona equivalência de EQU em uma tabela
                EQU.push_back(tabelaEQU{stoi(tokens[2]), tokens[0].substr(0, tokens[0].size()-1)});
                tokens.clear();
                continue;
            }
            // Determina se próxima linha deve ser pulada
            if(tokens[0] == "IF"){
                if(tokens[1] == "0"){ pular = true; }
                tokens.clear();
                continue;
            }
        }

        tokensTotal = tokensTotal + tokens.size();
        // Coloca em uma tabela a linha em que determinados tokens estão antes do pré-processamento
        linhaPre.push_back(linhaPreProcesso{linhas, tokensTotal});

        for(i = 0; i < tokens.size(); i++){ arquivoAsmPre << tokens[i] << ' '; }

        tokens.clear();
    }

    arquivoAsmPre << '\n';

    arquivoAsm.close();
    arquivoAsmPre.close();

    // Termina programa caso seja solicitado somente pré-processamento
    if(strcmp(argv[1], "-p") == 0){
        return 0;
    }

    ifstream arquivoPre("ArquivoPre.asm");
    ofstream arquivoObjeto(argv[3]);

    /*========= Montagem =========*/
    int contadorPosicao = 0;
    int contadorTokens = 0;
    bool simboloAnterior = false;
    bool publicToken = false;
    bool beginExiste = false;
    string simbolo;

    vector<tabelaSimbolos> simbolos;
    vector<tabelaUso> tabUso;
    vector<tabelaDef> tabDef;

    getline(arquivoPre, linha);

    istringstream iss(linha);

    /*====== Primeira passagem ======*/
    while(getline(iss, token, ' ')){
        contadorTokens++;

        // Adiciona símbolo na tabela de definições
        if(publicToken == true){
            tabDef.push_back(tabelaDef{token, 0});
            publicToken = false;
        }

        // Adiciona símbolo na tabela de símbolos de acordo com sua posição e caso seja externo
        if(simboloAnterior == true){
            if(token == "EXTERN"){
                simbolos.push_back(tabelaSimbolos{simbolo, contadorPosicao, true});
            }
            else{
                simbolos.push_back(tabelaSimbolos{simbolo, contadorPosicao, false});
            }
        }

        if(token.back() == ':'){
            // Verificação de erros para símbolos
            if(isdigit(token[0])){
                cout << "Erro léxico: símbolo " << token.substr(0, token.size()-1) << " começa com número. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
            }
            if (token.substr(0, token.size()-1).find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") != string::npos){
                cout << "Erro léxico: símbolo " << token.substr(0, token.size()-1) << " possui caractere especial. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
            }

            for(i = 0; i < simbolos.size(); i++){
                if(simbolos[i].nome == token.substr(0, token.size()-1)){
                    cout << "Erro semântico: símbolo " << token.substr(0, token.size()-1) << " já foi declarado anteriormente. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                    goto cnt1;
                }
            }

            if(simboloAnterior == true){
                cout << "Erro sintático: símbolo " << token.substr(0, token.size()-1) << " foi declarado na mesma linha que outro. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                simboloAnterior = false;
                simbolos.pop_back();
                continue;
            }

            simbolo = token.substr(0, token.size()-1);

            simboloAnterior = true;
            continue;
        }
        else{
            simboloAnterior = false;
        }

        for(i = 0; i < inst.size(); i++){
            if(token == inst[i].nome){
                contadorPosicao = contadorPosicao + inst[i].tamanho;
                break;
            }
        }

        if(token == "PUBLIC"){
            publicToken = true;
            continue;
        }

        if(token == "BEGIN"){
            beginExiste = true;
            continue;
        }

        if(token == "SPACE" || token == "CONST"){ contadorPosicao++; }

        cnt1:;
    }

    // Atualiza os valores da tabela de definição
    for(i = 0; i < simbolos.size(); i++){
        for(j = 0; j < tabDef.size(); j++){
            if(tabDef[j].simbolo == simbolos[i].nome){ tabDef[j].valor = simbolos[i].valor; }
        }
    }

    /* ====== Segunda passagem ======*/
    string secaoAtual;
    bool secao = false;
    bool secaoTextoDeclarada = false;

    bool instDeclarada = false;
    int instTamanho;

    bool operando = false;
    bool constDeclarada = false;
    int hexToInt;

    vector<int> enderecosRelativos;

    contadorPosicao = 0;
    contadorTokens = 0;
    istringstream iss1(linha);
    string objetoGerado;

    while(getline(iss1, token, ' ')){
        contadorTokens++;

        // Ignora rótulos
        if(token.back() == ':'){ continue; }

        if(instDeclarada == true){
            instTamanho--;
            if(instTamanho == 0){
                instDeclarada = false;
            }
            else{
                // Verificação de erros para operandos
                if(token.back() == ','){ 
                    token = token.substr(0, token.size()-1);
                    if(instTamanho <= 1){
                        cout << "Erro sintático: quantidade de operandos incorreta. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                        instTamanho++;
                    }
                }

                if(token == "SPACE" || token == "CONST" || token == "BEGIN" || token == "END" || token == "EXTERN" || token == "PUBLIC" || token == "SECAO" ||
                token == "TEXTO" || token == "DADOS"){
                    cout << "Erro sintático: quantidade de operandos incorreta. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                    goto cont2;
                }

                for(i = 0; i < inst.size(); i++){
                    if(inst[i].nome == token){
                        cout << "Erro sintático: quantidade de operandos incorreta. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                        goto cont2;
                    }
                }

                // Verifica se operando está na tabela de símbolos, caso sim coloca seu valor no código objeto
                for(i = 0; i < simbolos.size(); i++){
                    if(simbolos[i].nome == token){
                        operando = true;
                        objetoGerado = objetoGerado + to_string(simbolos[i].valor) + ' ';
                        // Coloca símbolo na tabela de uso
                        if(simbolos[i].externo == true){
                            tabUso.push_back(tabelaUso{simbolos[i].nome, contadorPosicao});
                        }
                        enderecosRelativos.push_back(contadorPosicao);
                        contadorPosicao++;
                        break;
                    }
                }
                if(operando == false){
                    
                    cout << "Erro sintático: operando inválido. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                    continue;
                }
                operando = false;
            }
        }

        cont2:;

        // Verificação de seção atual
        if(token == "TEXTO" && secao == true){
            secaoTextoDeclarada = true;
            secao = false;
            secaoAtual = "TEXTO";
            continue;
        }
        if(token == "DADOS" && secao == true){
            secao = false;
            secaoAtual = "DADOS";
            continue;
        }
        if(token == "SECAO"){
            secao = true;
            continue;
        }

        for(i = 0; i < inst.size(); i++){
            if(token == inst[i].nome){
                // Verificação de erros para seção de texto
                if(secaoAtual == "DADOS"){
                    cout << "Erro semântico: instrução foi declarada na seção de dados. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                }
                if(secaoTextoDeclarada == false){
                    cout << "Erro semântico: código não possui seção texto. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
                }
                
                contadorPosicao++;
                objetoGerado = objetoGerado + inst[i].opcode + ' ';
                instDeclarada = true;
                instTamanho = inst[i].tamanho;
                break;
                
            }
        }

        // Verificação de erros para seção de dados
        if((token == "SPACE" || token == "CONST") && secaoAtual != "DADOS"){
            cout << "Erro semântico: declaração de dados não está na seção de dados. Linha " << linhaErro(contadorTokens, linhaPre) << '\n';
            continue;
        }

        if(token == "SPACE"){
            contadorPosicao++;
            objetoGerado = objetoGerado + "0 ";
            continue;
        }
        if(token == "CONST"){ constDeclarada = true; continue; }
        if(constDeclarada == true){
            // Transforma hexadecimal para decimal
            if(token.size() > 1 && token[1] == 'X'){
                hexToInt = stoul(token, nullptr, 16);
                objetoGerado = objetoGerado + to_string(hexToInt) + ' ';
            }
            else{
                objetoGerado = objetoGerado + token + ' ';
            }
            constDeclarada = false;
            continue;
        }
    }

    // Gera código objeto dependendo do BEGIN
    if(beginExiste == true){
        arquivoObjeto << "TABELA USO\n";
        for(i = 0; i < tabUso.size(); i++){
            arquivoObjeto << tabUso[i].simbolo << ' ' << tabUso[i].endereco << '\n';
        }
        arquivoObjeto << "\nTABELA DEF\n";
        for(i = 0; i < tabDef.size(); i++){
            arquivoObjeto << tabDef[i].simbolo << ' ' << tabDef[i].valor << '\n';
        }
        arquivoObjeto << "\nENDERECOS RELATIVOS\n";
        for(i = 0; i < enderecosRelativos.size(); i++){
            arquivoObjeto << to_string(enderecosRelativos[i]) << ' ';
        }
        arquivoObjeto << "\n\n" << objetoGerado;
    }
    else{
        arquivoObjeto << objetoGerado;
    }

    arquivoPre.close();
    arquivoObjeto.close();

    return 0;
}