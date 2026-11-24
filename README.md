# Memoria-Virtual

### Aluno: José Milton Batista Neto
### Turma: 3A

## Sistema Operacional

Implementado e testado no **WSL2 + Windows**

## Arquivos

| Arquivo | Descrição |
|---|---|
| main.c | Coordena o programa: valida os argumentos e o arquivo de endereços, abre o BACKING_STORE e traduz cada endereço, imprimindo o resultado na saída padrão |
| entrada.h / entrada.c | Validação dos argumentos, fifo ou lru, e leitura do arquivo de endereços |
| memoria.h / memoria.c | Tabela de páginas, memória física (128 frames), leitura do BACKING_STORE.bin e substituição de páginas fifo e lru |
| tlb.h / tlb.c | TLB de 16 entradas com substituição fifo e lru. A busca usa uma thread para cada entrada, sincronizadas com mutex e variáveis de condição |
| Makefile | Compila o projeto, roda os testes, cria o evidencias.log e limpa os arquivos gerados |
| evidencias.log | Registro das sessões de terminal |

## Como compilar

```bash
make (para funcionar com 128 frames)
make clean
make FRAMES=256 (para funcionar com 256 frames)
./vm addresses.txt fifo lru