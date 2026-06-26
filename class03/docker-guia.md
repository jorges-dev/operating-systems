# Docker — Guia Prático

## O que é Docker?

Docker é uma plataforma que permite empacotar uma aplicação junto com tudo que ela precisa para funcionar — código, dependências, configurações — dentro de um **container**. Um container é um ambiente isolado e portátil que roda de forma consistente em qualquer máquina.

---

## Por que usar containers?

O problema clássico do desenvolvimento é o famoso *"funciona na minha máquina"*. Com Docker isso deixa de existir: se roda no teu container, roda em qualquer lugar.

Containers também são mais leves que máquinas virtuais — eles compartilham o kernel do sistema operacional host em vez de virtualizar um SO inteiro, o que os torna muito mais rápidos pra iniciar e mais econômicos em memória.

---

## Formas de uso

- **Desenvolvimento local** — isola o ambiente do projeto sem instalar dependências direto no sistema
- **Servidores web** — serve sites e APIs com Nginx, Apache ou Node dentro de containers
- **Bancos de dados** — sobe um PostgreSQL ou MySQL em segundos sem instalação
- **CI/CD** — pipelines de build e testes rodam em containers limpos e reproduzíveis
- **Microserviços** — cada serviço da aplicação roda em seu próprio container isolado
- **Distribuição de projetos** — empacota o projeto como imagem e compartilha com a equipe ou deploy em servidor

---

## Benefícios

- **Portabilidade** — a imagem roda igual em Linux, Mac, Windows e qualquer servidor
- **Isolamento** — cada container tem seu próprio ambiente, sem conflito entre projetos
- **Reprodutibilidade** — o `Dockerfile` documenta exatamente como o ambiente é construído
- **Leveza** — containers iniciam em segundos e consomem poucos recursos
- **Versionamento** — imagens têm versões, facilitando rollback em caso de problema
- **Escalabilidade** — fácil de replicar containers quando a aplicação precisa crescer

---

## Onde aplicar

| Área | Exemplo |
|---|---|
| Desenvolvimento web | Servidor Nginx servindo site estático |
| Backend | API Node.js ou Python Flask em container |
| Banco de dados | PostgreSQL isolado por projeto |
| Projetos acadêmicos | Compartilhar projeto pronto com a equipe |
| DevOps | Deploy em VPS com `docker compose up` |
| Testes | Ambiente limpo e descartável para cada teste |

---

## Principais comandos

### Imagens

```bash
# Baixar uma imagem do Docker Hub
docker pull nginx:alpine

# Listar imagens locais
docker images

# Construir imagem a partir de um Dockerfile
docker build -t nome-da-imagem .

# Remover imagem
docker rmi nome-da-imagem

# Enviar imagem para o Docker Hub
docker push usuario/nome-da-imagem
```

### Containers

```bash
# Rodar um container
docker run -d -p 8080:80 --name meu-site nome-da-imagem

# Listar containers rodando
docker ps

# Listar todos os containers (incluindo parados)
docker ps -a

# Parar um container
docker stop meu-site

# Remover um container
docker rm meu-site

# Ver logs
docker logs -f meu-site

# Entrar dentro do container
docker exec -it meu-site sh
```

### Docker Compose

```bash
# Subir todos os serviços em background
docker compose up -d

# Parar e remover os containers
docker compose down

# Ver status dos serviços
docker compose ps

# Ver logs em tempo real
docker compose logs -f

# Rebuildar as imagens e subir
docker compose up -d --build
```

---

## Estrutura básica de um projeto

```
meu-projeto/
├── Dockerfile          # como construir a imagem
├── docker-compose.yml  # como rodar os containers
└── src/                # código da aplicação
```

### Dockerfile mínimo

```dockerfile
FROM nginx:alpine
COPY ./src /usr/share/nginx/html
EXPOSE 80
```

### docker-compose.yml mínimo

```yaml
services:
  web:
    build: .
    ports:
      - "8080:80"
    restart: unless-stopped
```

---

> Docker é hoje uma das ferramentas mais importantes no desenvolvimento moderno. Aprender o ciclo `Dockerfile → build → run` abre caminho para deploy em nuvem, arquiteturas de microserviços e práticas de DevOps.
