# Guia Didático — Infraestrutura Docker N3

**Disciplina:** Sistemas Operacionais  
**Aluno:** Jorge Eduardo Salvador

---

## O que vamos construir

Uma infraestrutura com dois containers Docker conversando entre si:

```
Seu navegador
     │
     │ http://localhost:3000
     ▼
┌─────────────────┐     rede-so (bridge)     ┌─────────────────┐
│   container     │ ───────────────────────► │   container     │
│    so-app       │                          │    so-db        │
│  (Node.js)      │                          │   (MySQL)       │
│  porta 3000     │                          │  porta 3306     │
│  máx 128MB RAM  │                          │  máx 128MB RAM  │
│  user: appuser  │                          │  volume: dados  │
└─────────────────┘                          └─────────────────┘
```

---

## Passo 1 — Instalar o Docker no Ubuntu

```bash
# Atualiza os pacotes do sistema
sudo apt update

# Instala o Docker
sudo apt install -y docker.io docker-compose-v2

# Adiciona seu usuário ao grupo docker (evita precisar de sudo)
sudo usermod -aG docker $USER

# IMPORTANTE: faz logout e login para o grupo ter efeito
# Ou execute:
newgrp docker

# Verifica se funcionou
docker --version
docker compose version
```

---

## Passo 2 — Criar a estrutura de pastas

```bash
mkdir -p projeto-docker/app
mkdir -p projeto-docker/evidencias
cd projeto-docker
```

Sua pasta deve ficar assim:

```
projeto-docker/
├── app/
└── evidencias/
```

---

## Passo 3 — Criar a aplicação Node.js

### 3.1 — `app/package.json`

Esse arquivo diz ao Node.js quais bibliotecas o projeto precisa:

```json
{
  "name": "so-n3-app",
  "version": "1.0.0",
  "main": "server.js",
  "scripts": {
    "start": "node server.js"
  },
  "dependencies": {
    "express": "^4.18.2"
  }
}
```

### 3.2 — `app/server.js`

O servidor web que responde às rotas:

```javascript
const express = require('express');
const os      = require('os');

const app  = express();
const PORT = 3000;

// Rota principal /
app.get('/', (req, res) => {
  res.json({
    disciplina:  'Sistemas Operacionais',
    aluno:       process.env.ALUNO || 'Jorge Eduardo Salvador',
    hostname:    os.hostname(),    // ID do container
    plataforma:  os.platform(),    // 'linux'
    arquitetura: os.arch()         // 'x64'
  });
});

// Rota /info (desafio obrigatório)
app.get('/info', (req, res) => {
  res.json({
    pid:    process.pid,                  // sempre 1 dentro do container
    uptime: Math.floor(process.uptime()), // segundos rodando
    cpus:   os.cpus().length              // CPUs visíveis
  });
});

app.listen(PORT, '0.0.0.0', () => {
  console.log(`Servidor rodando na porta ${PORT}`);
});
```

**Por que `os.hostname()` retorna o ID do container?**  
O Docker usa o **UTS Namespace** para dar ao container um hostname próprio. Por padrão, esse hostname é o ID do container.

**Por que `process.pid` é sempre 1?**  
O Docker usa o **PID Namespace**. Dentro do container, o Node.js é o primeiro processo iniciado, então recebe PID 1 — como o `init` em um Linux real. No host, ele teria um PID diferente (ex: 3847).

---

## Passo 4 — Criar o Dockerfile

O `Dockerfile` é a "receita" para construir a imagem do container da aplicação:

```dockerfile
# app/Dockerfile
FROM node:18-alpine

# Cria usuário não-root (segurança)
RUN addgroup -S appgroup && adduser -S appuser -G appgroup

WORKDIR /app

# Copia package.json primeiro (otimização de cache)
COPY package.json .
RUN npm install --omit=dev

# Copia o restante do código
COPY . .

# Dá permissão ao usuário não-root
RUN chown -R appuser:appgroup /app

# Troca para usuário não-root
USER appuser

EXPOSE 3000

CMD ["node", "server.js"]
```

**Por que `node:18-alpine` e não `node:18`?**  
A imagem Alpine Linux tem apenas ~5MB. A imagem Ubuntu completa teria ~900MB. Em containers, menos é mais.

**Por que copiar o `package.json` antes do código?**  
O Docker constrói imagens em camadas. Se o código mudar mas o `package.json` não, o Docker reutiliza a camada do `npm install` do cache — tornando o rebuild muito mais rápido.

**Por que criar um usuário não-root?**  
Se um atacante explorar uma vulnerabilidade no Express, ele estaria rodando como `appuser` (sem poderes) em vez de `root` (poderia fazer qualquer coisa).

---

## Passo 5 — Criar o docker-compose.yml

O Compose orquestra múltiplos containers de uma vez:

```yaml
# docker-compose.yml (na raiz do projeto)
services:

  web:
    build: ./app
    container_name: so-app
    ports:
      - "3000:3000"
    environment:
      - ALUNO=Jorge Eduardo Salvador
      - DB_HOST=db
    networks:
      - rede-so
    depends_on:
      - db
    mem_limit: 128m

  db:
    image: mysql:8.0
    container_name: so-db
    environment:
      - MYSQL_ROOT_PASSWORD=root123
      - MYSQL_DATABASE=escola
      - MYSQL_USER=appuser
      - MYSQL_PASSWORD=app123
    volumes:
      - dados-mysql:/var/lib/mysql
    networks:
      - rede-so
    mem_limit: 128m

networks:
  rede-so:
    driver: bridge

volumes:
  dados-mysql:
    driver: local
```

**Explicação de cada bloco:**

`ports: "3000:3000"` — mapeia a porta 3000 do seu Ubuntu para a porta 3000 do container. Sem isso, o container estaria isolado e inacessível pelo navegador.

`environment` — variáveis de ambiente injetadas no container. O Node.js lê `process.env.ALUNO` para exibir seu nome.

`networks: rede-so` — ambos os containers estão na mesma rede privada. O container `web` acessa o banco de dados usando simplesmente `db` como endereço — o Docker resolve o nome via DNS interno.

`mem_limit: 128m` — limita a memória RAM usando **cgroups** do kernel Linux. Se o container tentar usar mais de 128 MB, o kernel mata o processo.

`volumes: dados-mysql:/var/lib/mysql` — o MySQL salva seus dados em `/var/lib/mysql`. O volume faz esse diretório "sair" do container e ser guardado no host. Os dados sobrevivem mesmo se o container for deletado.

`depends_on: db` — o Compose espera o container `db` iniciar antes de subir o `web`.

---

## Passo 6 — Subir a infraestrutura

```bash
# Dentro da pasta projeto-docker/
docker compose up -d --build
```

- `up` → sobe todos os serviços
- `-d` → modo detached (roda em segundo plano, libera o terminal)
- `--build` → reconstrói a imagem da aplicação antes de subir

**Acompanhe os logs em tempo real:**

```bash
docker compose logs -f
```

---

## Passo 7 — Testar as rotas

```bash
# Rota principal
curl http://localhost:3000

# Rota /info
curl http://localhost:3000/info
```

Ou abra no navegador: `http://localhost:3000`

Resultado esperado em `/`:
```json
{
  "disciplina": "Sistemas Operacionais",
  "aluno": "Jorge Eduardo Salvador",
  "hostname": "a3f2b1c4d5e6",
  "plataforma": "linux",
  "arquitetura": "x64"
}
```

Resultado esperado em `/info`:
```json
{
  "pid": 1,
  "uptime": 42,
  "cpus": 4
}
```

---

## Passo 8 — Coletar as evidências

Execute cada comando e salve a saída no arquivo correspondente:

```bash
# 1. docker ps — containers em execução
docker ps > evidencias/docker-ps.txt

# 2. docker stats — consumo de recursos (snapshot)
docker stats --no-stream > evidencias/docker-stats.txt

# 3. docker inspect — configuração completa do container da app
docker inspect so-app > evidencias/docker-inspect-app.txt

# 4. docker logs — logs da aplicação
docker logs so-app > evidencias/logs-app.txt

# 5. print-navegador.png — tire um print do navegador acessando
#    http://localhost:3000 e salve em evidencias/print-navegador.png
```

---

## Passo 9 — Compactar e entregar

```bash
# Volta um nível acima da pasta do projeto
cd ..

# Cria o .zip com tudo
zip -r projeto-docker-N3.zip projeto-docker/

# Verifica o conteúdo
unzip -l projeto-docker-N3.zip
```

---

## Passo 10 — Parar a infraestrutura

```bash
# Para e remove os containers (os dados do MySQL ficam no volume)
docker compose down

# Para remover também o volume (apaga os dados do banco):
docker compose down -v
```

---

## Conceitos de SO presentes em cada arquivo

| Arquivo | Conceito de SO |
|---|---|
| `Dockerfile` — `USER appuser` | Segurança — controle de privilégios |
| `Dockerfile` — `CMD ["node", "server.js"]` | Processos — PID 1 do container |
| `docker-compose.yml` — `mem_limit` | Gerenciamento de recursos — cgroups |
| `docker-compose.yml` — `networks` | Redes — namespaces de rede |
| `docker-compose.yml` — `volumes` | Sistema de arquivos — persistência |
| `server.js` — `os.hostname()` | UTS Namespace — hostname isolado |
| `server.js` — `process.pid` | PID Namespace — visão isolada de processos |

---

## Perguntas do sorteio — respostas rápidas

**O que acontece se removermos o volume?**  
Os dados do MySQL são perdidos permanentemente. O banco começa do zero na próxima vez que o container subir.

**Qual a função da rede Docker criada?**  
Permite comunicação entre os containers `so-app` e `so-db` por nome, e isola esse tráfego do restante da máquina.

**Por que o hostname do container é diferente do host?**  
O Docker usa o **UTS Namespace** para dar ao container um hostname próprio, independente do hostname do host.

**Como o Docker limita memória?**  
Usando **cgroups** (Control Groups) do kernel Linux. O parâmetro `mem_limit: 128m` instrui o Docker a configurar um cgroup que impede o processo de alocar mais de 128 MB.

**Qual a diferença entre container e máquina virtual?**  
A VM tem kernel próprio, ocupa gigabytes e demora minutos para iniciar. O container compartilha o kernel do host, ocupa megabytes e inicia em segundos. O isolamento da VM é em nível de hardware (hypervisor); o do container é em nível de processo (namespaces).

**Qual o papel do SO nessa infraestrutura?**  
O kernel Linux do host gerencia tudo: cria os namespaces que isolam os containers, aplica os cgroups que limitam a memória, controla o acesso à rede e ao disco, e agenda os processos dos containers na CPU — tudo sem que os containers saibam que estão compartilhando esses recursos.
