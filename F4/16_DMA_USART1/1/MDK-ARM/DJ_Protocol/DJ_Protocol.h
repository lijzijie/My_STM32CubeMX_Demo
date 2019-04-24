#ifndef _DJ_PROTOCOL_H_
#define _DJ_PROTOCOL_H_

#include "stm32f4xx.h"
//#include "USART.h"
//#include "USART2.h"	

/***********************************************
使用方法：
1、初始化串口； 实现串口发送函数
2、更改宏定义 USART_SendBuff ，将其改为实际串口发送函数名  xxxxxx   （该函数参数为 uchar *buff,uchar len    len为发送长度）
3、变量 tCmdID  cmdid = SelfDefinedData; //自定义数据 
				tSelfDefineInfo SelfDefineInfo = {.data1 = 1.133f,.data2 = 1.133f,.data3 = 1.143f};//自定义传输数据
   调用函数 Send_FrameData(cmdid,(uint8_t *)&SelfDefineInfo,sizeof(SelfDefineInfo));  //发送自定义数据至裁判系统
4、如果需要接收裁判系统发来的数据，需要实现串口中断接收函数，接收标志为 USART2_RX_STA，缓冲地址 USART2_RX_BUF
	然后在主程序调用 DataVerify() 函数，然后可以用另一个串口发送到串口数据观察（自行实现printf函数）。
	--------------------------------------------------------------------------2017/6/18
***********************************************/


// uart 通信配置，波特率 115200, 数据位 8, 停止位 1，检验位无，流控制无。
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1

#define USART_SendBuff 			HAL_UART_Transmit_DMA   //USART_Send_BUFF(uchar *buff,uchar len) 

/********************  结束更改  ***************************/
//0-3 bits
#define Strike_0   0x00 //0号装甲面（前）
#define Strike_1   0x01 //1号装甲面（左）
#define Strike_2   0x02 //2号装甲面（后）
#define Strike_3   0x03 //3号装甲面（右）
#define Strike_4   0x04 //4号装甲面（上1）
#define Strike_5   0x05 //5号装甲面（上2）
//4-7 bits
#define BC_Strike          0x00 //装甲伤害（受到攻击） BloodChanged    BC
#define BC_ShootSpeed      0x01 //子弹超速扣血
#define BC_ShootFreq       0x02 //子弹超频扣血
#define BC_PowerLimit      0x03 //功率超限
#define BC_ModuleOffline   0x04 //模块离线扣血
#define BC_CommonFoul      0x06 //普通犯规扣血
#define BC_Tarmac          0x0a //停机坪加血
#define BC_EngineerOuto    0x0b //工程机器人自动回血


typedef __packed struct
{
	uint8_t SOF;          //数据起始字节，固定为0xA5          
	uint16_t DataLength;  //数据长度
	uint8_t Seq;          //包序号
	uint8_t CRC8;         //帧头CRC校验
}tFrameHeader;//帧头

typedef enum 
{
	GameInfo = 0x0001,     //比赛进程信息    发送频率 50 Hz
	RealBloodChangedData,  //实时血量变化数据
	RealShootData,             //实时射击数据
	SelfDefinedData =0x0005, //学生自定义数据
	Wrong = 0x1301 //枚举无效，只是为了使该枚举大小为2字节
}tCmdID; 
typedef __packed struct
{
	uint8_t flag;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t compass; //方位
}tLocData; //位置信息

typedef __packed struct
{
	uint32_t remainTime;       //比赛剩余时间（从倒计时三分钟开始计算，单位 s）
	uint16_t remainLifeValue; //机器人剩余血量
	float realChassisOutV;    //实时底盘输出电压（单位V）
	float realChassisOutA;    //实时底盘输出电流（单位 A）
	tLocData locData;         //LOC 状态，见 tLocData 结构体定义
	float remainPower;        //剩余能量（满值 60J）
}tGameInfo; //比赛进程信息（0x0001）

typedef __packed struct
{
	uint8_t weakId:4;
	uint8_t way:4;
	uint16_t value;
}tRealBloodChangedData;//实时血量变化数据（0x0002）

typedef __packed struct
{
	float realBulletShootSpeed; //子弹实时射速（m/s）
	float realBulletShootFreq;  //子弹实时射频（发 /s）
	float realGolfShootSpeed;   //高尔夫实时射速（发 /s）
	float realGolfShootFreq;    //高尔夫实时射频 ( 发 /s 英雄机器人 )
}tRealShootData;    //实时射击信息 (0×0003)

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
}tSelfDefineInfo;   //学生上传自定义数据 (0×0005)   发送频率最大 200Hz.

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	__packed union 
	{
		tGameInfo       GameInfo;    //比赛进程信息 （0x0001）
		tRealBloodChangedData   RealBloodChangedData; //实时血量变化数据（0x0002）
		tRealShootData   RealShootData; //实时射击信息 (0×0003)
		tSelfDefineInfo SelfDefineInfo; //自定义数据 （0x0005）	
	}Data;
	uint16_t        CRC16;         //之前所有数据CRC校验   注意此数据和之前的数据可能不连续，所以不要直接使用，若需要直接使用，必须在此赋值
}tFrame;  //数据帧

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tGameInfo       GameInfo;   
	uint16_t        CRC16;         //数据CRC校验
}tGameInfoFrame;  //比赛进程信息帧 （0x0001）
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tRealBloodChangedData   RealBloodChangedData;   
	uint16_t        CRC16;         //数据CRC校验
}tRealBloodChangedDataFrame; //实时血量变化数据（0x0002）
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tRealShootData   RealShootData;   
	uint16_t        CRC16;         //数据CRC校验
}tRealShootDataFrame;    //实时射击信息 (0×0003)
typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	tSelfDefineInfo SelfDefineInfo;   
	uint16_t        CRC16;         //数据CRC校验
}tSelfDefineFrame;  //自定义数据帧 （0x0005）

void DataVerify(void);
// 使用方法 发送用户数据 Send_FrameData(SelfDefinedDara, userMessage,tSelfDefineInfo(userMessage)); 
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength); 


#endif

