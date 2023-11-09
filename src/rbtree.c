#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) 
{
  // (rbtree *) 자료형을 calloc으로 메모리 할당
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));

  // calloc으로 메모리 할당을 했으므로 nil_node의 멤버들은 0으로 초기화 된다. (포인터가 0으로 초기화 되면 NULL을 의미한다)
  node_t *nil_node = (node_t*)calloc(1,sizeof(node_t));
  nil_node->color = RBTREE_BLACK;

  // (rbtree *) 구조체 t의 멤버를 위에서 만든 nil_node로 초기화 해준다
  t->nil = nil_node;
  t->root = t->nil;
  return t;
}

void postorder_delete(node_t *root, node_t *nil)
{ 
	// 후위 순회로 돌면서 노드를 삭제
  if (nil == root)  // nil_node가 root라는 것은 처음 초기화 상태와 같으므로, 트리에 노드가 없는 상태
	  return;

  postorder_delete(root->left, nil); // 후위 순회이기에 왼쪽부터 확인
  postorder_delete(root->right, nil); // 더이상 왼쪽 노드가 없다면, 오른쪽 노드 확인
  free(root);
}

void delete_rbtree(rbtree *t) 
{
  postorder_delete(t->root, t->nil);

  free(t->nil); // nil_node 해제
  free(t); // tree 해제
}

void left_rotate(rbtree *t, node_t *cur_node)
{
  node_t *sub_node = cur_node->right; // 회전 후 나를 대체할 노드를 설정

  cur_node->right = sub_node->left;
  if (sub_node->left != t->nil) // sub_node의 왼쪽 자식이 실제 노드가 있다면 쌍방 연결. 부모->자식, 자식->부모  포인터 연결
    sub_node->left->parent = cur_node; // 자식->부모 포인터 연결

  sub_node->parent = cur_node->parent; // 부모->자식 포인터 연결

  if(cur_node->parent == t->nil) // 회전하는 내가 부모 노드가 없다면, 내가 루트 노드인 상태
    t->root = sub_node; // 나를 대체할 노드가 루트 노드가 되어야 한다
  else if (cur_node == cur_node->parent->left) // 부모 노드가 있다면, 내가 부모의 왼쪽인지, 오른쪽인지 체크 
    cur_node->parent->left = sub_node;
  else
    cur_node->parent->right = sub_node;

  //나와 대체할 노드의 관계 정리
  sub_node->left = cur_node; 
  cur_node->parent = sub_node;
}

void right_rotate(rbtree *t, node_t *cur_node)
{
  node_t *sub_node = cur_node->left;

  cur_node->left = sub_node->right;
  if (sub_node->right != t->nil)
    sub_node->right->parent = cur_node;

  sub_node->parent = cur_node->parent;

  if(cur_node->parent == t->nil)
    t->root = sub_node;
  else if (cur_node == cur_node->parent->left)
    cur_node->parent->left = sub_node;
  else
    cur_node->parent->right = sub_node;
  
  sub_node->right = cur_node;
  cur_node->parent = sub_node; 
}

void insert_fixup(rbtree *t, node_t *cur_node)
{                                   
  while (cur_node->parent->color == RBTREE_RED) // 트리 수정은 내 부모가 RED일 경우, 레드가 2개 연속오는 상황이 문제가 되므로 부모의 색이 RED일 경우 계속 수정을 한다
  {  
    if (cur_node->parent == cur_node->parent->parent->left) // 부모 노드가 어느 방향 자식인지에 따라 rotate 방향이 반대가 되므로 우선 왼쪽 자식일 경우를 고려해본다
    {
      node_t *uc_node = cur_node->parent->parent->right; // 삼촌 노드 초기화 설정
      if (uc_node->color == RBTREE_RED)  // 삼촌 노드가 빨강이라면 / Case 1
      {
        cur_node->parent->color = RBTREE_BLACK;
        uc_node->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        cur_node = cur_node->parent->parent;
      }
	  else  // 삼촌 노드가 검정이라면 / Case 2 or Case 3
      {
        if (cur_node == cur_node->parent->right) // Case 2
        {
          cur_node = cur_node->parent;
          left_rotate(t, cur_node);
        }
        cur_node->parent->color = RBTREE_BLACK; // Case 3      
        cur_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, cur_node->parent->parent); 
      }
    }
    else // 부모 노드가 오른쪽 자식일 경우
    {
      node_t *uc_node = cur_node->parent->parent->left;  
      if (uc_node->color == RBTREE_RED) 
      {
        cur_node->parent->color = RBTREE_BLACK;
        uc_node->color = RBTREE_BLACK;
        cur_node->parent->parent->color = RBTREE_RED;
        cur_node = cur_node->parent->parent;
      }
	  else 
      {
        if (cur_node == cur_node->parent->left)  
        {
          cur_node = cur_node->parent;      
          right_rotate(t, cur_node);
        }
        cur_node->parent->color = RBTREE_BLACK;          
        cur_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, cur_node->parent->parent);
      }        
    }  
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) 
{
  // input_node를 메모리 할당을 새로 해서 만들어야 함
  node_t *input_node = (node_t*)calloc(1,sizeof(node_t));
  input_node->key = key;
  input_node->left = t->nil;
  input_node->right = t->nil;
  input_node->color = RBTREE_RED;

  node_t *cur_node = t->root; // 현재 검사 중인 노드
  node_t *tracking_node = t->nil;  // 현재 검사 중인 노드의 부모 노드를 저장하는 용도. 검사하는 노드의 한 단계 전 노드를 백업해놓는 용도.
  while (cur_node != t->nil)
  {
    tracking_node = cur_node; // tracing_node에 현재 노드 위치를 백업하고, 검사를 위해 한 단계 내려간다
    if(input_node->key < cur_node->key)
      cur_node = cur_node->left;
    else
		cur_node = cur_node->right;
  }

  input_node->parent = tracking_node; // tracking_node에 부모 노드를 저장했기에 input_node->부모 포인터 연결이 가능
  if (tracking_node == t->nil)  // 저장해둔 부모 노드가 nil_node라면, 지금 내가 루트 노드
	  t->root = input_node;
  else if (input_node -> key < tracking_node->key) // 부모가 nil_node가 아니라면, 내가 왼쪽으로 붙을지, 오른쪽으로 붙을지 key값을 비교하여 결정
	  tracking_node->left = input_node;
  else 
	  tracking_node->right = input_node;

  insert_fixup(t, input_node);  // 삽입 후 정리 필요
  return input_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) 
{
  node_t *cur_node = t->root;
  while (cur_node != t->nil)               
  {
    if (cur_node->key > key)
      cur_node = cur_node->left;
    else if(cur_node->key < key)
      cur_node = cur_node->right;
    else
      break;
  }

  if (cur_node == t->nil)
    return NULL;
  else
    return cur_node;
}

node_t *return_min(node_t *cur_node, node_t *nil)
{
  node_t *min_node = cur_node;
  while (cur_node != nil)               
  {
    min_node = cur_node;
    cur_node = cur_node->left; 
  }
  return min_node;
}

node_t *rbtree_min(const rbtree *t) 
{
  node_t *min_node;
  min_node = return_min(t->root, t->nil);
  return min_node;
}

node_t *rbtree_max(const rbtree *t) 
{
  node_t *cur_node = t->root;
  node_t *max_node = t->root;
  while (cur_node != t->nil)               
  {
    max_node = cur_node;
    cur_node = cur_node->right; 
  };
  return max_node;
}

void rbtree_erase_fixup(rbtree *t, node_t *cur_node)
{
  while (cur_node != t->root && cur_node->color==RBTREE_BLACK) // 삭제 후 대체된 내가 루트 노드가 아니고, 검은색이면 doubly-black이므로 수정 반복  
  {
    if(cur_node == cur_node->parent->left) // 내가 부모의 어느쪽 자식인지에 따라 회전 방향이 반대이므로, 우선 내가 왼쪽 자식일 경우를 확인
	{  
      node_t *bro_node = cur_node->parent->right; // 형제 노드 설정
      if(bro_node->color == RBTREE_RED) // 형제 노드의 색이 RED / Case 1
	  { 
        bro_node->color = RBTREE_BLACK;
        cur_node->parent->color = RBTREE_RED;
        left_rotate(t, cur_node->parent);
        bro_node = cur_node->parent->right;
      }                 
      if(bro_node->left->color == RBTREE_BLACK && bro_node->right->color == RBTREE_BLACK) // Case 2
	  {
        bro_node->color = RBTREE_RED;
        cur_node = cur_node->parent;
      }
	  else 
	  {
        if (bro_node->right->color == RBTREE_BLACK) // Case 3
		{
          bro_node->left->color = RBTREE_BLACK;
          bro_node->color = RBTREE_RED;
          right_rotate(t, bro_node);
          bro_node = cur_node->parent->right;
        }
        bro_node->color = cur_node->parent->color; // Case 4
        cur_node->parent->color = RBTREE_BLACK;
        bro_node->right->color = RBTREE_BLACK;
        left_rotate(t, cur_node->parent);
        cur_node = t->root;
      }
    }
	else // 내가 오른쪽 자식일 경우
	{ 
      node_t *bro_node = cur_node->parent->left;
      if(bro_node->color == RBTREE_RED)
	  {
        bro_node->color = RBTREE_BLACK;
        cur_node->parent->color = RBTREE_RED;
        right_rotate(t, cur_node->parent);
        bro_node = cur_node->parent->left;
      }
      if(bro_node->left->color == RBTREE_BLACK && bro_node->right->color == RBTREE_BLACK)
	  {
        bro_node->color = RBTREE_RED;
        cur_node = cur_node->parent;
      }
	  else
	  { 
        if (bro_node->left->color == RBTREE_BLACK)
		{
          bro_node->right->color = RBTREE_BLACK;
          bro_node->color = RBTREE_RED;
          left_rotate(t, bro_node);
          bro_node = cur_node->parent->left;
        }
        bro_node->color = cur_node->parent->color;
        cur_node->parent->color = RBTREE_BLACK;
        bro_node->left->color = RBTREE_BLACK;
        right_rotate(t, cur_node->parent);
        cur_node = t->root;
      }      
    }
  }
  cur_node->color = RBTREE_BLACK;
}

void rbtree_transplant(rbtree *t, node_t *cur_node, node_t *sub_node)
{
  if (cur_node->parent == t->nil)
    t->root = sub_node;
  else if (cur_node == cur_node->parent->left)
    cur_node->parent->left = sub_node;
  else
    cur_node->parent->right = sub_node;
  
  sub_node->parent = cur_node->parent;
}

int rbtree_erase(rbtree *t, node_t *del_node) 
{
  node_t *cur_node = del_node;  // 삭제 할 노드 정보 저장                      
  color_t del_node_color = cur_node->color; 
  node_t *sub_node;              

  // 우선 삭제 노드의 자식이 1명 이하일 경우
  if (del_node->left == t->nil)  // 자식이 오른쪽만 있을 경우
  {                    
    sub_node = del_node->right;                            
    rbtree_transplant(t, del_node, del_node->right);
  }
  else if (del_node->right == t->nil) // 자식이 왼쪽만 있을 경우
  {  
    sub_node = del_node->left;     
    rbtree_transplant(t, del_node, del_node->left);
  }
  else // 자식이 2명인 경우
  {                  
    cur_node = return_min(del_node->right, t->nil); // 삭제하려는 노드의 오른쪽 서브 트리에서 가장 작은 값이 대체 노드가 된다      
    del_node_color = cur_node->color; // 삭제 색을 대체 노드의 색으로 업데이트         
    sub_node = cur_node->right;                               
    if (cur_node->parent == del_node)
      sub_node->parent = cur_node;                            
    else 
	{         
      rbtree_transplant(t, cur_node, cur_node->right);
      cur_node->right = del_node->right;               
      cur_node->right->parent = cur_node;                   
    }
    rbtree_transplant(t, del_node, cur_node);        
    cur_node->left = del_node->left;
    cur_node->left->parent = cur_node;
    cur_node->color = del_node->color;
  }
  free(del_node);   
  del_node = NULL;

  if(del_node_color == RBTREE_BLACK) // 삭제 완료 후 del_color가 doubly_black이면 트리 수정
    rbtree_erase_fixup(t, sub_node);                
											
  t->nil->parent = NULL;
  t->nil->right = NULL;
  t->nil->left = NULL;
  t->nil->color = RBTREE_BLACK;
  
  return 0;
}
                              
void inorder_arr(node_t *root, node_t *nil, key_t *arr, int *index)
{
  if (root == nil) 
	  return;

  inorder_arr(root->left, nil, arr, index);
  arr[(*index)++] = root->key; 
  inorder_arr(root->right, nil, arr, index);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  int *index = calloc(1, sizeof(int));
  inorder_arr(t->root, t->nil, arr, index);
  free(index);
  return 0;
}

