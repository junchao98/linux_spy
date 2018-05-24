


#ifndef COMMON
#define COMMON

/**************配置信息*************/
//#define	BOTTOM	1		//底层开发时定义h
#define	UPPER	1	//QT开发时定义h

//当前设备默认地址h
#define  ADDRESS  0x12

/**************^^^^^^*************/

//调试信息输出
#define DBUG_INF	0

//通用缓冲区大小
#define CACHE_SIZE  200	

//数据处理缓缓存大小
#define MAX_RELIVE_SIZE		CACHE_SIZE+100

// data 区上限
#define CHAR_FULL 240



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

	E_HEART = 40,			//心跳

	E_ALL_NUM,
};


typedef enum E_ORDER order_id_type;

extern const u8 order_list[][4];	//定义见msg_core,c

/*AES 私匙*/
extern const u8 enc_key[16];


#endif // !COMMON

