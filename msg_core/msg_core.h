

#ifndef DATA_KEY
#define DATA_KEY

#include "common.h"

	

//命令列大小
#define ORDER_SIZE  4		

//优先级
#define  GRADE_HIGH		 1	
#define  GRADE_MEDIUM	 2
#define  GRADE_LOW		 3

/*数据类型 
*数据类型的解释见t_task仓库的通讯核心文件夹
*/
#define  TY_ORDER	1	
#define  TY_DATA	0
#define  TY_KEY		2
#define  TY_MSG		3
#define  TY_TIME	4

/*
TY_ORDER 为控制命令类的数据		data域长度固定为4
TY_MSG   为较短的信息传输		data域长度为 1-1000
TY_DATA  为文件等大数据传输
*/


//数据状态
#define  WORKED		1	//已处理
#define  WORKING	0	//等待处理

#define LEN_BIT		3		//偏移
#define BASE_BIT	3
#define TYPE_BIT	2
#define CHECK_BIT	1
#define STATUS_BIT  1
#define FROM_ADDR_BIT 1
#define TO_ADDR_BIT	1
#define END_BIT		7

//保留区填充
#define SOLID 0 

#define BROADCAST_ADDR	0x00
#define DEFAULT_ADDR	0X01
#define SERVER_ADDR		0X02


//发送到stm32
struct slave_order {

	u8 status;		//状态 
	u8 order[4];	//命令 
	u8 grade;		//等级 
};


struct born_order {

	u8 form;
	u8 to;

	u8 grade;	//优先级
	u8 type;	//数据类型
	u8 len;		//data域长度
	
	/*参数单位 
		距离 m
		角度 度
	*/
	u8 para;	//附加参数

	u8 *data;
};

extern u8 my_addr;
extern u8 to_addr;

inline void set_myaddr(u8 addr)
{
	
	my_addr = addr;
}

inline void set_toaddr(u8 addr)
{

	to_addr = addr;
}

void char_to_int(u32 * data1, u8 * data2, u8 len);

/**init_order_list
*初始化命令队列

* @
* @ret: 返回队列头的指针
*/
struct _list *  init_order_list(void);

/**order_init
*上层 order默认参数填充
*@egg	
*@ret born_order指针
*/
void order_init(struct born_order * egg);

void msg_init(struct born_order * egg, int data_len);

/*born_order 结构体默认填充h*/
void time_init(struct born_order * egg, int time_s);

/**generdate_data
*上层 合成完整数据
*
*@st_data: 待合成结构体指针
*ret: 返回合成后的命令加密后指针
*/
u8 * generdate_data(struct born_order * st_data);

void bat_generdate_data(struct born_order * st_data, void *send_data(u8 *data, u32 num), u32 num);

/*free空间h*/
#define order_end(x, y)  do{ free(x);  free(y);}while(0)


/**获取数据目的地址
*上层
*@一条数据
*
*ret: 一个地址
*/
inline u8 get_data_toaddr(const char *data) {

	return data[BASE_BIT + data[BASE_BIT] + CHECK_BIT + STATUS_BIT + FROM_ADDR_BIT + TO_ADDR_BIT];

}


/**cook_data
* 处理传入的数据流，提取命令
*
*@in_data  数据指针
*@len: 数据总长度
*@ret: 返回0则处理完毕
*/
u8 cook_data(u8 * in_data, u32 len);

void generdata_msg(u8 data[], u8 type, u8 int_area, u8 dec_area);

/**find_cmd
*底层
*@cmd:接收到的长度为6*sizeof(char)的命令
*ret: 返回cmd在order_list中的下标
*/
int find_cmd(u8 *cmd);

#endif