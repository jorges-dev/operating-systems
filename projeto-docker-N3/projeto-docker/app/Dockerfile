# -------------------------------------------------------
# Dockerfile - Imagem da aplicação Node.js
# -------------------------------------------------------

# 1. Imagem base: Node.js versão LTS leve (Alpine Linux)
#    Alpine usa ~5MB vs ~900MB do Ubuntu — ideal para containers
FROM node:18-alpine

# 2. Cria um usuário não-root chamado "appuser" (requisito de segurança)
#    Por padrão containers rodam como root, o que é um risco de segurança.
#    Se um atacante explorar a aplicação, ele teria acesso root ao container.
RUN addgroup -S appgroup && adduser -S appuser -G appgroup

# 3. Define o diretório de trabalho dentro do container
WORKDIR /app

# 4. Copia apenas o package.json primeiro (otimização de cache do Docker)
#    Se o código mudar mas as dependências não, o Docker reutiliza
#    a camada do npm install — economizando tempo no rebuild
COPY package.json .

# 5. Instala as dependências Node.js
RUN npm install --omit=dev

# 6. Copia o restante do código da aplicação
COPY . .

# 7. Muda a propriedade dos arquivos para o usuário não-root
RUN chown -R appuser:appgroup /app

# 8. Troca para o usuário não-root antes de iniciar a aplicação
USER appuser

# 9. Documenta que a aplicação escuta na porta 3000
EXPOSE 3000

# 10. Comando que inicia a aplicação (se esse processo morrer, o container para)
#     Formato exec [] garante que node seja o PID 1 do container
CMD ["node", "server.js"]
