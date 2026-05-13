1. Como o caminho é resolvido no Ubuntu?

O Linux usa um sistema baseado em inodes e diretórios.

Exemplo:
/empresa/engenharia/backend

O processo é:

Começa na raiz /
Procura por empresa (entrada de diretório → inode)
Depois entra em engenharia
Depois em backend

Cada diretório é basicamente uma tabela que mapeia:

nome → número do inode
2. Qual método de alocação seria melhor?

No Linux (ext4, por exemplo), não é usado só um método puro:

Usa algo parecido com alocação indexada (inode)
Mas otimizado com extents (blocos contínuos)

👉 Na prática:

Melhor que indexada pura (menos overhead)
Melhor que contígua pura (mais flexível)

✔ Então:
Extents (usado pelo ext4) é o mais eficiente hoje

3. Onde pode ocorrer fragmentação?

Mesmo no Linux:

Fragmentação externa: quando os blocos de um arquivo ficam espalhados
Fragmentação interna: último bloco parcialmente usado

Mas no ext4 isso é reduzido porque:

Ele tenta alocar blocos próximos (extents)
Faz pré-alocação

👉 Por isso Linux fragmenta bem menos que Windows tradicionalmente

4. Como o SO encontra um arquivo?

No Ubuntu:

Percorre o caminho (como explicado)
Encontra o inode do arquivo
O inode contém:
Permissões
Tamanho
Ponteiros para blocos/extents
A partir disso, lê os dados no disco

Se quiser, posso te mostrar comandos reais no Ubuntu tipo:

ls -i
stat arquivo
tree /empresa

pra você ver isso funcionando na prática.
