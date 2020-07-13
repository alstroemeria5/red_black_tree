/*
 * rb.h
 *
 *  Created on: Jun 29, 2020
 *      Author: eric
 */

#ifndef _STDDEF_H_
#include <stddef.h>
#endif
#ifndef _ASSERT_H
#include <assert.h>
#endif
#ifndef RB_H_
#define RB_H_
//#define offsetof(TYPE,MEMBER)	((size_t)&((TYPE*)0)->MEMBER)
#define rb_red 1
#define rb_black 0
#define RB_ROOT_TAG 1<<1
#define RB_NIL_TAG 1<<2
#define container_of(ptr, type, member) ({  \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
           (type *)( (char *)__mptr - offsetof(type,member) );})
#define rb_parent(r)   (r)->rb_parent
#define rb_grandparent(r) ((r)->rb_parent)->rb_parent

#define rb_is_root_by_tag(r) ((r->rb_color|(RB_ROOT_TAG>>2))==1)
#define rb_is_root_by_rb_root(node,root)	(node==root->rb_node)
struct rb_node{
	unsigned long rb_color;
	struct rb_node* rb_left;
	struct rb_node* rb_right;
	struct rb_node* rb_parent;
}__attribute__((aligned(sizeof(long))));
void rb_set_color_red(struct rb_node* r)	{
	r->rb_color=r->rb_color|1UL;
}
void rb_set_color_black(struct rb_node* r){
	r->rb_color=r->rb_color&~1UL;
}
struct rb_node nil={{((unsigned long)0)|(RB_NIL_TAG|rb_black),NULL,NULL,NULL}};
struct rb_node* nil_ptr;
#define RB_ROOT (struct rb_root){((unsigned long)0)|(RB_ROOT_TAG|rb_black),nil_ptr,nil_ptr,nil_ptr}

struct rb_root{
	struct rb_node *rb_node;
};
int RB_EMPTY_ROOT(const struct rb_root* root){
	return (root->rb_node==NULL);
}
int RB_IS_NIL_NODE(const struct rb_node* node){
	return (node==nil_ptr);
}
int RB_NOT_NIL_NODE(const struct rb_node *node){
	return (node!=nil_ptr);
}
void RB_SET_NIL_NODE(const struct rb_node* node){
	(node=nil_ptr);
int RB_EMPTY_NODE(const struct rb_node* node){
	return (node==NULL);
}
void RB_CLEAR_NODE(const struct rb_node* node){
	(node=NULL);
}

#define rb_entry(ptr,type,member)	container_of(ptr,type,member)
#define rb_entry_safe(ptr,type,member) \
	({typeof(ptr) ____ptr=(ptr);\
	____ptr?rB_entry(____ptr,type,member):NULL;\
	})
#define rb_is_red(rb)     ((rb->rb_color&1UL)==rb_red<<0)
#define rb_is_black(rb)    ((rb->rb_color&1UL)==rb_black<<0)
#define rb_color(rb)	(rb->rb_color&1UL)
extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);
extern struct rb_node *rb_sibling(const struct rb_node *);
extern struct rb_node* rb_uncle(const struct rb_node* node);
extern void rb_LeftRotate(struct rb_node *x,struct rb_root* root);
extern  void rb_RightRotate(struct rb_node *x,struct rb_root* root);
extern  void __rb_change_child(struct rb_node *old, struct rb_node *new,struct rb_node *parent, struct rb_root *root);
extern  void rb_replace_node(struct rb_node *victim, struct rb_node *new,struct rb_root *root);

void rb_link_node(struct rb_node* node,struct rb_node* parent,struct rb_node** rb_link)
{
	assert(RB_EMPTY_NODE(node));
	assert(RB_EMPTY_NODE(node->rb_parent));
	node->rb_parent=parent;
	node->rb_left=node->rb_right=nil_ptr;
	*rb_link=node;
}
int rb_is_left_child(const struct rb_node *node){
	assert(RB_EMPTY_NODE(node));
	assert(RB_EMPTY_NODE(node->rb_parent));
	return (node==node->rb_parent->rb_left);
}
int rb_is_right_child(const struct rb_node *node){
	assert(RB_EMPTY_NODE(node));
	assert(RB_EMPTY_NODE(node->rb_parent));
	return (node==node->rb_parent->rb_right);
}
int rb_is_root(const struct rb_node* node,const struct rb_root* root){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_ROOT(root));
	return (node==root->rb_node);
}
void rb_set_parent(struct rb_node* node,struct rb_node* p){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(p));
	rb_parent(node)=p;
}
extern struct rb_node *rb_search(struct rb_root* root,void* value,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*));
enum INSERTFIXUPCASE{
	insertcase1,
	insertcase2,
	insertcase3,
	insertnofix,
};
extern void rb_insert(void *key,void* data,struct rb_root* root,int (*bigger_node)(struct rb_node*,struct rb_node*),struct rb_node* (*alloc)(void*,void*));
extern enum INSERTFIXUPCASE rb_fix_insert_case(struct rb_node* node,struct rb_root *root);
extern void rb_fix_insert_case1(struct rb_node* node,struct rb_root* root);
extern void rb_fix_insert_case2(struct rb_node* node,struct rb_root* root);
extern void rb_fix_insert_case3(struct rb_node* node,struct rb_root* root);
enum DELETEFIXUPCASE{
	deletecase1,
	deletecase2,
	deletecase3,
	deletecase4,
	deletecaseroot,
	deletenofix,
};
extern void rb_delete(struct rb_root* root ,void* key,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*),int(*cpy)(struct rb_node*,struct rb_node*),int (*del)(struct rb_node*));
extern enum DELETEFIXUPCASE rb_fix_delete_case(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case1(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case2(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case3(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case4(struct rb_node* node,struct rb_root* root);
#endif /* RB_H_ */
