
#include "list.h"
#include "stdlib.h"
#include "common.h"
#include "stdio.h"


int list_node_size = 0;   //记录节点个数

/*->创建链表

@head 链表结构体头指针
@len 创建的链表长度
@data_size 数据大小
@cycle 是否创建循环链表
*/

int creat_list(struct _list * head, int len, int cycle)
{

	int i = 0;
	struct _list *p = NULL;


	if (cycle == CYCLE){
	
		head->next = head;
		
	}else if (cycle == NO_CYCLE){

		head->next = NULL;
	
	}
	
	head->p_data = NULL;
	head->data = 0;

	list_node_size = len;
	
	for (i = 1; i <= len; i ++) {

		p = malloc(sizeof(struct _list));

		if (NULL == p)goto error1;

		p->next = head->next;
		p->p_data = NULL;
		p->data = 0;

		head->next = p;
	}

	return 0;

error1:
	destory_list(head, i);
	return -1;
}

/*-> 销毁链表

@head 链表结构体头指针
@len  链表长度

*/

int destory_list(struct _list * head, int len)
{
	struct _list * tmp = NULL;
	
	//if (len > ORDER_SIZE)return -1;

	head = head->next;

	while (len ) {

		tmp = head->next;

		if(NULL != head->p_data) free(head->p_data);		//bug 释放时触发断点

		free(head);

		head = tmp;

		len-=1;
	}
	
	return 0;
}



/*-> 添加一个节点

@head 链表结构体头指针
@point 添加的位置
@data 添加的数据
@data_size

*/

struct _list * list_add_node(struct _list * head, int point, int data_size)
{

	int i = 0;
	struct _list *tmp ;
	struct _list * p;


	head->data += 1;	//	链表节点数目

	p = malloc(sizeof(struct _list)+sizeof(u8) * data_size);

	p->p_data = malloc(sizeof(u8) * (data_size + 1));
	
	p->data = data_size;

	tmp = head;

	for (i = 0; i < point; i++) {
		
		if (tmp->next != NULL) {
			tmp = tmp->next;	//移动指针
		}
		else {
			goto error1;
		}
	}

	p->next = tmp->next;		//完成交换
	tmp->next = p;

	list_node_size += 1;

	return p;

error1:
	free(p);
	return NULL;

}


/*-> 删除一个节点
**
@head 链表结构体头指针
@point 删除的位置
*
*/
int list_del_node(struct _list * head, int point)
{

	int i = 0,
		data = 0;

	struct _list *tmp = NULL,
				 *p = NULL;

	tmp = head;

	//point--;

	for (i = 1; i < point; i++) {
	
		if (tmp->next != NULL) {
			tmp = tmp->next;	//移动指针
		}
		else {
			return -1;
		}
		
	}


	p = tmp->next;
	data = p->data;

   tmp->next = tmp->next->next;
	

	free(p);

	list_node_size -= 1;

	return data;
}


/*-> 寻找节点
**
@head 链表结构体头指针
@data 寻找的数据
*
*return  数据位置
*/
int list_find_node(struct _list * head, int data)
{
	int i = 0;
	struct _list * p = NULL;

	while (1){
	

		if (p->data == data)return i;		//找到节点
		if (p->next == NULL)return -1;
		p = p->next;

		i++;
	}



}


/*-> 打印所有节点
**
@head 链表结构体头指针
*
*return  
*/
int list_print(struct _list * head, int ch)
{
	struct _list * p =	NULL;

	p = head;

	while (1){

		if (ch == STACK){if (p->data > 0) printf("%c \n", p->data);}

		if (ch == QUEUE){if (p->data > 0) printf("%d \n", p->data);}
		
		
		p = p->next;

		if (p == NULL)return 0;
	}

}


/*-> 创建栈
**
@head 链表结构体头指针
@len 栈长度
@data_size  未启用（无效参数）
*
*return
*/

int creat_stack(struct _list * head, int len, int data_size)
{

	creat_list(head, data_size, NO_CYCLE);

	return 0;
}



/*->出栈
* 栈都是对栈顶操作（链表的最后一个数据）
*return 被出栈的数据
*/

int pop_stack(struct _list * head)
{

	return list_del_node(head, list_node_size);

}

/*->入栈
* 栈都是对栈顶操作（链表的最后一个数据）
*return 0
*/

int push_stack(struct _list * head, int data)
{

	list_add_node(head, list_node_size, data);

	return 0;

}



/*->入队
* 入队都是对队尾操作（链表的最后一个数据）
*return 0
*/

struct _list * queue_add(struct _list * head, int data_size)
{

	return list_add_node(head, list_node_size, data_size);
}

/*->出队
* 出队都是对队头操作（链表的第一个数据）
*return 被出队的数据
*/

int queue_del(struct _list * head)
{
	
	return list_del_node(head, 1);
}


