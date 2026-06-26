// server.js - Aplicação Node.js com Express
// Disciplina: Sistemas Operacionais - N3

const express = require('express');
const os      = require('os');  // módulo nativo do Node para info do sistema

const app  = express();
const PORT = 3000;

// -------------------------------------------------------
// ROTA PRINCIPAL: /
// Retorna informações sobre o aluno e o ambiente
// -------------------------------------------------------
app.get('/', (req, res) => {
  res.json({
    disciplina:  'Sistemas Operacionais',
    aluno:       process.env.ALUNO || 'Jorge Eduardo Salvador',
    hostname:    os.hostname(),       // ID do container (fornecido pelo Docker via namespace)
    plataforma:  os.platform(),       // 'linux'
    arquitetura: os.arch()            // 'x64' ou 'arm64'
  });
});

// -------------------------------------------------------
// ROTA DESAFIO: /info
// Retorna informações sobre o processo Node em execução
// -------------------------------------------------------
app.get('/info', (req, res) => {
  res.json({
    pid:    process.pid,          // PID do processo dentro do container (sempre 1)
    uptime: Math.floor(process.uptime()), // segundos desde que o processo iniciou
    cpus:   os.cpus().length      // número de CPUs visíveis para o container
  });
});

// Inicia o servidor na porta definida, escutando em todas as interfaces
app.listen(PORT, '0.0.0.0', () => {
  console.log(`Servidor rodando na porta ${PORT}`);
  console.log(`Aluno: ${process.env.ALUNO || 'Jorge Eduardo Salvador'}`);
  console.log(`Hostname (container): ${os.hostname()}`);
});
