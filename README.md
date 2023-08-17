# Montador e Ligador SB
1° Trabalho da disciplina Software Básico (SB) realizado no 1° semestre de 2022 na Universidade de Brasília (UnB).
O programa apresenta um montador que traduz um código assembly simplificado para código objeto, realizando pré-processamento e detecção de erros léxicos, sintáticos e semânticos. Além disso, é apresentado um ligador que consegue ligar dois arquivos objetos gerados pelo montador. O trabalho foi desenvolvido em linguagem C++ em sistema operacional Ubuntu.
- Aluno: Vinícius Caixeta de Souza
- Matrícula: 180132199
- Versão do G++: 9.4.0

Para compilar o montador e ligador digite no terminal:
```
g++ Montador_180132199.cpp -o montador
g++ Ligador_180132199.cpp -o ligador
```
O montador recebe três argumentos, os últimos dois argumentos são os arquivos de entrada e saída e o primeiro parâmetro indica o funcionamento do montador a partir dos seguintes argumentos:
- -p: Pré-processa o arquivo .asm, resultando no arquivo ArquivoPre.asm
- -o: Traduz o .asm, gerando um arquivo .obj

O ligador recebe dois argumentos, sendo eles os arquivos .obj que serão ligados em um único arquivo.
