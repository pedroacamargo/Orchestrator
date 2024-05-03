#!/bin/bash

# Número de vezes que o conjunto de processos será executado
num_execucoes=3

# Loop para executar o conjunto de processos três vezes
for ((i = 1; i <= $num_execucoes; i++)); do
    # Executa o primeiro comando em segundo plano
    ./client execute 3 -u "sleep 4" &

    # Aguarda 4 segundos para o primeiro comando iniciar
    sleep 0.5

    # Executa o segundo comando em segundo plano
    ./client execute 15 -u "sleep 10" &

    # Aguarda 1 segundo para o segundo comando iniciar
    sleep 0.5

    # Executa o terceiro comando em segundo plano
    ./client execute 1 -u "sleep 1" &

    # Aguarda 1 segundo para o terceiro comando iniciar
    sleep 0.5

    # Executa o quarto comando em segundo plano
    ./client execute 5 -u "sleep 5" &

    # Aguarda 5 segundos para o quarto comando iniciar
    sleep 0.5

    # Executa o quinto comando em segundo plano
    ./client execute 10 -u "sleep 7" &

    # Espera todos os processos em segundo plano terminarem
    wait
done
