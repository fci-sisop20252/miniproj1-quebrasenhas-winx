#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include "hash_utils.h"


/**
 * PROCESSO COORDENADOR - Mini-Projeto 1: Quebra de Senhas Paralelo
 *
 * Este programa coordena múltiplos workers para quebrar senhas MD5 em paralelo.
 * O MD5 JÁ ESTÁ IMPLEMENTADO - você deve focar na paralelização (fork/exec/wait).
 *
 * Uso: ./coordinator <hash_md5> <tamanho> <charset> <num_workers>
 *
 * Exemplo: ./coordinator "900150983cd24fb0d6963f7d28e17f72" 3 "abc" 4
 *
 * SEU TRABALHO: Implementar os TODOs marcados abaixo
 */


#define MAX_WORKERS 16
#define RESULT_FILE "password_found.txt"


/**
 * Calcula o tamanho total do espaço de busca
 *
 * @param charset_len Tamanho do conjunto de caracteres
 * @param password_len Comprimento da senha
 * @return Número total de combinações possíveis
 */
long long calculate_search_space(int charset_len, int password_len) {
    long long total = 1;
    for (int i = 0; i < password_len; i++) {
        total *= charset_len;
    }
    return total;
}


/**
 * Converte um índice numérico para uma senha
 * Usado para definir os limites de cada worker
 *
 * @param index Índice numérico da senha
 * @param charset Conjunto de caracteres
 * @param charset_len Tamanho do conjunto
 * @param password_len Comprimento da senha
 * @param output Buffer para armazenar a senha gerada
 */
void index_to_password(long long index, const char *charset, int charset_len,
                       int password_len, char *output) {
    for (int i = password_len - 1; i >= 0; i--) {
        output[i] = charset[index % charset_len];
        index /= charset_len;
    }
    output[password_len] = '\0';
}


/**
 * Função principal do coordenador
 */
int main(int argc, char *argv[]) {
    // TODO 1: Validar argumentos de entrada
    // Verificar se argc == 5 (programa + 4 argumentos)
    // Se não, imprimir mensagem de uso e sair com código 1
   
    // IMPLEMENTE AQUI: verificação de argc e mensagem de erro
    if (argc != 5){
        fprintf(stderr, "Uso: %s <hash_md5> <tamanho> <charset> <num_workers>\n", argv[0]); // saída de mensagens de erro 
        // -e diagnósticos de um programa de computador, separando-o da saída normal (stdout), é direcionado para o terminal
        // verificar ser realmente esta certo e o q significa
        exit(1);
    }
    // Parsing dos argumentos (após validação)
    const char *target_hash = argv[1];
    int password_len = atoi(argv[2]);
    const char *charset = argv[3];
    int num_workers = atoi(argv[4]);
    int charset_len = strlen(charset);
   
    // TODO: Adicionar validações dos parâmetros
    // - password_len deve estar entre 1 e 10
    if (password_len < 1 || password_len > 10) {
        fprintf(stderr, "Erro: tamanho da senha deve estar entre 1 e 10\n");
        exit(1);
    }
    // - num_workers deve estar entre 1 e MAX_WORKERS
    if (num_workers < 1 || num_workers > MAX_WORKERS) {
        fprintf(stderr, "Erro: número de workers deve estar entre 1 e %d\n", MAX_WORKERS);
        exit(1);
    }
    // - charset não pode ser vazio
    if (charset_len == 0) {
        fprintf(stderr, "Erro: charset não pode ser vazio\n");
        exit(1);
    }


    printf("=== Mini-Projeto 1: Quebra de Senhas Paralelo ===\n");
    printf("Hash MD5 alvo: %s\n", target_hash);
    printf("Tamanho da senha: %d\n", password_len);
    printf("Charset: %s (tamanho: %d)\n", charset, charset_len);
    printf("Número de workers: %d\n", num_workers);
   
    // Calcular espaço de busca total
    long long total_space = calculate_search_space(charset_len, password_len);
    printf("Espaço de busca total: %lld combinações\n\n", total_space);
   
    // Remover arquivo de resultado anterior se existir
    unlink(RESULT_FILE);    
    // Registrar tempo de início
    time_t start_time = time(NULL);
   
    // TODO 2: Dividir o espaço de busca entre os workers
    // Calcular quantas senhas cada worker deve verificar
    // DICA: Use divisão inteira e distribua o resto entre os primeiros workers
   
    // IMPLEMENTE AQUI:
    // long long passwords_per_worker = ?
    long long base = total_space / num_workers;


    // long long remaining = ?
    long long resto = total_space % num_workers;
   
    // Arrays para armazenar PIDs dos workers
    pid_t workers[MAX_WORKERS];
   
    // TODO 3: Criar os processos workers usando fork()
    printf("Iniciando workers...\n");
   
    // IMPLEMENTE AQUI: Loop para criar workers
    long long inicio = 0; 
    for (int i = 0; i < num_workers; i++) {
        // TODO: Calcular intervalo de senhas para este worker
        long long qtd = base + (i < resto ? 1 : 0);
        long long fim = inicio + qtd - 1;
        // TODO: Converter indices para senhas de inicio e fim
        char id_str[8], len_str[8];
        sprintf(id_str, "%d", i);
        sprintf(len_str, "%d", password_len);
        char start_pwd[64], end_pwd[64];
        index_to_password(inicio, charset, charset_len, password_len, start_pwd);
        index_to_password(fim, charset, charset_len, password_len, end_pwd);


        // char start_str[32], end_str[32];
        // sprintf(start_str, "%lld", inicio);
        // sprintf(end_str, "%lld", fim);
        // Preparar strings auxiliares

        // char start_str[32], end_str[32], id_str[8], len_str[8];
        // sprintf(start_str, "%lld", inicio);
        // sprintf(end_str, "%lld", fim);
        // sprintf(id_str, "%d", i);
        // sprintf(len_str, "%d", password_len);

        // char start_pwd[64], end_pwd[64];
        // index_to_password(inicio, charset, charset_len, password_len, start_pwd);
        // index_to_password(fim, charset, charset_len, password_len, end_pwd);


        // TODO 4: Usar fork() para criar processo filho
        pid_t pid = fork();
        if (pid < 0) {
            perror("Erro em fork"); // TODO 7
            exit(1);
        } else if (pid == 0) {
            // execl("./worker", "worker", // TODO 6
            //       target_hash,start_str, end_str,
            //     charset, len_str, id_str,
            //       (char *)NULL);
            execl("./worker", "worker",
              target_hash, start_pwd, end_pwd,
              charset, len_str, id_str,
              (char *)NULL);
            perror("Erro em execl"); // TODO 7
            exit(1);
        } else {
        // TODO 5: No processo pai: armazenar PID
            workers[i] = pid;
            // char start_pwd[64], end_pwd[64]; // discutir com as meninas qual aceb melhor e mostra os erros
            // index_to_password(inicio, charset, charset_len, password_len, start_pwd);
            // index_to_password(fim, charset, charset_len, password_len, end_pwd);
            printf("Worker %d (PID=%d) intervalo [%s ... %s]\n", i, pid, start_pwd, end_pwd);
        }
        inicio += qtd;
    }
        // TODO 6: No processo filho: usar execl() para executar worker - implementado dentro do if else
        // TODO 7: Tratar erros de fork() e execl() - implementado dentro do if else
   // }
   
    printf("\nTodos os workers foram iniciados. Aguardando conclusão...\n");
   
    // TODO 8: Aguardar todos os workers terminarem usando wait()
    // IMPORTANTE: O pai deve aguardar TODOS os filhos para evitar zumbis
   
    // IMPLEMENTE AQUI:
    // - Loop para aguardar cada worker terminar
    // - Usar wait() para capturar status de saída
    // - Identificar qual worker terminou
    // - Verificar se terminou normalmente ou com erro
    // - Contar quantos workers terminaram
    int finished = 0;
    while (finished < num_workers) {
        int status;
        pid_t pid = wait(&status);
        if (pid == -1) {
            perror("Erro em wait");
            break;
        }
        finished++;
        if (WIFEXITED(status)) {
            printf("Worker (PID=%d) terminou com código %d\n", pid, WEXITSTATUS(status));
        } else {
            printf("Worker (PID=%d) terminou de forma anormal\n", pid);
        }
    }
   
    // Registrar tempo de fim
    time_t end_time = time(NULL);
    double elapsed_time = difftime(end_time, start_time);
   
    printf("\n=== Resultado ===\n");
   
    // TODO 9: Verificar se algum worker encontrou a senha
    // Ler o arquivo password_found.txt se existir
   
    // IMPLEMENTE AQUI:
    // - Abrir arquivo RESULT_FILE para leitura
    // - Ler conteúdo do arquivo
    // - Fazer parse do formato "worker_id:password"
    // - Verificar o hash usando md5_string()
    // - Exibir resultado encontrado
    int fd = open(RESULT_FILE, O_RDONLY);
    if (fd >= 0) {
        char buffer[256];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        if (n > 0) {
            buffer[n] = '\0';
            char *sep = strchr(buffer, ':'); // encontra primeira ocorrência de um caractere específico dentro de uma string
            if (sep) {
                *sep = '\0';
                int worker_id = atoi(buffer);
                char *senha = sep + 1;
                senha[strcspn(senha, "\r\n")] = '\0';


                char hash[33];
                md5_string(senha, hash);
                if (strcmp(hash, target_hash) == 0) {
                    printf("Senha encontrada pelo worker %d: %s\n", worker_id, senha);
                } else {
                    printf("Senha inválida no arquivo de resultado.\n");
                }
            }
        }
    } else {
        printf("Nenhum worker encontrou a senha.\n");
    }
    // Estatísticas finais (opcional)
    // TODO: Calcular e exibir estatísticas de performance
    printf("Tempo total: %.2f segundos\n", elapsed_time);


    return 0;
}
