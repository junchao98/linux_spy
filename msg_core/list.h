
#ifndef LIST

#define LIST


#define CYCLE  1		//循环链表
#define NO_CYCLE	0	//非循环链表

#define QUEUE	1
#define STACK	0



struct _list {

	char grade;
	struct _list * next;
	//int data[];		//柔性数组  *********

	int data;
	 char p_data[];
};


struct _queue {

	struct _list * head;
	struct _list * tail;
};

int creat_list(struct _list * head, int len, int cycle);

struct _list * list_add_node(struct _list * head, int point,  int data_size);

int list_del_node(struct _list * head, int point);

int destory_list(struct _list * head, int len);

int list_find_node(struct _list * head, int data);

int list_print(struct _list * head, int ch);

/***********************************/

int push_stack(struct _list * head, int data);

int pop_stack(struct _list * head);

int creat_stack(struct _list * head, int len, int data_size);

/******************************************/

struct _list * queue_add(struct _list * head, int data_size);

int queue_del(struct _list * head);

#endif // !1



