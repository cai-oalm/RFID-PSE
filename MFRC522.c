#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "MFRC522.h"


void Write_MFRC522(u_char addr, u_char val) {
	
    u_char addr_bits = (((addr<<1) & 0x7E));

  HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi2, &addr_bits, 1, 500);
  HAL_SPI_Transmit(&hspi2, &val, 1, 500);
  HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_SET);
}
//-----------------------------------------------
/*
 * Nome da função: Read_MFRC522
 * Descrição: Lê um byte do registrador de um MFRC522
 * Entrada: addr - endereço do registrador
 * Retorna: a byte of data read from the
 */
u_char Read_MFRC522(u_char addr) {
  u_char rx_bits;
  u_char addr_bits = (((addr<<1) & 0x7E) | 0x80);

  HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi2, &addr_bits, 1, 500);

  HAL_SPI_Receive(&hspi2, &rx_bits, 1, 500);

  HAL_GPIO_WritePin(RC522_CS_GPIO_Port, RC522_CS_Pin, GPIO_PIN_SET);

	return (u_char) rx_bits; // retorna os bits rx, transmitindo um int de 8 bits e cortando os 24 superiores
}
//--------------------------------------------------------
/*
 * Nome da função: SetBitMask
 * Descrição: Define o bit do registrador
 * Entrada: reg - endereço do registrador; mask - define valor
 * Retorna: Nada
 */
void SetBitMask(u_char reg, u_char mask)
{
    u_char tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp | mask);  // define a bit mask
}
//
/*
 * Nome da função: ClearBitMask
 * Descrição: limpa o registrador
 * Entrada: reg - endereço do registrador; mask - limpa valor
 * Retorna: Nada
*/
void ClearBitMask(u_char reg, u_char mask)
{
    u_char tmp;
    tmp = Read_MFRC522(reg);
    Write_MFRC522(reg, tmp & (~mask));  // limpa bit mask
}

//-----------------------------------------------
/*
 * Nome da função: AntennaOn
 * Descrição: Ativa antenas
 * Entradas: Nada
 * Retorna: Nada
 */
void AntennaOn(void)
{
  SetBitMask(TxControlReg, 0x03);
}


/*
  * Nome da função: AntennaOff
  * Descrição: Desativa antenas
  * Entradas: Nada
  * Retorna: Nada
 */
void AntennaOff(void)
{
  ClearBitMask(TxControlReg, 0x03);
}


/*
 * Nome da função: ResetMFRC522
 * Descrição: Reseta o RC522
 * Entradas: Nada
 * Retorna: Nada
 */
void MFRC522_Reset(void)
{
  Write_MFRC522(CommandReg, PCD_RESETPHASE);
}
//--------------------------------------------------
/*
 * Nome da função: InitMFRC522
 * Descrição: Inicializa módulo RC522
 * Entradas: Nada
 * Retorna: Nada
*/
void MFRC522_Init(void)
{
  MFRC522_Reset();

  // Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
  Write_MFRC522(TModeReg, 0x80); // 0x8D);      // Tauto=1; f(Timer) = 6.78MHz/TPreScaler
  Write_MFRC522(TPrescalerReg, 0xA9); //0x34); // TModeReg[3..0] + TPrescalerReg
  Write_MFRC522(TReloadRegL, 0x03); //30);
  Write_MFRC522(TReloadRegH, 0xE8); //0);
  Write_MFRC522(TxAutoReg, 0x40);     // força moludação em amplitude por chaveamento (ASK)
  Write_MFRC522(ModeReg, 0x3D);       // Valor inicial do CRC 0x6363
  AntennaOn();
}
/*
 * Nome da função: MFRC522_ToCard
 * Descrição: Comunicação entre RC522 e ISO14443 
 * Entrada: command - dá comando
 *			 sendData--RC522 sent to the card by the data
 *			 sendLen--tamanho do dado
 *			 backData--recebido, o cartão retorna dado,
 *			 backLen--retorna comprimento do bit
 * Retorna: se bem sucedido, MI_OK
 */
u_char MFRC522_ToCard(u_char command, u_char *sendData, u_char sendLen, u_char *backData, uint *backLen)
{
  u_char status = MI_ERR;
  u_char irqEn = 0x00;
  u_char waitIRq = 0x00;
  u_char lastBits;
  u_char n;
  uint i;

  switch (command)
  {
    case PCD_AUTHENT:     // Desativa certificação do cartão
      {
        irqEn = 0x12;
        waitIRq = 0x10;
        break;
      }
    case PCD_TRANSCEIVE:  // Transmite dado FIFO (first-in-first-out)
      {
        irqEn = 0x77;
        waitIRq = 0x30;
        break;
      }
    default:
      break;
  }

  Write_MFRC522(CommIEnReg, irqEn|0x80);  // Interrompe requisição
  ClearBitMask(CommIrqReg, 0x80);         // Limpa os pedidos de interrupção 
  SetBitMask(FIFOLevelReg, 0x80);         // FlushBuffer=1, inicializa FIFO

  Write_MFRC522(CommandReg, PCD_IDLE);    // Sem ação; cancela comando atual

  // Escreve dado para o FIFO
  for (i=0; i<sendLen; i++)
  {
    Write_MFRC522(FIFODataReg, sendData[i]);
  }

  // Executa o comando
  Write_MFRC522(CommandReg, command);
  if (command == PCD_TRANSCEIVE)
  {
    SetBitMask(BitFramingReg, 0x80);      // StartSend=1, inicia a transmissão de dados
  }

  // Espera receber dados para completar
  i = 2000;	// i baseia-se no ajuste de frequência do clock
  do
  {
    n = Read_MFRC522(CommIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  ClearBitMask(BitFramingReg, 0x80);      

  if (i != 0)
  {
    if(!(Read_MFRC522(ErrorReg) & 0x1B))  
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
      {
        status = MI_NOTAGERR;             // ??
      }

      if (command == PCD_TRANSCEIVE)
      {
        n = Read_MFRC522(FIFOLevelReg);
        lastBits = Read_MFRC522(ControlReg) & 0x07;
        if (lastBits)
        {
          *backLen = (n-1)*8 + lastBits;
        }
        else
        {
          *backLen = n*8;
        }

        if (n == 0)
        {
          n = 1;
        }
        if (n > MAX_LEN)
        {
          n = MAX_LEN;
        }

        // Lê dados recebidos no FIFO
        for (i=0; i<n; i++)
        {
          backData[i] = Read_MFRC522(FIFODataReg);
        }
      }
    }
    else {
      status = MI_ERR;
    }
  }
  else {
  }

  return status;
}


//---------------------------------------------------------------

/*
 * Nome da função: MFRC522_Anticoll
 * Descrição: Detecção anti-colisão, lendo o serial do cartão
 * Entrada: serNum - Retorna 4 bytes do serial do cartão, sendo que os 5 primeiros bytes são para o checksum
 * Retorna: se bem sucedido, MI_OK
 */
u_char MFRC522_Anticoll(u_char *serNum)
{
  u_char status;
  u_char i;
  u_char serNumCheck=0;
  uint unLen;

  Write_MFRC522(BitFramingReg, 0x00);	

  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

  if (status == MI_OK)
  {
    //Confere serial
    for (i=0; i<4; i++)
    {
      serNumCheck ^= serNum[i];
    }
    if (serNumCheck != serNum[i])
    {
      status = MI_ERR;
    }
  }
  return status;
}
//---------------------------------------------------



/*
 * Nome da função: MFRC522_Read
 * Descrição Lê bloco de dados
 * Entrada: blockAddr - endereço do bloco; recvData - lê bloco de dados
 * Retorna: se bem sucedido, MI_OK
 */
u_char MFRC522_Read(u_char blockAddr, u_char *recvData)
{
  u_char status;
  uint unLen;

  recvData[0] = PICC_READ;
  recvData[1] = blockAddr;
  CalulateCRC(recvData,2, &recvData[2]);
  status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

  if ((status != MI_OK) || (unLen != 0x90))
  {
    status = MI_ERR;
  }

  return status;
}
/*
 * Nome da função: CalulateCRC
 * Descrição: Calcula redundância cíclica (CRC)
 * Entradas: pIndata - Lê os dados da CRC, len - tamanho dos dados, pOutData - resultado do cálculo
 * Retorna: Nada
 */
void CalulateCRC(u_char *pIndata, u_char len, u_char *pOutData)
{
  u_char i, n;

  ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
  SetBitMask(FIFOLevelReg, 0x80);			

  for (i=0; i<len; i++)
  {
    Write_MFRC522(FIFODataReg, *(pIndata+i));
  }
  Write_MFRC522(CommandReg, PCD_CALCCRC);

  //Espera o cálculo de redundância cíclica (CRC)
  i = 0xFF;
  do
  {
    n = Read_MFRC522(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));			//CRCIrq = 1

  //Lê o resultado do cálculo de CRC
  pOutData[0] = Read_MFRC522(CRCResultRegL);
  pOutData[1] = Read_MFRC522(CRCResultRegM);
}

void MIRC522_CheckApi(void){
	u_char buffer[9];
	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	u_char data[MAX_LEN+1];
	// Verifica status e pega ID do cartão
	status = MFRC522_Read(0X00, data);
	// Calcula CRC
	CalulateCRC(buffer, 7, &buffer[7]);
	Write_MFRC522(cardstr,0x00);
	MFRC522_Anticoll(cardstr);

}
