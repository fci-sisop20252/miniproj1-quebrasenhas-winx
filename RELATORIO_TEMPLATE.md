# Relatório: Mini-Projeto 1 - Quebra-Senhas Paralelo

**Aluno(s):** Anna Carolina (10443894), João Francisco (10443666), Maria Teresa(10386449), Carolyne Gomes(10436604).  
---

## 1. Estratégia de Paralelização


**Como você dividiu o espaço de busca entre os workers?**

O espaço de busca total foi dividido de formas quase iguais entre o número de workers especificado. A gente primeiro calcula uma quantidade base de senhas que cada worker deve verificar, dividindo o total de combinações pelo número de workers (total_space / num_workers). Como essa divisão pode não ser exata, o resto da divisão (total_space % num_workers) é distribuído entre os primeiros workers. Cada um dos primeiros resto workers recebe uma senha a mais para verificar, garantindo que todo trabalho seja distribuído da maneira mais uniforme possível.

**Código relevante:** Cole aqui a parte do coordinator.c onde você calcula a divisão:
```c


// TODO 2: Dividir o espaço de busca entre os workers
// Calcular quantas senhas cada worker deve verificar
// DICA: Use divisão inteira e distribua o resto entre os primeiros workers

// IMPLEMENTE AQUI:
// long long passwords_per_worker = ?
long long base = total_space / num_workers;


// long long remaining = ?
long long resto = total_space % num_workers;

// ... (dentro do loop de criação de workers)

long long inicio = 0; 
for (int i = 0; i < num_workers; i++) {
    // TODO: Calcular intervalo de senhas para este worker
    long long qtd = base + (i < resto ? 1 : 0);
    long long fim = inicio + qtd - 1;
    
    // ...
    
    inicio += qtd;
}
```

---

## 2. Implementação das System Calls

**Descreva como você usou fork(), execl() e wait() no coordinator:**

O processo coordenador utiliza um laço for para criar o número de workers desejado. Dentro de cada iteração, a chamada de sistema fork() é usada para criar um novo processo filho.

* No processo filho (pid == 0): A chamada execl() é executada imediatamente. Ela troca a imagem do processo filho pelo programa ./worker, passando como argumentos todas as informações necessárias para sua execução, como o hash alvo, a senha inicial e final de seu intervalo de busca, o charset, o tamanho da senha e seu próprio ID. Se execl() falhar, o programa encerra com erro.
* No processo pai (pid > 0): O pai armazena o PID (Process ID) do filho que acabou de criar em um array.

Após o laço de criação: O pai entra em um laço while que chama wait() repetidamente. A chamada wait() bloqueia o pai até que um de seus processos filhos termine. Isso garante que o coordenador aguarde a conclusão de todos os workers, evitando a criação de processos "zumbis" e permitindo que ele saiba quando a busca terminou.

**Código do fork/exec:**
```c
long long inicio = 0; 
for (int i = 0; i < num_workers; i++) {
    // ... (cálculo do intervalo) ...
    
    // TODO 4: Usar fork() para criar processo filho
    pid_t pid = fork();
    if (pid < 0) {
        perror("Erro em fork"); // TODO 7
        exit(1);
    } else if (pid == 0) {
        // Processo filho
        execl("./worker", "worker",
          target_hash, start_pwd, end_pwd,
          charset, len_str, id_str,
          (char *)NULL);
        perror("Erro em execl"); // TODO 7
        exit(1);
    } else {
    // TODO 5: No processo pai: armazenar PID
        workers[i] = pid;
        printf("Worker %d (PID=%d) intervalo [%s ... %s]\n", i, pid, start_pwd, end_pwd);
    }
    inicio += qtd;
}
```

---

## 3. Comunicação Entre Processos

**Como você garantiu que apenas um worker escrevesse o resultado?**

A cláusula do open() que impede com que a condição de corrida ocorra é a combinação do O_CREAT com o O_EXCL, basicamente o O_CREAT permite com que se o arquivo não existir o open consiga também realizar a sua criação antes de abrir, porém o O_EXCL colocado na função garante que o arquivo não pode existir, isto é que a criação precisa ser exclusiva. Neste sentido, caso o worker tente escrever o resultado que já foi escrito por outra thread, o arquivo já teria sido criado e a função open() retornaria com falha por conta das flags passadas como parâmetro da função.


**Como o coordinator consegue ler o resultado?**

[Explique como o coordinator lê o arquivo de resultado e faz o parse da informação]

---

## 4. Análise de Performance
Complete a tabela com tempos reais de execução:
O speedup é o tempo do teste com 1 worker dividido pelo tempo com 4 workers.

| Teste | 1 Worker | 2 Workers | 4 Workers | Speedup (4w) |
|-------|----------|-----------|-----------|--------------|
| Hash: 202cb962ac59075b964b07152d234b70<br>Charset: "0123456789"<br>Tamanho: 3<br>Senha: "123" | ___s | ___s | ___s | ___ |
| Hash: 5d41402abc4b2a76b9719d911017c592<br>Charset: "abcdefghijklmnopqrstuvwxyz"<br>Tamanho: 5<br>Senha: "hello" | ___s | ___s | ___s | ___ |

**O speedup foi linear? Por quê?**
[Analise se dobrar workers realmente dobrou a velocidade e explique o overhead de criar processos]

---

## 5. Desafios e Aprendizados
**Qual foi o maior desafio técnico que você enfrentou?**
[Descreva um problema e como resolveu. Ex: "Tive dificuldade com o incremento de senha, mas resolvi tratando-o como um contador em base variável"]

---

## Comandos de Teste Utilizados

```bash
# Teste básico
./coordinator "900150983cd24fb0d6963f7d28e17f72" 3 "abc" 2

# Teste de performance
time ./coordinator "202cb962ac59075b964b07152d234b70" 3 "0123456789" 1
time ./coordinator "202cb962ac59075b964b07152d234b70" 3 "0123456789" 4

# Teste com senha maior
time ./coordinator "5d41402abc4b2a76b9719d911017c592" 5 "abcdefghijklmnopqrstuvwxyz" 4
```
---

**Checklist de Entrega:**
- [ ] Código compila sem erros
- [ ] Todos os TODOs foram implementados
- [ ] Testes passam no `./tests/simple_test.sh`
- [ ] Relatório preenchido
