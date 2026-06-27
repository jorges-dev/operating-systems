# Trabalho N3 - Sistemas Operacionais

**Aluno:** Jorge Eduardo Salvador  
**Disciplina:** Sistemas Operacionais  
**Curso:** Engenharia de Software - 1º Período
**Professor:** Rafael Descio

---

## Questões do relatório

**1. Qual a diferença entre imagem e container?**

A imagem é tipo uma receita ou um modelo, ela tem tudo que o ambiente precisa mas não está rodando. O container é quando você usa essa receita e ela vira um programa rodando de verdade na máquina. É parecido com classe e objeto em programação.

**2. Qual processo está executando dentro do container?**

O processo `node server.js`. Como ele é o primeiro a iniciar no container, recebe o PID 1. Isso aparece quando a gente acessa a rota /info.

**3. O container possui kernel próprio? Justifique.**

Não tem. O container usa o mesmo kernel do sistema operacional que está rodando o Docker. Diferente de uma máquina virtual que tem seu próprio SO, o container só isola o processo usando dois recursos do linux chamados namespaces e cgroups.

**4. Qual recurso foi limitado na sua infraestrutura?**

Minha memória RAM. Coloquei `mem_limit: 128m` no docker-compose.yml, então cada container pode usar no máximo 128 megabytes. O linux é quem controla esse limite.

**5. Qual a finalidade do volume Docker utilizado?**

É para os dados do MySQL não se perderem quando o container for desligado ou removido. Sem o volume, toda vez que o container sumisse os dados sumiam junto. Com o volume `dados-mysql` os arquivos ficam salvos no computador mesmo.

**6. Qual a finalidade da rede Docker criada?**

Serve para o container da aplicação e o container do banco conseguirem se comunicar. Dentro dessa rede, o container web acessa o banco só usando o nome `db` como endereço, porque o Docker tem um DNS interno que resolve isso. Também isola os dois containers do resto da máquina.

**7. Por que executar aplicações como usuário não-root?**

É bem fundamental para ter segurança. Se alguém encontrar uma falha na aplicação e ela estiver rodando como root, essa pessoa teria controle total. Com um usuário sem poderes de administrador, o estrago que ela poderia fazer fica bem menor.

**8. Por que Docker não é uma máquina virtual?**

Porque a máquina virtual simula um computador completo, com sistema operacional e kernel próprios, o que deixa ela pesada e lenta para ligar. O Docker não faz isso, ele usa o SO que já existe no computador e só isola os processos. Por isso é muito mais leve e inicia bem rápido.

**9. O que representa o PID exibido na rota /info?**

É o número de identificação do processo. Aparece como 1 porque o Docker usa um recurso chamado PID Namespace que faz o container acreditar que o Node.js é o primeiro processo rodando, como se fosse um computador só dele. Na máquina real esse processo teria um número diferente.

**10. Cite três conceitos de Sistemas Operacionais presentes neste projeto.**

- **Processos:** cada container é um processo isolado rodando no linux. O Node.js vira o PID 1 dentro do container.
- **Gerenciamento de memória:** o limite de 128MB que configurei é controlado pelo Linux através dos cgroups, que impedem o container de usar mais memória.
- **Sistema de arquivos:** o volume do MySQL é um exemplo de persistência de dados. O Docker isola o sistema de arquivos de cada container, mas o volume cria uma ligação com o disco do host para os dados sobreviverem.
