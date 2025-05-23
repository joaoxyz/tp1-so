# PAGINADOR DE MEMÓRIA - RELATÓRIO

1. Termo de compromisso

Os membros do grupo afirmam que todo o código desenvolvido para este
trabalho é de autoria própria.  Exceto pelo material listado no item
3 deste relatório, os membros do grupo afirmam não ter copiado
material da Internet nem ter obtido código de terceiros.

2. Membros do grupo e alocação de esforço

Preencha as linhas abaixo com o nome e o e-mail dos integrantes do
grupo.  Substitua marcadores `XX` pela contribuição de cada membro
do grupo no desenvolvimento do trabalho (os valores devem somar
100%).

  * Nome <email@domain> XX%
  * Pedro Renato Ferreira da Silva pedrorfs11@gmail.com 50%

3. Referências bibliográficas

- https://opensource.com/article/21/10/linux-timers
- https://www.ibm.com/docs/en/zos/3.1.0?topic=files-ucontexth-context-related-functions
- https://pubs.opengroup.org/onlinepubs/7908799/xsh/ucontext.h.html

4. Estruturas de dados

  1. Descreva e justifique as estruturas de dados utilizadas para
     gerência das threads de espaço do usuário (partes 1, 2 e 5).

    A struct dccthread foi utilizada como representação de uma thread. Nesse sentido, tal estrutura possui três membros, sendo eles: uc, name, e waiting. O membro uc é um ponteiro para um contexto de usuário. No caso do membro name, ele armazena o nome da thread. Por fim, o membro waiting foi criado com o objetivo de ser utilizado para identificar qual thread deve ser esperada no caso da chamada da função dccthread_wait().

  2. Descreva o mecanismo utilizado para sincronizar chamadas de
     dccthread_yield e disparos do temporizador (parte 4).
