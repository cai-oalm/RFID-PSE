// 07/11/2022 - Caio Almeida e Gabriel Henrique - v1.0
// Licenca de codigo aberto - Baseado no projeto de Reza Tanhaei
// Hardware necessario: STM32F410RB e 
// “Esta API foi desenvolvida como trabalho da disciplina de Programação de Sistemas Embarcados da UFMG – Prof. Ricardo de Oliveira Duarte – Departamento de Engenharia Eletrônica

#ifndef MFRC522_H_
#define MFRC522_H_

#include "main.h"
#include "stm32f4xx_hal.h"
#include <sys/types.h>
typedef unsigned char   u_char;

#define MAX_LEN 16 //tamanho maximo array

//Comandos no MF522
#define PCD_IDLE              0x00               //ocioso/cancela acao
#define PCD_AUTHENT           0x0E               //chave de autenticacao
#define PCD_RECEIVE           0x08               //recebe dados
#define PCD_TRANSMIT          0x04               //transmite dados
#define PCD_TRANSCEIVE        0x0C               //transmite *E* recebe dados
#define PCD_RESETPHASE        0x0F               //reseta
#define PCD_CALCCRC           0x03               //Calcula verificacao cicilica de redundancia (CRC)

// Comandos do cartao
# define PICC_REQIDL          0x26               // acha a area da antena que não hiberna
# define PICC_REQALL          0x52               // acha todos os cartoes na area da antena
# define PICC_ANTICOLL        0x93               // anti colisao
# define PICC_SElECTTAG       0x93               // election card
# define PICC_AUTHENT1A       0x60               // chave de autenticao A
# define PICC_AUTHENT1B       0x61               // chave de autenticao B
# define PICC_READ            0x30               // Bloco de leitura
# define PICC_WRITE           0xA0               // Bloco de escrita
# define PICC_DECREMENT       0xC0               // Debita
# define PICC_INCREMENT       0xC1               // Recarrega
# define PICC_RESTORE         0xC2               // Transfere bloco de dados para buffer
# define PICC_TRANSFER        0xB0               // Salva dado no buffer
# define PICC_HALT            0x50               // Hiberna


//Codigo de erro é retornado quando há comunicacao
#define MI_OK                 0
#define MI_NOTAGERR           1
#define MI_ERR                2


//------------------Registrador do MFRC522 (padrão)---------------
//Pagina 0: Comandos e status
#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivlEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F

//Pagina 1: Comandos
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     Reserved11            0x1A
#define     Reserved12            0x1B
#define     MifareReg             0x1C
#define     Reserved13            0x1D
#define     Reserved14            0x1E
#define     SerialSpeedReg        0x1F

//Pagina 2: Configuracao
#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     Reserved21            0x23
#define     ModWidthReg           0x24
#define     Reserved22            0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsPReg              0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F

//Pagina 3: Teste/Registro
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34            0x3F

// definicao de funcoes
void Write_MFRC522(u_char, u_char);
u_char Read_MFRC522(u_char);
void SetBitMask(u_char, u_char);
void ClearBitMask(u_char, u_char);
void AntennaOn();
void AntennaOff();
void MFRC522_Reset();
void MFRC522_Init();
u_char MFRC522_Request(u_char, u_char*);
u_char MFRC522_ToCard(u_char, u_char*, u_char, u_char*, uint*);
u_char MFRC522_Anticoll(u_char*);
void CalulateCRC(u_char*, u_char, u_char*);
u_char MFRC522_Read(u_char, u_char*);

#endif /* MFRC522_H_ */
