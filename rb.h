/*
 * rb.h
 *
 *  Created on: Jun 29, 2020
 *      Author: eric
 */
#ifndef EXCEPTIONS4C
#include "e4c.h"
#endif
#ifndef _STDDEF_H_
#include <stddef.h>
#endif
#ifdef EXCEPTIONS4C
#else
#ifndef _ASSERT_H
#include <assert.h>
#endif
#endif
#ifndef RB_H_
#define RB_H_
//#define offsetof(TYPE,MEMBER)	((size_t)&((TYPE*)0)->MEMBER)
E4C_DEFINE_EXCEPTION(RootNoneBlackException, "Root node is not black violating property 2.", RuntimeException);
E4C_DEFINE_EXCEPTION(DoubleRedViolationException,"Node and at least one of its child being red simultaneously violating property 4.",RuntimeException);
E4C_DEFINE_EXCEPTION(BlackUnbalancedException,"Not each path containing the same number of black node violating property 5.",RuntimeException);
typedef struct rb_information* rb_information_t;
#define DBG(msg, arg...) printf("%s:%s(%d): " msg, __FILE__, __FUNCTION__, __LINE__, ##arg)
struct rb_information{
	int depth;
	int black_count;
	char *direction;
	rb_information_t next;
};
#define rb_red 1
#define rb_black 0
#define RB_ROOT_TAG 1<<1
#define RB_NIL_TAG 1<<2
#define container_of(ptr, type, member) ({  \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);  \
           (type *)( (char *)__mptr - offsetof(type,member) );})
#define rb_parent(r)   (r)->rb_parent
#define rb_grandparent(r) ((r)->rb_parent)->rb_parent
#define rb_is_color_red(r) (r->rb_color&((unsigned long)rb_red))
#define rb_is_color_black(r) !(r->rb_color&((unsigned long)rb_red))
#define rb_is_root_by_tag(r) ((r->rb_color|(RB_ROOT_TAG>>2))==1)
#define rb_set_root_tag(r)	(r->rb_color|(unsigned long)RB_ROOT_TAG)
#define rb_clear_root_tag(r) (r->rb_color&~((unsigned long)RB_ROOT_TAG))
#define rb_is_root_by_rb_root(node,root)	(node==root->rb_node)
struct rb_node{
	unsigned long rb_color;
	struct rb_node* rb_left;
	struct rb_node* rb_right;
	struct rb_node* rb_parent;
}__attribute__((aligned(sizeof(long))));
extern void rb_set_color_red(struct rb_node*);
extern void rb_set_color_black(struct rb_node*);
struct rb_node nil;
struct rb_node* nil_ptr;
extern void nil_initializer(struct rb_node*);
extern struct rb_root* rb_root_alloc(struct rb_node*);

struct rb_root{
	struct rb_node *rb_node;
};
extern int RB_EMPTY_ROOT(const struct rb_root*);
extern int RB_IS_NIL_NODE(const struct rb_node*);
extern int RB_NOT_NIL_NODE(const struct rb_node *);
extern void RB_SET_NIL_NODE(const struct rb_node*);
extern int RB_EMPTY_NODE(const struct rb_node*);
extern void RB_CLEAR_NODE(const struct rb_node*);

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

extern void rb_link_node(struct rb_node* node,struct rb_node* parent,struct rb_node** rb_link);

extern int rb_is_left_child(const struct rb_node *node);
extern int rb_is_right_child(const struct rb_node *node);
extern int rb_is_root(const struct rb_node* node,const struct rb_root* root);
extern void rb_set_parent(struct rb_node* node,struct rb_node* p);
extern struct rb_node *rb_search(struct rb_root* root,void* value,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*));
enum INSERTFIXUPCASE{
	insertcase1,
	insertcase2,
	insertcase3,
	insertnofix,
};
extern void rb_insert(void *key,void* data,struct rb_root* root,int (*bigger_node)(struct rb_node*,struct rb_node*),struct rb_node* (*alloc)(void*,void*));
extern void rb_fix_insert_cormen(struct rb_node* node,struct rb_root* root);
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
void rb_delete_cormen(struct rb_node* node,struct rb_root* root);
void rb_delete_fixup_cormen(struct rb_node* node,struct rb_root* root);
extern void rb_delete(struct rb_root* root ,void* key,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*),int(*cpy)(struct rb_node*,struct rb_node*),int (*del)(struct rb_node*));
extern enum DELETEFIXUPCASE rb_fix_delete_case(struct rb_node* node,struct rb_root* root);
extern void rb_transplant(struct rb_node* u,struct rb_node* v,struct rb_root* root);
extern void rb_fix_delete_case1(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case2(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case3(struct rb_node* node,struct rb_root* root);
extern void rb_fix_delete_case4(struct rb_node* node,struct rb_root* root);

extern void rb_inorder(struct rb_node* node,struct rb_root* root,void (*print_id)(struct rb_node* node));
extern void rb_predorder(struct rb_node* node,struct rb_root* root,void (*print_id)(struct rb_node* node));
extern void rb_postorder(struct rb_node*node, struct rb_root* root,void (*print_id)(struct rb_node* node));

extern int rb_maxDepth(struct rb_node* node,struct rb_root* root);
extern void rb_information_set(rb_information_t this,int depth,int black_count,char* direction,rb_information_t next);
extern int rb_information_verify_blackcount(rb_information_t head);
extern void rb_information_print(rb_information_t head);
extern void rb_information_free(rb_information_t head);
extern void rb_verify(struct rb_root* root);
extern void __rb_verify(struct rb_node* node,struct rb_root* root,int depth,int black_count,char inroute[],rb_information_t head);

#endif /* RB_H_ */
