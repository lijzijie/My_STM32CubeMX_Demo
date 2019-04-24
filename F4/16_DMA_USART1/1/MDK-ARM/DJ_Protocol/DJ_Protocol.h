#ifndef _DJ_PROTOCOL_H_
#define _DJ_PROTOCOL_H_

#include "stm32f4xx.h"
//#include "USART.h"
//#include "USART2.h"	

/***********************************************
ʹ�÷�����
1����ʼ�����ڣ� ʵ�ִ��ڷ��ͺ���
2�����ĺ궨�� USART_SendBuff �������Ϊʵ�ʴ��ڷ��ͺ�����  xxxxxx   ���ú�������Ϊ uchar *buff,uchar len    lenΪ���ͳ��ȣ�
3������ tCmdID  cmdid = SelfDefinedData; //�Զ������� 
				tSelfDefineInfo SelfDefineInfo = {.data1 = 1.133f,.data2 = 1.133f,.data3 = 1.143f};//�Զ��崫������
   ���ú��� Send_FrameData(cmdid,(uint8_t *)&SelfDefineInfo,sizeof(SelfDefineInfo));  //�����Զ�������������ϵͳ
4�������Ҫ���ղ���ϵͳ���������ݣ���Ҫʵ�ִ����жϽ��պ��������ձ�־Ϊ USART2_RX_STA�������ַ USART2_RX_BUF
	Ȼ������������� DataVerify() ������Ȼ���������һ�����ڷ��͵��������ݹ۲죨����ʵ��printf��������
	--------------------------------------------------------------------------2017/6/18
***********************************************/


// uart ͨ�����ã������� 115200, ����λ 8, ֹͣλ 1������λ�ޣ��������ޡ�
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1

#define USART_SendBuff 			HAL_UART_Transmit_DMA   //USART_Send_BUFF(uchar *buff,uchar len) 

/********************  ��������  ***************************/
//0-3 bits
#define Strike_0   0x00 //0��װ���棨ǰ��
#define Strike_1   0x01 //1��װ���棨��
#define Strike_2   0x02 //2��װ���棨��
#define Strike_3   0x03 //3��װ���棨�ң�
#define Strike_4   0x04 //4��װ���棨��1��
#define Strike_5   0x05 //5��װ���棨��2��
//4-7 bits
#define BC_Strike          0x00 //װ���˺����ܵ������� BloodChanged    BC
#define BC_ShootSpeed      0x01 //�ӵ����ٿ�Ѫ
#define BC_ShootFreq       0x02 //�ӵ���Ƶ��Ѫ
#define BC_PowerLimit      0x03 //���ʳ���
#define BC_ModuleOffline   0x04 //ģ�����߿�Ѫ
#define BC_CommonFoul      0x06 //��ͨ�����Ѫ
#define BC_Tarmac          0x0a //ͣ��ƺ��Ѫ
#define BC_EngineerOuto    0x0b //���̻������Զ���Ѫ


typedef __packed struct
{
	uint8_t SOF;          //������ʼ�ֽڣ��̶�Ϊ0xA5          
	uint16_t DataLength;  //���ݳ���
	uint8_t Seq;          //�����
	uint8_t CRC8;         //֡ͷCRCУ��
}tFrameHeader;//֡ͷ

typedef enum 
{
	GameInfo = 0x0001,     //����������Ϣ    ����Ƶ�� 50 Hz
	RealBloodChangedData,  //ʵʱѪ���仯����
	RealShootData,             //ʵʱ�������
	SelfDefinedData =0x0005, //ѧ���Զ�������
	Wrong = 0x1301 //ö����Ч��ֻ��Ϊ��ʹ��ö�ٴ�СΪ2�ֽ�
}tCmdID; 
typedef __packed struct
{
	uint8_t flag;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t compass; //��λ
}tLocData; //λ����Ϣ

typedef __packed struct
{
	uint32_t remainTime;       //����ʣ��ʱ�䣨�ӵ���ʱ�����ӿ�ʼ���㣬��λ s��
	uint16_t remainLifeValue; //������ʣ��Ѫ��
	float realChassisOutV;    //ʵʱ���������ѹ����λV��
	float realChassisOutA;    //ʵʱ���������������λ A��
	tLocData locData;         //LOC ״̬���� tLocData �ṹ�嶨��
	float remainPower;        //ʣ����������ֵ 60J��
}tGameInfo; //����������Ϣ��0x0001��

typedef __packed struct
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
}tRealBloodChangedData;//ʵʱѪ���仯���ݣ�0x0002��

typedef __packed struct
{
	float realBulletShootSpeed; //�ӵ�ʵʱ���٣�m/s��
	float realBulletShootFreq;  //�ӵ�ʵʱ��Ƶ���� /s��
	float realGolfShootSpeed;   //�߶���ʵʱ���٣��� /s��
	float realGolfShootFreq;    //�߶���ʵʱ��Ƶ ( �� /s Ӣ�ۻ����� )
}tRealShootData;    //ʵʱ�����Ϣ (0��0003)

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
}tSelfDefineInfo;   //ѧ���ϴ��Զ������� (0��0005)   ����Ƶ����� 200Hz.

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	__packed union 
	{
		tGameInfo       GameInfo;    //����������Ϣ ��0x0001��
		tRealBloodChangedData   RealBloodChangedData; //ʵʱѪ���仯���ݣ�0x0002��
		tRealShootData   RealShootData; //ʵʱ�����Ϣ (0��0003)
		tSelfDefineInfo SelfDefineInfo; //�Զ������� ��0x0005��	
	}Data;
	uint16_t        CRC16;         //֮ǰ��������CRCУ��   ע������ݺ�֮ǰ�����ݿ��ܲ����������Բ�Ҫֱ��ʹ�ã�����Ҫֱ��ʹ�ã������ڴ˸�ֵ
}tFrame;  //����֡

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tGameInfo       GameInfo;   
	uint16_t        CRC16;         //����CRCУ��
}tGameInfoFrame;  //����������Ϣ֡ ��0x0001��
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tRealBloodChangedData   RealBloodChangedData;   
	uint16_t        CRC16;         //����CRCУ��
}tRealBloodChangedDataFrame; //ʵʱѪ���仯���ݣ�0x0002��
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tRealShootData   RealShootData;   
	uint16_t        CRC16;         //����CRCУ��
}tRealShootDataFrame;    //ʵʱ�����Ϣ (0��0003)
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tSelfDefineInfo SelfDefineInfo;   
	uint16_t        CRC16;         //����CRCУ��
}tSelfDefineFrame;  //�Զ�������֡ ��0x0005��

void DataVerify(void);
// ʹ�÷��� �����û����� Send_FrameData(SelfDefinedDara, userMessage,tSelfDefineInfo(userMessage)); 
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength); 


#endif

