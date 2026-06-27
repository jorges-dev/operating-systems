const express = require('express')
const os = require('os')

const app = express()

// rota principal
app.get('/', (req, res) => {
  res.json({
      disciplina: 'Sistemas Operacionais',
      aluno: process.env.ALUNO,
      hostname: os.hostname(),
      plataforma: os.platform(),
      arquitetura: os.arch()
  })
})

// rota do desafio
app.get('/info', (req, res) => {
    res.json({
      pid: process.pid,
      uptime: Math.floor(process.uptime()),
      cpus: os.cpus().length
    })
})

app.listen(3000, () => {
  console.log('servidor rodando na porta 3000')
})
