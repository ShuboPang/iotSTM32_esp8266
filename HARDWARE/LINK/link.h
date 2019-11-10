#ifndef _LINK_H_
#define _LINK_H_

#define OK 0
#define ERROR -1


typedef int ElemType;

//定义链表的结点
typedef struct LNode
{
  ElemType data;//数据域
  struct LNode *next;//指针域
}LNode;


int create_LinkList(LNode* L);
int Rcreate_LinkList(LNode* L);
int Insert_LNode(LNode* L,int pos,ElemType e);
int Delete_LNode(LNode* L,int pos);
ElemType Search_by_pos(LNode* L,int pos);
LNode* Search_by_data(LNode * L,ElemType e);
int Get_Length(LNode* L);
int destroy_LinkList(LNode* L);
int printf_Link(LNode* L);









#endif