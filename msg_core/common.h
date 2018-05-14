


#ifndef COMMON
#define COMMON

/**************配置信息*************/
//#define	BOTTOM	1		//底层开发时定义h
#define	UPPER	1	//QT开发时定义h

//当前设备地址h
#define  ADDRESS  0x12

/**************^^^^^^*************/

//调试信息输出
#define DBUG_INF	0

//数据处理缓缓存大小
#define RELIVE_SIZE		20

// data 区上限
#define CHAR_FULL 240

//通用缓冲区大小
#define CACHE_SIZE  100	

#ifdef	UPPER

#define u8	char
#define u32 int

#include <malloc.h>
#include <memory.h>

#endif


#ifdef BOTTOM
#include "stm32f10x.h"

#endif
#include <stdlib.h>

/*上层调用命令集h
#define 	UP 		0		//前进h
#define 	DOWN 	1		//后退h
#define 	LEFT 	2		//左转h
#define 	RIGHT 	3		//右转 h
#define 	CW 		4		//原地顺时针旋转h
#define 	CCW 	5		//原地逆时针旋转 h
#define 	FAST 	6		//快速前进h
#define 	STOP 	7		//强制停止h
#define 	FIX 	8		//车体固定h
#define 	WEAPON 	9		//武器状态h
#define 	LASER 	10		//激光火力h
#define 	ELE 	11		//电磁火力h
#define 	TEMP 	12		//温度采集h
#define 	VELO 	13		//速度h
#define 	STATE  	14		//状态h
#define 	DEP 	15		//写配置h
*/

/*对应order_list 偏移*/
enum E_ORDER
{
	E_UP = 0,	//前进h
	E_DOWN,		//后退h
	E_LEFT,		//左转h
	E_RIGHT,	//右转 h
	E_CW,		//原地顺时针旋转h
	E_CCW,		//原地逆时针旋转 h
	E_FAST_UP,  // 快速前进h
	E_STOP,		//强制停止h
	E_FIX,		//车体固定h

	E_WEAPON_STATUS = 17,	//武器状态h
	E_LASER_FIRE,			//激光火力h
	E_ELE_FIRE,				// 电磁火力h

	E_TEMP_VAL = 24,		//温度采集h
	E_VELO_VAL,				//速度h
	E_HUM_VAL,				//湿度信息
	E_CONFIG_VAL,			//写配置

	E_TOP_INF = 33,			//读top信息
	E_FREE_INF,				//读内存信息
	E_DISK_INF,				//读磁盘信息


	E_ALL_NUM,
};


typedef enum E_ORDER order_id_type;

extern const u8 order_list[][4];	//定义见msg_core,c

/*AES 私匙*/
extern const u8 enc_key[16];


#endif // !COMMON

