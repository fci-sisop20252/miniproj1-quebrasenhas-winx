# Relatório: Mini-Projeto 1 - Quebra-Senhas Paralelo

**Aluno(s):** Nome (Matrícula), Nome (Matrícula),,,  
---

## 1. Estratégia de Paralelização


**Como você dividiu o espaço de busca entre os workers?**

[Explique seu algoritmo de divisão]

**Código relevante:** Cole aqui a parte do coordinator.c onde você calcula a divisão:
```c
// Cole seu código de divisão aqui
```

---

## 2. Implementação das System Calls

**Descreva como você usou fork(), execl() e wait() no coordinator:**

[Explique em um parágrafo como você criou os processos, passou argumentos e esperou pela conclusão]

**Código do fork/exec:**
```c
// Cole aqui seu loop de criação de workers
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