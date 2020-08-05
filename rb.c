/*
 * rb.c
 *
 *  Created on: Jun 30, 2020
 *      Author: eric
 */
#include "rb.h"
#ifndef _STDIO_H_
#include <stdio.h>
#endif
#ifndef _STDLIB_H_
#include <stdlib.h>
#endif
#ifndef GC_H
#include "gc.h"
#endif
#ifndef _STRING_H
#include <string.h>
#endif
//whether the node is NULL

//set the node to be NULL

struct rb_root* rb_root_alloc(struct rb_node* nil_ptr){
	struct rb_root* root=(struct rb_root*)GC_malloc(sizeof(struct rb_root));
	struct rb_node* node=(struct rb_node*)GC_malloc(sizeof(struct rb_node));
	try{
		node=nil_ptr;
		root->rb_node=node;
		node->rb_color=(unsigned long)0|(RB_ROOT_TAG|rb_black);
	}
	catch(RuntimeException){
			const e4c_exception *e=e4c_get_exception();
			DBG("Problem is %s(%s)\n",e->name,e->message);
			exit(1);
	}
	return root;
}


//the first element
void rb_set_color_red(struct rb_node* r)	{
	r->rb_color=r->rb_color|1UL;
}
void rb_set_color_black(struct rb_node* r){
	r->rb_color=r->rb_color&~1UL;
}
void nil_initializer(struct rb_node* nil_ptr){
	nil_ptr->rb_color=((unsigned long)0)|(RB_NIL_TAG|rb_black);
	nil_ptr->rb_parent=nil_ptr;
	nil_ptr->rb_left=nil_ptr;
	nil_ptr->rb_right=nil_ptr;
}
int RB_EMPTY_ROOT(const struct rb_root* root){
	return (&root->rb_node==NULL);
}
int RB_IS_NIL_NODE(const struct rb_node* node){
	return (node==nil_ptr);
}
int RB_NOT_NIL_NODE(const struct rb_node *node){
	return (node!=nil_ptr);
}
void RB_SET_NIL_NODE(const struct rb_node* node){
	(node=nil_ptr);
}
int RB_EMPTY_NODE(const struct rb_node* node){
	return (node==NULL);
}
void RB_CLEAR_NODE(const struct rb_node* node){
	(node=NULL);
}
void rb_link_node(struct rb_node* node,struct rb_node* parent,struct rb_node** rb_link)
{
	assert(!RB_EMPTY_NODE(node));
	assert(!RB_EMPTY_NODE(node->rb_parent));
	node->rb_parent=parent;
	node->rb_left=node->rb_right=nil_ptr;
	*rb_link=node;
}
int rb_is_left_child(const struct rb_node *node){
	assert(!RB_EMPTY_NODE(node));
	assert(!RB_EMPTY_NODE(node->rb_parent));
	return (node==node->rb_parent->rb_left);
}
int rb_is_right_child(const struct rb_node *node){
	assert(!RB_EMPTY_NODE(node));
	assert(!RB_EMPTY_NODE(node->rb_parent));
	return (node==node->rb_parent->rb_right);
}
int rb_is_root(const struct rb_node* node,const struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_ROOT(root));
	return (node==root->rb_node);
}
void rb_set_parent(struct rb_node* node,struct rb_node* p){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(p));
	rb_parent(node)=p;
}
struct rb_node *rb_first(const struct rb_root *root){
	struct rb_node* n;
	assert(RB_EMPTY_ROOT(root));
	n=root->rb_node;
	if(!n)
		return NULL;
	while(RB_NOT_NIL_NODE(n->rb_left)){
		n=n->rb_left;
	}
	return n;
}
struct rb_node *rb_last(const struct rb_root *root){
	struct rb_node* n;
	assert(!RB_EMPTY_ROOT(root));
	n=root->rb_node;
	if(!n)
		return NULL;
	while(RB_NOT_NIL_NODE(n->rb_right)){
		n=n->rb_right;
	}
	return n;
}
//the least larger element
struct rb_node *rb_next(const struct rb_node *node){
	struct rb_node *parent;
	assert(!RB_EMPTY_NODE(node));
	if(RB_NOT_NIL_NODE(node->rb_right)){
		node=node->rb_right;
		while(RB_NOT_NIL_NODE(node->rb_left)){
			node=node->rb_left;
		}
		return (struct rb_node*) node;
	}
	while((parent=node->rb_parent)&&(node==parent->rb_right)){
		node=parent;
	}
	return parent;
}
//the most smaller element
struct rb_node *rb_prev(const struct rb_node *node){
	struct rb_node *parent;
	assert(!RB_EMPTY_NODE(node));
	if(RB_NOT_NIL_NODE(node->rb_left)){
		node=node->rb_left;
		while(RB_NOT_NIL_NODE(node->rb_right)){
			node=node->rb_right;
		}
		return (struct rb_node*) node;
	}
	while((parent=node->rb_parent)&&(node==parent->rb_left)){
		node=parent;
	}
	return parent;
}
struct rb_node* rb_sibling(const struct rb_node *node){
	if(rb_is_left_child(node))
		return node->rb_parent->rb_right;
	else if(rb_is_right_child(node))
		return node->rb_parent->rb_left;
	return NULL;//redundent
}
struct rb_node* rb_uncle(const struct rb_node* node){
	assert(!RB_EMPTY_NODE(node));
	assert(!RB_EMPTY_NODE(rb_parent(node)));
	assert(!RB_EMPTY_NODE(rb_grandparent(node)));
	return rb_sibling(node->rb_parent);
}
struct rb_node *rb_left_deepest_node(const struct rb_node *node)
{
	for(;;){
		if(RB_NOT_NIL_NODE(node->rb_left))
			node=node->rb_left;
		else if(RB_NOT_NIL_NODE(node->rb_right))
			node=node->rb_right;
		else
			return (struct rb_node*) node;
	}
	return NULL;//redundent
}
void __rb_change_child(struct rb_node *old, struct rb_node *new,
		  struct rb_node *parent, struct rb_root *root){
	assert(!RB_EMPTY_NODE(old)||!RB_EMPTY_NODE(new)||!RB_EMPTY_NODE(parent)||!RB_EMPTY_ROOT(root));
	if(RB_NOT_NIL_NODE(parent)){
		if(parent->rb_left==old)
			parent->rb_left=new;
		if(parent->rb_right==old)
			parent->rb_right=new;
	}
	else if(root->rb_node==old){
		root->rb_node=new;
	}
}
 void rb_LeftRotate(struct rb_node *node,struct rb_root* root){
	 assert(!RB_IS_NIL_NODE(node)&&!RB_EMPTY_NODE(node)&&!RB_EMPTY_ROOT(root));
	 struct rb_node* x=node;
	 struct rb_node* y=x->rb_right;
	 x->rb_right=y->rb_left;
	 if(y->rb_left!=nil_ptr){
		 rb_parent(y->rb_left)=x;
	 }
	 rb_parent(y)=rb_parent(x);
	 if(rb_parent(x)==nil_ptr){
		 root->rb_node=y;
		 rb_clear_root_tag(x); // @suppress("Statement has no effect")
		 rb_set_root_tag(y);
	 }
	 else if(rb_is_left_child(x)){
		 rb_parent(x)->rb_left=y;
	 }
	 else{
		 rb_parent(x)->rb_right=y;
	 }
	 y->rb_left=x;
	 rb_parent(x)=y;
}

 void rb_RightRotate(struct rb_node *node,struct rb_root* root){
	assert(	!RB_IS_NIL_NODE(node) && !RB_EMPTY_NODE(node)&& !RB_EMPTY_ROOT(root));
	struct rb_node *x = node;
	struct rb_node *y = x->rb_left;
	x->rb_left=y->rb_right;
	if (y->rb_right != nil_ptr) {
		rb_parent(y->rb_right) = x;
	}
	rb_parent(y) = rb_parent(x);
	if (rb_parent(x) == nil_ptr) {
		root->rb_node = y;
		rb_clear_root_tag(x);
		rb_set_root_tag(y);
	}
	if (rb_is_left_child(x)) {
		rb_parent(x)->rb_left = y;
	} else {
		rb_parent(x)->rb_right = y;
	}
	y->rb_right = x;
	rb_parent(x) = y;
}
void rb_replace_node(struct rb_node *victim, struct rb_node *new,
		     struct rb_root *root){
	struct rb_node *parent=rb_parent(victim);
	/*set the surrounding nodes to point to the replacement*/
	__rb_change_child(victim,new,parent,root);
	if(!RB_EMPTY_NODE(victim->rb_left))
		rb_set_parent(victim->rb_left,new);
	if(!RB_EMPTY_NODE(victim->rb_right))
		rb_set_parent(victim->rb_right,new);
	*new=*victim;
}
void rb_insert(void *key,void* data,struct rb_root* root,int (*bigger_node)(struct rb_node*,struct rb_node*),struct rb_node* (*alloc)(void*,void*)){
	assert(!RB_EMPTY_ROOT(root)||!RB_EMPTY_NODE(root->rb_node));
	struct rb_node *y=nil_ptr;
	struct rb_node *x=root->rb_node;
	struct rb_node *insert_node=alloc(key,data);
	while(x!=nil_ptr){
		y=x;
		if(bigger_node(insert_node,x)){
			x=x->rb_right;
		}
		else{
			x=x->rb_left;
		}
	}
	rb_parent(insert_node)=y;
	if(y==nil_ptr){
		root->rb_node=insert_node;
	}
	else if(bigger_node(insert_node,y)){
		y->rb_right=insert_node;
	}
	else{
		y->rb_left=insert_node;
	}
	insert_node->rb_left=nil_ptr;
	insert_node->rb_right=nil_ptr;
	rb_set_color_red(insert_node);
#ifdef debug_cormen
	rb_fix_insert_cormen(insert_node,root);
#else
	rb_fix_insert_case(insert_node,root);
#endif
}
void rb_fix_insert_cormen(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_ROOT(root)||!RB_IS_NIL_NODE(node));
	struct rb_node* z=node;
	while(rb_is_color_red(rb_parent(z))){
		if(rb_is_left_child(rb_parent(z))){
			struct rb_node* y=rb_uncle(z);
			if(rb_is_color_red(y)){
				rb_set_color_black(rb_parent(z));
				rb_set_color_black(y);
				rb_set_color_red(rb_grandparent(z));
				z=rb_grandparent(z);
				continue;
			}
			else if(rb_is_right_child(z)){
				z=rb_parent(z);
				rb_LeftRotate(z, root);
			}
				rb_set_color_black(rb_parent(z));
				rb_set_color_red(rb_grandparent(z));
				rb_RightRotate(rb_grandparent(z), root);
		}
		else{
			struct rb_node *y = rb_uncle(z);
			if (rb_is_color_red(y)) {
				rb_set_color_black(rb_parent(z));
				rb_set_color_black(y);
				rb_set_color_red(rb_grandparent(z));
				z = rb_grandparent(z);
				continue;
			} else if (rb_is_left_child(z)) {
				z = rb_parent(z);
				rb_RightRotate(z, root);
			}
				rb_set_color_black(rb_parent(z));
				rb_set_color_red(rb_grandparent(z));
				rb_LeftRotate(rb_grandparent(z), root);
		}
	}
	rb_set_color_black(root->rb_node);
}
enum INSERTFIXUPCASE rb_fix_insert_case(struct rb_node* node,struct rb_root *root){
	enum INSERTFIXUPCASE directive=insertnofix;
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_ROOT(root)||!RB_EMPTY_NODE(rb_sibling(node)));
	if(RB_IS_NIL_NODE(node))
		return directive=insertnofix;
	struct rb_node* uncle=rb_uncle(node);
	struct rb_node* parent=rb_parent(node);
	if(rb_is_root_by_rb_root(node,root)){
		return insertnofix;
	}
	if(uncle==NULL){
		return insertnofix;
	}

	//case 1 detection
	if(rb_is_color_red(uncle)){
		rb_fix_insert_case1(node,root);
		return directive==insertcase1;
	}
	//case 2 detection
	else if((rb_is_color_black(uncle))&&(rb_is_right_child(node))){
		rb_fix_insert_case2(node,root);
		return directive==insertcase2;
	}
	//case 3 detection
	else if((rb_is_color_black(uncle))&&(rb_is_left_child(node))){
		rb_fix_insert_case3(node,root);
		return directive==insertcase3;
	}
	return insertnofix;//redundent

}

void rb_fix_insert_case1(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_parent(node))||!RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||!RB_EMPTY_NODE(rb_grandparent(node)));
	struct rb_node* parent=rb_parent(node);
	struct rb_node* grandparent=rb_grandparent(node);
	struct rb_node* uncle =rb_sibling(parent);
	rb_set_color_black(parent);
	rb_set_color_black(uncle);
	rb_set_color_red(node);
	rb_set_color_red(grandparent);
	if(grandparent==root->rb_node){
		rb_set_color_black(grandparent);
		return;
	}
	if(rb_is_red(rb_parent(grandparent))){
		rb_fix_insert_case(grandparent,root);
	}
}
void rb_fix_insert_case2(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_parent(node))||!RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||!RB_EMPTY_NODE(rb_grandparent(node)));
	struct rb_node* parent=rb_parent(node);
	//struct rb_node* uncle=rb_sibling(node);
	//struct rb_node* grandparent=rb_grandparent(node);
	rb_LeftRotate(parent,root);
	rb_fix_insert_case3(parent,root);
}
void rb_fix_insert_case3(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_parent(node))||!RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||!RB_EMPTY_NODE(rb_grandparent(node)));
	struct rb_node* parent=rb_parent(node);
	//struct rb_node* uncle=rb_sibling(node);
	struct rb_node* grandparent=rb_grandparent(node);
	rb_set_color_black(parent);
	rb_set_color_red(grandparent);
	if(rb_is_right_child(parent)){
		rb_LeftRotate(grandparent,root);
	}
	else if(rb_is_left_child(parent)){
		rb_RightRotate(grandparent,root);
	}
}
struct rb_node *rb_search(struct rb_root* root,void* value,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*)){
	struct rb_node* current=root->rb_node;
	while (!RB_EMPTY_NODE(current)&&!equal(current,value)){
		if(bigger(current,value)){
			current=current->rb_right;
		}
		else{
			current=current->rb_left;
		}
	}
	if(RB_IS_NIL_NODE(current))
		return NULL;
	return current;
}
void rb_delete(struct rb_root* root ,void* key,int (*bigger)(struct rb_node*,void*),int (*equal)(struct rb_node*,void*),int(*cpy)(struct rb_node*,struct rb_node*),int (*del)(struct rb_node*)){
	struct rb_node* delete_node=rb_search(root,key,bigger,equal);
	if(!RB_EMPTY_NODE(delete_node))
		printf("node not found");
	struct rb_node* y=0;	//node to be delete and free
	struct rb_node* x=0;	//child of the deleted node
	if(delete_node->rb_left==nil_ptr || delete_node->rb_right==nil_ptr){
		y=delete_node;
	}
	else{
		y=rb_next(delete_node);
	}
	if(RB_NOT_NIL_NODE(y->rb_left)){
		x=y->rb_left;
	}
	else{
		x=y->rb_right;
	}
	rb_parent(x)=rb_parent(y);
	if(rb_parent(y)==nil_ptr){
		root->rb_node=y;
	}
	else if(rb_is_left_child(y)){
		rb_parent(y)->rb_left=y;
	}
	else{
		rb_parent(y)->rb_right=y;
	}
	//for case 3
	if(y!=delete_node){
		cpy(delete_node,y);
	}
	if(rb_is_black(y)){
		rb_fix_delete_case(x,root);
	}
	del(y);
}
void rb_delete_cormen(struct rb_node* node,struct rb_root* root){
	assert(!RB_IS_NIL_NODE(node)&&!RB_EMPTY_NODE(node)&&RB_EMPTY_ROOT(root));
	struct rb_node* y=node;
	unsigned long y_orig_color=y->rb_color;
	if(node->rb_left==nil_ptr){
		struct rb_node* x=node->rb_right;
		rb_transplant(node,node->rb_right,root);
	}
	else if(node->rb_right==nil_ptr){
		struct rb_node* x=node->rb_left;
		rb_transplant(node,node->rb_left,root);
	}
	else{
		y=rb_next(node->rb_right);
		y_orig_color=y->rb_color;
		struct rb_node* x=y->rb_right;

		if(rb_parent(y)==node){
			rb_parent(x)=y;
		}

		else{
			rb_transplant(y,y->rb_right,root);
			y->rb_right=node->rb_right;
			rb_parent(y->rb_right)=y;
		}
		rb_transplant(node,y,root);
		y->rb_left=node->rb_left;
		rb_parent(y->rb_left)=y;
		y->rb_color=node->rb_color;

		if(y_orig_color&&(unsigned long)rb_black){
			rb_delete_fixup_cormen(x,root);
		}
	}
}
void rb_delete_fixup_cormen(struct rb_node* node,struct rb_root* root){
	assert(!RB_IS_NIL_NODE(node)&&!RB_EMPTY_NODE(node)&&RB_EMPTY_ROOT(root));
	struct rb_node* z=node;
	while(rb_is_color_red(rb_parent(z))){
		if(rb_is_left_child(rb_parent(z))){
			struct rb_node* y=rb_sibling(rb_parent(z));
			if(rb_is_color_red(y)){
				rb_set_color_black(rb_parent(z));
				rb_set_color_black(y);
				rb_set_color_red(rb_grandparent(z));
				z=rb_grandparent(z);
			}
			else if(rb_is_right_child(z)){
				z=rb_parent(z);
				rb_LeftRotate(z, root);
			}
			rb_set_color_black(rb_parent(z));
			rb_set_color_red(rb_grandparent(z));
			rb_RightRotate(rb_parent(z), root);
		}
		else{
			struct rb_node* y=rb_sibling(rb_parent(z));
			if(rb_is_color_red(y)){
				rb_set_color_black(rb_parent(z));
				rb_set_color_black(y);
				rb_set_color_red(rb_grandparent(z));
				z=rb_grandparent(z);
			}
			else if(rb_is_right_child(z)){
				z=rb_parent(z);
				rb_LeftRotate(z,root);
			}
			rb_set_color_black(rb_parent(z));
			rb_set_color_red(rb_grandparent(z));
			rb_LeftRotate(rb_parent(z), root);
		}
	}
	rb_set_color_black(root->rb_node);
}
void rb_transplant(struct rb_node* u,struct rb_node* v,struct rb_root* root)
{
	if(rb_is_root_by_rb_root(u,root)){
		root->rb_node=v;
	}
	else if(rb_is_left_child(u)){
		rb_parent(u)->rb_left=v;
	}
	else if(rb_is_right_child(u)){
		rb_parent(u)->rb_right=v;
	}
}
enum DELETEFIXUPCASE rb_fix_delete_case(struct rb_node* node,struct rb_root* root){
	enum DELETEFIXUPCASE directive;
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_sibling(node))||!RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);

	if(rb_is_red(sibling)){
		rb_fix_delete_case1(node,root);
		return directive=deletecase1;
	}
	if(rb_is_black(sibling)&&rb_is_black(sibling->rb_left)&&rb_is_black(sibling->rb_right)){
		rb_fix_delete_case2(node,root);
		return directive=deletecase2;
	}
	if(rb_is_black(sibling)&&rb_is_red(sibling->rb_left)&&rb_is_black(sibling->rb_right)){
		rb_fix_delete_case3(node,root);
		return directive=deletecase3;
	}
	if(rb_is_black(sibling)&&rb_is_black(sibling->rb_left)&&rb_is_red(sibling->rb_right)){
		rb_fix_delete_case4(node,root);
		return directive=deletecase4;
	}
	return deletenofix;//redundent

}
void rb_fix_delete_case1(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_sibling(node))||!RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	struct rb_node* parent=rb_parent(node);
	rb_set_color_red(parent);
	rb_set_color_black(sibling);

	rb_LeftRotate(parent,root);
	rb_fix_delete_case(node,root);
}
void rb_fix_delete_case2(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_sibling(node))||!RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	struct rb_node* parent=rb_parent(node);
	rb_set_color_red(sibling);
	if(rb_is_red(parent)){
		rb_set_color_black(parent);
	}
	else{
		if(rb_is_root_by_rb_root(parent,root)){
		}else{
			rb_fix_delete_case(parent,root);
		}

	}
}
void rb_fix_delete_case3(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_sibling(node))||!RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	rb_set_color_black(sibling->rb_left);
	rb_set_color_red(sibling);
	rb_RightRotate(sibling,root);
	rb_fix_delete_case(node,root);
}
void rb_fix_delete_case4(struct rb_node* node,struct rb_root* root){
	assert(!RB_EMPTY_NODE(node)||!RB_EMPTY_NODE(rb_sibling(node))||!RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	struct rb_node* parent=rb_parent(node);
	sibling->rb_color=parent->rb_color;
	rb_set_color_black(parent);
	rb_set_color_black(sibling->rb_right);
	rb_LeftRotate(parent,root);
	rb_set_color_black(root->rb_node);
}
void rb_preorder(struct rb_node* node,struct rb_root* root,void (*print_id)(struct rb_node* node))
{
	if(node==NULL){
		struct rb_node* tmp=root->rb_node;
		rb_preorder(tmp,root,print_id);
		return;
	}
	if(RB_NOT_NIL_NODE(node)){
		print_id(node);
		rb_preorder(node->rb_left,root,print_id);
		rb_preorder(node->rb_right,root,print_id);
	}
}
void rb_inorder(struct rb_node* node,struct rb_root* root,void (*print_id)(struct rb_node* node))
{
	if(node==NULL){
		struct rb_node* tmp=root->rb_node;
		rb_inorder(tmp,root,print_id);
		return;
	}
	if(RB_NOT_NIL_NODE(node)){
		rb_inorder(node->rb_left,root,print_id);
		print_id(node);
		rb_inorder(node->rb_right,root,print_id);
	}
}
void rb_postorder(struct rb_node* node,struct rb_root* root,void (*print_id)(struct rb_node* node))
{

	if(node==NULL){
		struct rb_node* tmp=root->rb_node;
		rb_postorder(tmp,root,print_id);
		return;
	}
	if(RB_NOT_NIL_NODE(node)){
		rb_postorder(node->rb_left,root,print_id);
		print_id(node);
		rb_postorder(node->rb_right,root,print_id);
	}
}
int rb_maxDepth(struct rb_node* node,struct rb_root* root)
{
	if(node==NULL){
		node=root->rb_node;
	}
	if (node==nil_ptr){
		return 0;
	}
	else
	{
       /* compute the depth of each subtree */
		int lDepth = rb_maxDepth(node->rb_left,root);
		int rDepth = rb_maxDepth(node->rb_right,root);

       /* use the larger one */
		if (lDepth > rDepth)
			return(lDepth+1);
		else return(rDepth+1);
	}
}
void rb_information_set(rb_information_t this,int depth,int black_count,char* direction,rb_information_t next){
	this->black_count=black_count;
	this->depth=depth;
	this->direction=direction;
	this->next=next;
}
int rb_information_verify_blackcount(rb_information_t head){
	assert(head!=NULL);
	int firstcount=head->black_count;
	rb_information_t tmp;
	for(tmp=head;tmp!=NULL;tmp=tmp->next){
		if(tmp->black_count!=firstcount){
			return 0;
		}
	}
	return 1;
}
void rb_information_print(rb_information_t head){
	assert(head!=NULL);
	rb_information_t tmp;
	for(tmp=head;tmp!=NULL;tmp=tmp->next){
		DBG("Direction:%s,black count:%d\n",tmp->direction,tmp->black_count);
	}
}
void rb_information_free(rb_information_t head){
	assert(head!=NULL);
	rb_information_t tmp;
	tmp=head;
	while(head!=NULL){
		tmp=head;
		head=head->next;
		GC_FREE(tmp->direction);
		GC_FREE(tmp);
	}
}

void rb_verify(struct rb_root* root){



	int max_depth=rb_maxDepth(NULL,root);
	rb_information_t head=GC_MALLOC(sizeof(struct rb_information));
	int depth=0;
	int black_count=0;
	char *route=GC_MALLOC(sizeof(char)*(max_depth+1));
	memset(route,'\0',sizeof(char)*(max_depth+1));
	rb_information_set(head,depth,black_count,route,NULL);
	struct rb_node* node=root->rb_node;
	try{
		if(node==NULL){
			DBG("Tree is empty.Be careful next time.");
		}
		if(rb_is_color_red(node)) throw(RootNoneBlackException,NULL);
		__rb_verify(node,root,depth,black_count,route,head);
		if(!rb_information_verify_blackcount(head)) throw(BlackUnbalancedException,NULL);
		rb_information_free(head);
		return;
	}
	catch(RootNoneBlackException){
		const e4c_exception *e=e4c_get_exception();
		DBG("%s:%s\n",e->name,e->message);
		rb_information_free(head);
		return;
	}
	catch(DoubleRedViolationException){
		const e4c_exception *e=e4c_get_exception();
		DBG("%s:%s\n",e->name,e->message);
		rb_information_free(head);
		return;
	}
	catch(BlackUnbalancedException){
		const e4c_exception *e=e4c_get_exception();
		DBG("%s:%s\n",e->name,e->message);
		DBG("Print information of every path\n");
		rb_information_print(head);
		rb_information_free(head);
		return;
	}
	catch(BadPointerException){
		const e4c_exception *e=e4c_get_exception();
		DBG("%s:%s --  Perhaps there are inaccessible memory or trying to dereference null pointer \n",e->name,e->message);
		return;
	}
}
void __rb_verify(struct rb_node* node,struct rb_root* root,int depth,int black_count,char inroute[],rb_information_t head){
	if(RB_IS_NIL_NODE(node)){
				return;
	}
	depth++;

	if(node!=root->rb_node){
		if(rb_is_left_child(node)){
			strcat(inroute,"L");
		}
		else{
			strcat(inroute,"R");
		}
	}

	if(rb_is_color_black(node)){
		black_count++;
	}
	if((rb_is_color_red(node))&&(rb_is_color_red(node->rb_left)||rb_is_color_red(node->rb_right))){
		throw(DoubleRedViolationException,NULL);
	}
	if(RB_IS_NIL_NODE(node->rb_left)&&RB_IS_NIL_NODE(node->rb_right)){
		rb_information_t tail;
		for(tail=head;tail->next!=NULL;tail=tail->next){}
		if(tail->depth==0){
			char* direction=GC_MALLOC(sizeof(char)*rb_maxDepth(NULL,root));
			strcpy(direction,inroute);
			rb_information_set(head,depth,black_count,direction,NULL);
		}
		else{
			rb_information_t new=GC_MALLOC(sizeof(struct rb_information));
			char* direction=GC_MALLOC(sizeof(char)*rb_maxDepth(NULL,root));
			strcpy(direction,inroute);
			rb_information_set(new,depth,black_count,direction,NULL);
			tail->next=new;
		}
		inroute[strlen(inroute)-1]='\0';
		return;
	}
	if(!RB_IS_NIL_NODE(node->rb_left)){
	__rb_verify(node->rb_left,root,depth,black_count,inroute,head);
	}
	if(!RB_IS_NIL_NODE(node->rb_right)){
	__rb_verify(node->rb_right,root,depth,black_count,inroute,head);
	}
	inroute[strlen(inroute)-1]='\0';
	return;

}
