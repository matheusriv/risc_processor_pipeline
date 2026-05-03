# Simulador de Processador Pipeline MIPS

Este projeto implementa um simulador de um processador MIPS de 5 estágios com pipeline, desenvolvido em SystemC. Ele inclui suporte para detecção de hazards de dados, unidade de adiantamento (forwarding) e tratamento de desvios.

## Pré-requisitos

Para compilar e executar o projeto, você precisa ter o **SystemC** instalado no seu sistema.

## Configuração

Antes de compilar, é necessário configurar a variável de ambiente `SYSTEMC_PATH` para que ela aponte para o diretório de instalação do SystemC.

Exemplo en sistema baseados em Unix:
```sh
export SYSTEMC_PATH="/usr/local/systemc"
```

## Instruções Suportadas
* **Aritméticas e Lógicas:**
    * `add`: Soma entre dois registradores.
    * `sub`: Subtração entre dois registradores.
    * `addi`: Soma imediata (soma um valor constante a um registrador).
    * `and`, `or`, `xor`, `not`: Operações lógicas.
* **Acesso à Memória:**
    * `ld` (Load): Carrega um valor da memória para um registrador.
    * `st` (Store): Armazena o valor de um registrador na memória.
* **Controle de Fluxo (Jumps):**
    * `j`: Salto incondicional para um endereço específico.
    * `jz`: Salto condicional se o resultado da última operação for zero.
    * `jn`: Salto condicional se o resultado da última operação for negativo.

## Testando o Processador
O processador se encontra em `processor.cpp`. Para rodar o teste, executando com um arquivo com instruções, siga o exemplo:
```sh
make run PROG=programs/sub_1.txt
```