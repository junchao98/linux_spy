
#include <stdio.h>
#include "msg_core.h"
#include "list.h"
#include "common.h"

#ifdef  BOTTOM
//#include "mbedtls/aes.h"
#endif //  BOTTOM


 u8 const order_list[][4] = {

	/*运动控制*/ 
	{ 0x7,0x7,0x5,0x0 },		//前进 				0
	{ 0x7,0x7,0x2,0x0 },		//后退 				1
	{ 0x7,0x7,0x1,0x0 },		//左转 				2
	{ 0x7,0x7,0x3,0x0 }, 		//右转 				3
	{ 0x7,0x4,0x4,0x0 }, 		//原地顺时针旋转 	4
	{ 0x7,0x4,0x5,0x0 }, 		//原地逆时针旋转 	5
	{ 0x7,0x6,0x8,0x0 }, 		//快速前进 			6
	{ 0x7,0x5,0x8,0x0 }, 		//强制停止			7
	{ 0x7,0x5,0x6,0x0 }, 		//车体固定 			8

	/*保留*/
	{ 0x0,0x0,0x0,0x0 },		//					9
	{ 0x0,0x0,0x0,0x0 },		//					10
	{ 0x0,0x0,0x0,0x0 },		//					11
	{ 0x0,0x0,0x0,0x0 },		//					12
	{ 0x0,0x0,0x0,0x0 },		//					13
	{ 0x0,0x0,0x0,0x0 },		//					14
	{ 0x0,0x0,0x0,0x0 },		//					15
	{ 0x0,0x0,0x0,0x0 },		//					16

	/*火力系统*/							
	{ 0x8,0x8,0x0,0x0 },		//武器状态			17
	{ 0x8,0x8,0x7,0x0 },		//激光火力			18
	{ 0x8,0x3,0x2,0x0 },		//电磁火力			19

	/*保留*/
	{ 0x0,0x0,0x0,0x0 },		//					20
	{ 0x0,0x0,0x0,0x0 },		//					21
	{ 0x0,0x0,0x0,0x0 },		//					22
	{ 0x0,0x0,0x0,0x0 },		//					23
	
	/*环境数据采集*/
	{ 0x1,0x8,0x0,0x0 },		//温度信息			24
	{ 0x1,0x2,0x0,0x0 },		//速度				25
	{ 0x1,0x3,0x0,0x0 },		//湿度信息			26
	{ 0x1,0x7,0x0,0x0 }, 		//写配置			27
	
	/*保留*/
	{ 0x0,0x0,0x0,0x0 },		//					28
	{ 0x0,0x0,0x0,0x0 },		//					29
	{ 0x0,0x0,0x0,0x0 },		//					30
	{ 0x0,0x0,0x0,0x0 },		//					31
	{ 0x0,0x0,0x0,0x0 },		//					32

	/*系统状态信息*/
	{ 0x2,0x1,0x0,0x0 },		//top信息			33
	{ 0x2,0x2,0x0,0x0 },		//内存信息			34
	{ 0x2,0x3,0x0,0x0 },		//磁盘信息			35
	{ 0x2,0x4,0x0,0x0 },

	{ 0x0,0x0,0x0,0x0 },
	{ 0x0,0x0,0x0,0x0 },
	{ 0x0,0x0,0x0,0x0 },
	{ 0x0,0x0,0x0,0x0 },
};
					
const u8 enc_key[16] = { 0x22, 0xa2, 0x13, 0xe2, 0x22, 0xa2, 0x12, 0x20, 0x22, 0x15, 0x22, 0xda, 0x67, 0x22, 0x32, 0x22 };
//mbedtls_aes_context aes;


u8 *tmp_data = NULL;

/*缓冲信息*/
u8 relive[RELIVE_SIZE] = { 0 };		

/*relive 数组下标指针*/
u32 re_point = 0;	

static void analyze_data(u8 in_data[], struct _list * p);
static u32 check_data(u8 * in_data);
static int kill_data(u8 * data, int num);

struct _list *head = NULL;			//信息链表头



/**
* @
* @param void
* @return boolean:返回队列头的指针
*/
struct _list * init_order_list(void)
{

	head = (struct _list  * )malloc(sizeof(struct _list));

	creat_list(head, 0, NO_CYCLE);

	return head;
}

/**
*kill_data 把一条命令解析为mcu可识别后加入到队列
*
*@idata  数据指针
*
*@num 数据长度
*/

static int kill_data(u8 * data, int num)
{
	struct _list * p = NULL;

	int i = 0;

	p = queue_add(head, num);

	if(p) {
	
		for (i = 0; i < num; i++){
		
			analyze_data(data, p);
		}

	}else{
	
		return -1;
	
	}

	return 0;
}

/**
 *@解析数据为mcu可识别格式(取出data域数据)
 *
 *@ in_data[] 一条完整的命令
 *
 *@ p 解析后存入的结构体
**/

static void analyze_data(u8 in_data[], struct _list * p)
{

	if(in_data[TYPE_BIT] == TY_ORDER){
		int i = 0;
		p->p_data[0] = '#';
		for(i=1; i<=ORDER_SIZE; i++){
		
			p->p_data[i] = in_data[LEN_BIT + i];
		}
		
		p->p_data[5] = '#';
	
	}


}


void char_to_int(u32 * data1, u8 * data2, u8 len)
{
	int i = 0;

	for (i = 0; i < len; i++)data1[i] = data2[i];

}

/**
 *cook_data 处理传入的数据流，提取命令
 *
 *@in_data  数据指针
 *
 *@len: 数据总长度
 *
 *@ret: 返回0则处理完毕
*/

u8 cook_data(u8 *in_data, u32 len)
{
	
	u8 * data = NULL;
	u32 i = 0,
		start = 0,
		tmp_len = 0;
	

	static u8 bad_head = 0;

	data = in_data;


	while(1){

		if (i >= len){		//数据查询完毕，但无尾
		
			for (start; start < len; start++) {

				relive[re_point] = data[start];
				re_point++;
				bad_head = 1;
			
			}
			
			break;
		}
	
	
		if (bad_head) {		//没有尾的后续处理

			while(1){
				
				if (data[i] == '#'){		//找到残尾

					if (i<len - 2) {		//防止访问溢出
						if ('#' == data[i + 1])i++;
					}

					bad_head = 0;
					relive[re_point] = '#';		//尾部追加

					tmp_len = check_data(relive);
					
					if (-1 != tmp_len) kill_data(relive, re_point);		//把命令入队

					re_point = 0;
					bad_head = 0;

					break;
				}

				if (i >= len)return -1;

				relive[re_point] = data[i];

				re_point++;
				i++;	

			} //end while
		
			
		}

		
		if ('#' == data[i]){				//找到头

			if(i<len-2){if ('#' == data[i + 1])i++;}
			
			start = i;
			
			while(1){
			
				if ('#' == data[i]){	  //有头有尾
				
					if (start + LEN_BIT < len){

						tmp_len = check_data(&data[start]);

						if (-1 != tmp_len)kill_data(&data[i], tmp_len);
						
					}
					
					break;	
				} //完成一条命令提取

				i++;					//寻找下一条命令

			}//end while

		
		}


		i++;
	
	} //all while end
	

	return 0; 
}


/**
*上层
*check_data 验校数据完整性
*
*@in_data  数据指针
*
*@return： 验证成功返回数据长度
*/
static u32 check_data(u8 *in_data)
{
	
	u8 i = 0;
	u8 tmp =  0x65;

	u32 len = BASE_BIT + in_data[LEN_BIT] + END_BIT;
	
	if (len <= 0)goto c_error;

	if (len > RELIVE_SIZE)goto c_error;

	if (in_data[len] != '#')goto c_error;  //长度验校

	for (i = 1; i <= in_data[LEN_BIT] ; i++){			//计算验校位
		
		tmp ^= in_data[i + LEN_BIT];
#if DBUG_INF
		printf("data  = %d \n", in_data[LEN_BIT+i]);
#endif		
	}
#if DBUG_INF
	printf("tmp = %d\n", tmp);
#endif
	/*对比校验位*/
	if (tmp != in_data[BASE_BIT + in_data[LEN_BIT] + CHECK_BIT])goto c_error2;
	
	return len+1;

c_error:
#if DBUG_INF
	printf("bad data \n");
#endif
	return -1;
c_error2:

#if DBUG_INF
	printf(" %d \n", in_data[len]);
	printf("check bad data \n");
#endif
	return -1;

}


int dbug(void)
{

	u8 i = 0;
	//list_print(head, STACK);

	for (i = 0; i < head->next->data; i++){
	
	
		printf(" %c ", head->next->p_data[i]);
	}

	printf("list node = %d \n", head->data);

	destory_list(head, head->data);
	
}

static void comm_start(struct born_order * egg)
{

	egg->form = ADDRESS;
	egg->to = 0;

	/*数据优先级*/
	egg->grade = GRADE_HIGH;

}

/**order默认参数填充
 *上层
 *
 *@return born_order指针
 */
void order_init(struct born_order * egg)
{

	comm_start(egg);

	/*data长度*/
	egg->len = 4;

	/*data数据类型*/
	egg->type = TY_ORDER;


}


/**time默认参数填充
*
*
*@return born_order指针
*/
void time_init(struct born_order * egg, int time_s)
{
	int d_len = 0,
		i = 0,
		lave = 0;	//余数
	char lave_flag = 0;
	char *data = NULL;

	comm_start(egg);

	d_len = time_s / 200;
	lave = time_s % 200;

	if (lave){
		d_len++;
		lave_flag = 1;
	
	}

	/*data长度*/
	egg->len = d_len;

	data = (char * )malloc(sizeof(char)  *  d_len);

	/*q全部set 为200*/
	memset(data, 200, sizeof(char) * d_len);

	/*余数处理*/
	if (lave) {		
		d_len -= 1;
		data[d_len] = lave;
	}

	/*data数据类型*/
	egg->type = TY_TIME;
	egg->data = data;

}

/**合成完整数据
 *上层
 *@st_data待合成结构体指针
 *
 *ret: 返回合成后的命令加密后指针
 *
*/
u8 * generdate_data(struct born_order * st_data)
{
	 u32 size = BASE_BIT + st_data->len + END_BIT;

	u8  *data = NULL,
		*enc_data = NULL;

	static unsigned int times = 0;

	u32 offset = 0;
	u32 tmp_len;

	u8 check = 0x65;
	u8 i = 0;

#if DBUG_INF
	printf("len = %d\n", size);
#endif

	//加密数组应大于16
	if (size < 16)size = 16;

	/*flag 应进行分割，每次只能加密16个*/

	data = (u8*)malloc(sizeof(u8) * (size+1));
	enc_data = (u8*)malloc(sizeof(u8) * (size + 1));
	
	memset(data, 0, sizeof(u8) * (size + 1));
	memset(enc_data, 0, sizeof(u8) * (size));
	

	data[0] = '#';
	data[1] = st_data->grade;
	data[2] = st_data->type;

	if (st_data->len > CHAR_FULL) {
		free(st_data);
	}

	if (st_data->type == TY_ORDER){	//order 固定长度4
	
		data[3] = 4;
		tmp_len = 4;
		
	}else {
		data[3] = st_data->len;
		tmp_len = st_data->len;
	}
	
	
	/*COPY DATA*/
	for (i = 0; i < tmp_len; i++) {

		data[4 + i] = st_data->data[i];
#if DBUG_INF
		printf("st_ %x \n", data[i]);
#endif
		/*验校位积累*/
		check ^= st_data->data[i];	
	}

	offset = BASE_BIT  + st_data->len;

	data[offset + 1] = check;
	data[offset + 2] = WORKING;
	data[offset + 3] = ADDRESS;
	data[offset + 4] = st_data->to;

	data[offset + 5] = st_data->para;
	data[offset + 6] = SOLID;

	data[offset + 7] = '#';

	//只设置一次key
	if (!times);
	//mbedtls_aes_setkey_enc(&aes, enc_key, 128);//  set encrypt key  

	//加密
	//mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, data, enc_data);

	times = 0xff;
	

	free(enc_data);

	return data;

}



/**批量生成加密数据
*通用层
*@st_data 批量结构体头指针
*@send_data 发送数据的回调函数
*@num 需要处理的个数
*/

void bat_generdate_data(struct born_order * st_data, void *send_data(u8 *data, u32 num), u32 num)
{

	u8 i = 0;

	for (i = 0; i < num; i++)
	{
		generdate_data(&st_data[i]);

	}

}


/**通用层msg合成
 *@data 用来存放合成后数据，长度4
 *@type 传感器类型
 *@int_area 数据整数部分
 *@dec_area 数据小数部分
*/
void generdata_msg(u8 data[], u8 type, u8 int_area, u8 dec_area)
{

	data[0] = type;
	data[1] = int_area;
	data[2] = dec_area;
	data[3] = 0;

}


/**find_cmd
*底层 命令查询
*@接收到的长度为6*sizeof(char)的命令
*
*#返回cmd在order_list中的下标
*/
int find_cmd(u8 *cmd)
{
	int i = 0;

	if (cmd[1] == 0x8)i = E_WEAPON_STATUS;
	if (cmd[1] == 0x1)i = E_TEMP_VAL;
	if (cmd[1] == 0x2)i = E_TOP_INF;

	for(i; i<E_ALL_NUM; i++){
	
		if(cmd[1] == order_list[i][0] && cmd[2] == order_list[i][1]\
			&& cmd[3] == order_list[i][2] && cmd[4] == order_list[i][3]){
		
			return i;
		
		}
	
	
	}

	return -1;

}







