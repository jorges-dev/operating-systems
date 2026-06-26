# Trabalho N3 — Sistemas Operacionais
**Infraestrutura Docker com Node.js e MySQL**

**Aluno:** Jorge Eduardo Salvador  
**Disciplina:** Sistemas Operacionais  
**Curso:** Engenharia de Software — 1º Período

---

## Estrutura do Projeto

```
projeto-docker/
├── docker-compose.yml
├── README.md
├── evidencias/
│   ├── docker-ps.txt
│   ├── docker-stats.txt
│   ├── docker-inspect-app.txt
│   ├── logs-app.txt
│   └── print-navegador.png
└── app/
    ├── Dockerfile
    ├── package.json
    └── server.js
```

---

## Relatório — Questões

### 1. Qual a diferença entre imagem e container?

A **imagem** é um modelo estático e imutável — a "receita" de como o ambiente deve ser. Ela contém o sistema de arquivos base, as dependências instaladas e as instruções de execução, mas não está rodando. É como uma classe em programação orientada a objetos.

O **container** é a instância viva da imagem em execução. Quando o Docker cria um container a partir de uma imagem, ele adiciona uma camada de escrita por cima das camadas somente-leitura da imagem, e inicia o processo definido no `CMD` do Dockerfile. É como um objeto instanciado de uma classe.

---

### 2. Qual processo está executando dentro do container?

O processo em execução dentro do container `so-app` é o `node server.js`. Por ser definido no `CMD ["node", "server.js"]` do Dockerfile, o Node.js torna-se o **PID 1** do container. Isso pode ser confirmado acessando a rota `/info`, que retorna `"pid": 1`. Se esse processo morrer, o container para automaticamente.

---

### 3. O container possui kernel próprio? Justifique.

**Não.** O container não possui kernel próprio. Essa é a principal diferença entre containers e máquinas virtuais.

Os containers utilizam o **kernel do sistema operacional host** diretamente. O isolamento é obtido por dois mecanismos nativos do kernel Linux:

- **Namespaces:** criam uma "bolha" de visão para cada container — o container enxerga seu próprio sistema de arquivos, sua própria rede e seus próprios processos, mas tudo isso é gerenciado pelo mesmo kernel do host.
- **cgroups (Control Groups):** limitam quanto de CPU e memória cada container pode consumir.

Como o kernel é compartilhado, containers são muito mais leves que máquinas virtuais (que precisam carregar um kernel completo para cada instância).

---

### 4. Qual recurso foi limitado na sua infraestrutura?

Foi limitada a **memória RAM** de ambos os containers, usando a diretiva `mem_limit: 128m` no `docker-compose.yml`. Isso restringe cada container a no máximo 128 MB de RAM.

Esse limite é implementado pelo kernel Linux por meio dos **cgroups**. Caso o processo dentro do container tente alocar mais memória do que o permitido, o kernel envia um sinal de encerramento (OOM — Out of Memory Killer) para o processo.

---

### 5. Qual a finalidade do volume Docker utilizado?

O volume `dados-mysql` foi criado para garantir a **persistência dos dados do banco de dados MySQL**.

Por padrão, tudo que um container escreve dentro de si mesmo é perdido quando o container é removido (a camada de escrita do container é descartada). O volume resolve isso mapeando o diretório `/var/lib/mysql` (onde o MySQL armazena seus dados) para um local gerenciado pelo Docker no sistema de arquivos do host.

Dessa forma, mesmo que o container `so-db` seja parado, removido e recriado, os dados do banco permanecem intactos no volume `dados-mysql`.

---

### 6. Qual a finalidade da rede Docker criada?

A rede `rede-so` (driver `bridge`) foi criada com duas finalidades:

**Comunicação entre containers:** dentro dessa rede, o container `web` consegue acessar o container `db` simplesmente usando o nome `db` como hostname — o Docker possui um DNS interno que resolve automaticamente o nome do serviço para o IP do container correto. Sem essa rede, os containers não conseguiriam se comunicar.

**Isolamento:** a rede privada isola o tráfego dos dois containers do restante da rede do host e de outros containers que possam existir na máquina. Apenas containers conectados à `rede-so` podem se comunicar entre si.

---

### 7. Por que executar aplicações como usuário não-root?

Por razões de **segurança**. Por padrão, processos dentro de um container Docker rodam como `root`. Se uma vulnerabilidade na aplicação for explorada por um atacante, ele teria privilégios de root dentro do container.

Embora o container seja isolado do host por namespaces, existem vetores de ataque (como misconfigurações de volume ou vulnerabilidades no próprio Docker) que poderiam permitir que um processo root dentro do container escalasse privilégios no sistema host.

No Dockerfile deste projeto, foi criado um usuário `appuser` sem privilégios administrativos, e a diretiva `USER appuser` garante que o Node.js seja iniciado com esse usuário — minimizando o impacto de uma eventual exploração.

---

### 8. Por que Docker não é uma máquina virtual?

Uma **máquina virtual** emula hardware completo via hypervisor, carrega um sistema operacional inteiro (com seu próprio kernel) e ocupa gigabytes de disco. O boot leva minutos.

O **Docker** não emula hardware nem carrega um kernel próprio. Ele usa recursos nativos do kernel Linux (namespaces e cgroups) para isolar processos. Um container é, fundamentalmente, um processo do host com visão restrita. Ocupa megabytes, inicia em segundos e compartilha o kernel com o host e com os outros containers.

| Característica | VM | Container Docker |
|---|---|---|
| Kernel | Próprio | Compartilhado com o host |
| Boot | Minutos | Segundos |
| Tamanho | Gigabytes | Megabytes |
| Isolamento | Hardware | Processo (namespaces) |

---

### 9. O que representa o PID exibido na rota /info?

O `"pid": 1` exibido na rota `/info` representa o **identificador do processo Node.js dentro do namespace de PID do container**.

O Docker usa o **PID Namespace** para criar uma visão isolada de processos para cada container. Dentro desse namespace, o processo iniciado pelo `CMD` do Dockerfile é sempre o processo número 1 — assim como o `init` é o PID 1 em um sistema Linux completo. Ele é o ancestral de todos os outros processos que possam existir dentro do container.

No host, esse mesmo processo teria um PID diferente (por exemplo, 4502), pois no namespace global do host ele é apenas mais um processo entre todos os outros.

---

### 10. Cite três conceitos de Sistemas Operacionais presentes neste projeto.

**Gerenciamento de Processos:** cada container é um processo isolado executado pelo kernel do host. O Node.js torna-se o PID 1 do container, e o kernel controla seu ciclo de vida — criação, execução e encerramento.

**Gerenciamento de Memória:** o limite de 128 MB configurado via `mem_limit` é implementado pelo kernel Linux através dos **cgroups**, que monitoram e restringem o uso de memória RAM por grupo de processos.

**Sistema de Arquivos:** o volume `dados-mysql` ilustra o conceito de sistema de arquivos persistente. O Docker usa o **Mount Namespace** para que cada container enxergue seu próprio sistema de arquivos isolado, enquanto o volume cria um ponto de montagem que conecta o sistema de arquivos do container ao do host, garantindo persistência além do ciclo de vida do container.

---

## Como executar

```bash
# 1. Subir toda a infraestrutura
docker compose up -d --build

# 2. Verificar os containers rodando
docker ps

# 3. Testar as rotas
curl http://localhost:3000
curl http://localhost:3000/info

# 4. Parar e remover tudo (os dados do banco permanecem no volume)
docker compose down
```
