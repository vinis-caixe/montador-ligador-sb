/*
Trabalho 1 - Software Básico - Ligador
Vinícius Caixeta de Souza
180132199
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct tabelaDef{
    string simbolo;
    int valor;
};

struct tabelaUso{
    string simbolo;
    int endereco;
};

int main(int argc, char *argv[]){
    vector<tabelaUso> tabUso1;
    vector<tabelaUso> tabUso2;
    vector<tabelaDef> tabDef1;
    vector<tabelaDef> tabDef2;
    vector<tabelaDef> tabDefGlobal;
    vector<int> endRelativo1;
    vector<int> endRelativo2;

    int i, j, fatorDeCorrecao = 0;

    string linha;
    string simbolo;
    string valor;
    string objetoLigado;
    bool secao[3];

    ifstream arquivoObjeto1(argv[1]);
    ifstream arquivoObjeto2(argv[2]);

    // Obtem dados de tabela de use, definição e endereços relativos do primeiro arquivo objeto
    while(getline(arquivoObjeto1, linha)){
        if(linha == "TABELA USO"){ secao[0] = true; continue; }
        if(secao[0] == true){
            if(linha.empty()){ secao[0] = false; continue; }

            istringstream tabela(linha);
            getline(tabela, simbolo, ' ');
            getline(tabela, valor, ' ');
            tabUso1.push_back(tabelaUso{simbolo, stoi(valor)});
            continue;
        }

        if(linha == "TABELA DEF"){ secao[1] = true; continue; }
        if(secao[1] == true){
            if(linha.empty()){ secao[1] = false; continue; }

            istringstream tabela(linha);
            getline(tabela, simbolo, ' ');
            getline(tabela, valor, ' ');
            tabDef1.push_back(tabelaDef{simbolo, stoi(valor)});
            continue;
        }

        if(linha == "ENDERECOS RELATIVOS"){ secao[2] = true; continue; }
        if(secao[2] == true){
            if(linha.empty()){ secao[2] = false; continue;}

            istringstream tabela(linha);
            while(getline(tabela, valor, ' ')){
                endRelativo1.push_back(stoi(valor));
            }
            continue;
        }

        objetoLigado = linha;
    }

    // Determina fator de correção
    for(i = 0; i < objetoLigado.size(); i++){
        if(objetoLigado[i] == ' '){ fatorDeCorrecao++; }
    }

    // Obtem dados de tabela de use, definição e endereços relativos do segundo arquivo objeto
    while(getline(arquivoObjeto2, linha)){
        if(linha == "TABELA USO"){ secao[0] = true; continue; }
        if(secao[0] == true){
            if(linha.empty()){ secao[0] = false; continue; }

            istringstream tabela(linha);
            getline(tabela, simbolo, ' ');
            getline(tabela, valor, ' ');
            tabUso2.push_back(tabelaUso{simbolo, stoi(valor)});
            continue;
        }

        if(linha == "TABELA DEF"){ secao[1] = true; continue; }
        if(secao[1] == true){
            if(linha.empty()){ secao[1] = false; continue; }

            istringstream tabela(linha);
            getline(tabela, simbolo, ' ');
            getline(tabela, valor, ' ');
            tabDef2.push_back(tabelaDef{simbolo, stoi(valor)});
            continue;
        }

        if(linha == "ENDERECOS RELATIVOS"){ secao[2] = true; continue; }
        if(secao[2] == true){
            if(linha.empty()){ secao[2] = false; continue;}

            istringstream tabela(linha);
            while(getline(tabela, valor, ' ')){
                endRelativo2.push_back(stoi(valor));
            }
            continue;
        }

        objetoLigado = objetoLigado + linha;
    }

    // Cria a tabela de definições global
    for(i = 0; i < tabDef1.size(); i++){
        tabDefGlobal.push_back(tabDef1[i]);
    }
    for(i = 0; i < tabDef2.size(); i++){
        tabDefGlobal.push_back(tabelaDef{tabDef2[i].simbolo, (tabDef2[i].valor + fatorDeCorrecao)});
    }

    // Remove endereços relativos que já estão na tabela de uso
    for(i = 0; i < tabUso2.size(); i++){
        for(j = 0; j < endRelativo2.size(); j++){
            if(tabUso2[i].endereco == endRelativo2[j]){
                endRelativo2.erase(endRelativo2.begin()+j);
                break;
            }
        }
    }

    arquivoObjeto1.close();
    arquivoObjeto2.close();

    // Processo de correção de endereços
    int aux, contadorPosicao = 0;
    string token;
    istringstream iss(objetoLigado);
    ofstream objetosLigados("objetosLigados.obj");

    while(getline(iss, token, ' ')){
        // Corrige endereços a partir da tabelo de uso do primeiro objeto
        for(i = 0; i < tabUso1.size(); i++){
            if(tabUso1[i].endereco == contadorPosicao){
                for(j = 0; j < tabDefGlobal.size(); j++){
                    if(tabDefGlobal[j].simbolo == tabUso1[i].simbolo){
                        aux = stoi(token) + tabDefGlobal[j].valor;
                        token = to_string(aux);
                    }
                }
            }
        }

        // Corrige endereços a partir da tabelo de uso do segundo objeto
        for(i = 0; i < tabUso2.size(); i++){
            if((tabUso2[i].endereco + fatorDeCorrecao) == contadorPosicao){
                for(j = 0; j < tabDefGlobal.size(); j++){
                    if(tabDefGlobal[j].simbolo == tabUso2[i].simbolo){
                        token = to_string(tabDefGlobal[j].valor);
                    }
                }
            }
        }

        // Corrige endereços relativos do segundo objeto que não estão em sua tabela de uso
        for(i = 0; i < endRelativo2.size(); i++){
            if((endRelativo2[i] + fatorDeCorrecao) == contadorPosicao){
                aux = stoi(token) + fatorDeCorrecao;
                token = to_string(aux);
                endRelativo2.erase(endRelativo2.begin()+i);
                break;
            }
        }

        contadorPosicao++;

        objetosLigados << token << ' ';
    }

    objetosLigados.close();

    return 0;
}