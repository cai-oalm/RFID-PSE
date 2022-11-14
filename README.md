# 💳 Leitor  RFID MFRC522 Mifare
Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG – Prof. Ricardo de Oliveira Duarte – Departamento de Engenharia Eletrônica

# 📃 Descrição
O projeto consiste na implementação de uma função, a partir da utilização de um STM32F410RB e um módulo RFID MFRC522, que lê um cartão, calcula o CRC (para validar a integridade dos dados lidos), escreve no cartão e detecta anti-colisão (para evitar ações de sobrescrita entre os cartões).

# 🔨 Montagem
Abaixo, as fotos da montagem que fizemos para testar o projeto:

![Alt text](img/montagem_1.jpeg?raw=true "Visão frontal da montagem")
![Alt text](img/montagem_2.jpeg?raw=true "Visão superior da montagem")
![Alt text](img/montagem_3.jpeg?raw=true "Associacao dos pinos")
# 💻 Funções 
## 📖 MFRC522_Read
 Essa função é responsável por ler um bloco de dados do cartão
### Parâmetros:

 **blockAddr**: Endereço do bloco que será lido.
 
**recvData**: Variável em que os dados serão salvos

### Retorno:

**MI_OK** - sucesso; **MI_ERR** = erro 

## 💥 MFRC522_Anticoll
Essa função é responsável pela detecção anti-colisão, lendo o serial do cartão

### Parâmetros:  
 **serNum**: Retorna 4 bytes do serial do cartão, sendo que os 5 primeiros bytes são para o checksum

### Retorno:
**MI_OK** - sucesso; **MI_ERR** = erro 

## ✏️ Write_MFRC522
Essa função é responsável por escrever os dados em um certo bloco do cartão
### Parâmetros:

**addr**: Endereço do bloco que será sobrescrevido.  
**val**: Variável contendo o dado que será salvo

### Retorno:
Sem Retorno

## ➕ CalulateCRC
Essa funcão calcula redundância cíclica (CRC)
### Parâmetros:

**pIndata**: Lê os dados da CRCS  
**len**: tamanho dos dados
**pOutData**: resultado do cálculo

### Retorno:
Sem Retorno