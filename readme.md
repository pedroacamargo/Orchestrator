Temos que fazer 2 formas de escalonamento:
FCFS
SJF

- Sistema de pipes


"dando uma indicação da duração em milissegundos que necessitam para a mesma"
-> Guardar a informação numa file pra ver qual o maior/menor


"Informação produzida pelas tarefas para o standard output ou standard error devem ser redirecionadas pelo servidor para um ficheiro cujo nome corresponde ao identificador da tarefa."


------------------------------------------------------------------------------------------------------------------------------------------------
Execução de tarefas do utilizador
- indicação do tempo que o utilizador acha que é necessário para executar a sua tarefa

->  execute time -u "prog-a [args]"      (Argumentos variáveis, tamanho total 300 dos [args] nao pode ser > 300)
-> Criar um PID para o processo a executar pedido pelo client
-> Após a terminação de uma tarefa, o servidor deve registar em ficheiro, de forma persistente, o identificador da tarefa e o seu tempo de execução
(gettimeofday(&t1, NULL);)

Pedro: Arquitetura do projeto, escalonamento FCFS
Moniz: Salvar as files
Carlão: escalonamento SJF


depois eu atualizo :)
