## Repositório de SO
Link para o vídeo: https://youtu.be/ds1sCGkhvl8

# Instruções
Para utilizar o arquivo de casos de teste no programa basta compilar o arquivo principal (Trabalho3_SO.cpp)
e utilizar o seguinte comando:
./executavelDoPrograma < casosDeTeste.txt

## Comandos
No arquivo de casos de teste, o '#' é usado para comentários. Linhas iniciadas por '#' não fazem parte do código.
As instruções que o gerenciador é capaz de realizar são:
- C: cria um novo processo com um ID e tamanho (em número de páginas) especificado.<br/>
	Ex.: P1 C 2 cria um processo com ID 1 e tamanho de duas páginas.

- R: 'lê' (apenas simula uma operação de leitura) o endereço do processo especificado.<br/>
	Ex.: P1 R 0000000000010 lê o endereço lógico 0000000000010 do processo de ID 1.
	Nota: é possível definir o tamanho de cada página virtual, o número de páginas e o tamanho da memória.
	Sendo os 10 últimos bits o endereço lógico na página e os demais bits o número da página.

- W: 'escreve' (apenas simula uma operação de escrita) no endereço especificado.<br/>
	Ex.: P1 W 0010000000000 escreve no endereço lógico 0010000000000 do processo de ID 1.

O gerenciador também simula operações de I/O e CPU, com os comandos:
- I: simula uma execução de instrução de I/O.<br/>
	Ex.: P1 I 2

- P: simula uma execução de instrução de CPU.<br/>
	Ex.: P1 P 1
