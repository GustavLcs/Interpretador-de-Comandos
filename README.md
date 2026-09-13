# Interpretador de Comandos

## Descrição
Implementação de um programa de sistemas que funciona como um interpretador de linha de comando
(shell) executando comandos do sistema operacional, manipulando processos e gerenciando entrada e saída.

## Comandos Internos
O terminal de linha de comandos é responsável por executar comandos de programação instalados no sistema. Além
dos programas instalado, a shell possui um conjunto de comandos que ela reconhece e executa. Estes comandos
são chamados de comandos internos da shell. Comando implementados: exit, cd, pwd e history -c [n].

## Comandos externos
Para executar comandos externos, a shell procura o nome do comando digitado, que deve ser o nome de um 
arquivo em um diretório. Ela armazena uma lista com os caminhos completos dos diretório onde 
procura o comando a ser executado. Há uma variável de ambiente, chamada PATH que 
armazena uma string contendo todos os diretórios separados por ':' (dois pontos). Ao modificar essa 
variável, altera-se a lista de diretórios onde a shell procura os diretórios (ver função: "find_command(string)" ).
