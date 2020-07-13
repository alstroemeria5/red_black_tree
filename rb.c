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
//whether the node is NULL

//set the node to be NULL




//the first element
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
	assert(RB_EMPTY_ROOT(root));
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
	assert(RB_EMPTY_NODE(node));
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
	assert(RB_EMPTY_NODE(node));
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
	assert(RB_EMPTY_NODE(node));
	assert(RB_EMPTY_NODE(rb_parent(node)));
	assert(RB_EMPTY_NODE(rb_grandparent(node)));
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
	assert(RB_EMPTY_NODE(old)||RB_EMPTY_NODE(new)||RB_EMPTY_NODE(parent)||RB_EMPTY_ROOT(root));
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
 void rb_LeftRotate(struct rb_node *x,struct rb_root* root){
	struct rb_node * y=x->rb_right;
	x->rb_right =y->rb_left;
	if(RB_NOT_NIL_NODE(x->rb_right))
		rb_parent(x->rb_right)=x;

	rb_parent(y)=rb_parent(x);

	if(RB_IS_NIL_NODE(rb_parent(x))){
		root->rb_node=y;
		root->rb_node->rb_color|=RB_ROOT_TAG;
	}
	if(rb_is_left_child(x))
		y=rb_parent(x)->rb_left;
	else
		y=rb_parent(x)->rb_right;

	y->rb_left=x;
	rb_parent(x)=y;
}

 void rb_RightRotate(struct rb_node *x,struct rb_root* root){
	struct rb_node * y=x->rb_left;
		x->rb_left =y->rb_right;
		if(RB_NOT_NIL_NODE(x->rb_left))
			rb_parent(x->rb_left)=x;

		rb_parent(y)=rb_parent(x);

		if(RB_IS_NIL_NODE(rb_parent(x))){
			root->rb_node=y;
			root->rb_node->rb_color|=RB_ROOT_TAG;
		}
		if(rb_is_left_child(x))
			y=rb_parent(x)->rb_left;
		else
			y=rb_parent(x)->rb_right;

		y->rb_right=x;
		rb_parent(x)=y;
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
	assert(RB_EMPTY_ROOT(root)||RB_EMPTY_NODE(root->rb_node));
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
		root->rb_node=x;
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
	rb_fix_insert_case(insert_node,root);
}
enum INSERTFIXUPCASE rb_fix_insert_case(struct rb_node* node,struct rb_root *root){
	enum INSERTFIXUPCASE directive=insertnofix;
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_ROOT(root)||RB_EMPTY_NODE(rb_sibling(node)));
	if(RB_IS_NIL_NODE(node))
		return directive=insertnofix;
	struct rb_node* uncle=rb_uncle(node);
	//case 1 detection
	if((uncle->rb_color&rb_red)){
		rb_fix_insert_case1(node,root);
		return directive==insertcase1;
	}
	//case 2 detection
	if((uncle->rb_color&rb_black)&&(rb_is_right_child(node))){
		rb_fix_insert_case2(node,root);
		return directive==insertcase2;
	}
	//case 3 detection
	if((uncle->rb_color&rb_black)&&(rb_is_left_child(node))){
		rb_fix_insert_case3(node,root);
		return directive==insertcase3;
	}
	return insertnofix;//redundent

}

void rb_fix_insert_case1(struct rb_node* node,struct rb_root* root){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_parent(node))||RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||rb_grandparent(node));
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
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_parent(node))||RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||rb_grandparent(node));
	struct rb_node* parent=rb_parent(node);
	//struct rb_node* uncle=rb_sibling(node);
	//struct rb_node* grandparent=rb_grandparent(node);
	rb_LeftRotate(node,root);
	rb_fix_insert_case3(parent,root);
}
void rb_fix_insert_case3(struct rb_node* node,struct rb_root* root){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_parent(node))||RB_EMPTY_NODE(rb_sibling(rb_parent(node)))||rb_grandparent(node));
	struct rb_node* parent=rb_parent(node);
	//struct rb_node* uncle=rb_sibling(node);
	struct rb_node* grandparent=rb_grandparent(node);
	rb_set_color_black(parent);
	rb_set_color_red(grandparent);
	rb_RightRotate(parent,root);
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
	if(RB_EMPTY_NODE(delete_node))
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
enum DELETEFIXUPCASE rb_fix_delete_case(struct rb_node* node,struct rb_root* root){
	enum DELETEFIXUPCASE directive;
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_sibling(node))||RB_EMPTY_NODE(rb_parent(node)));
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
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_sibling(node))||RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	struct rb_node* parent=rb_parent(node);
	rb_set_color_red(parent);
	rb_set_color_black(sibling);

	rb_LeftRotate(parent,root);
	rb_fix_delete_case(node,root);
}
void rb_fix_delete_case2(struct rb_node* node,struct rb_root* root){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_sibling(node))||RB_EMPTY_NODE(rb_parent(node)));
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
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_sibling(node))||RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	rb_set_color_black(sibling->rb_left);
	rb_set_color_red(sibling);
	rb_RightRotate(sibling,root);
	rb_fix_delete_case(node,root);
}
void rb_fix_delete_case4(struct rb_node* node,struct rb_root* root){
	assert(RB_EMPTY_NODE(node)||RB_EMPTY_NODE(rb_sibling(node))||RB_EMPTY_NODE(rb_parent(node)));
	struct rb_node* sibling=rb_sibling(node);
	struct rb_node* parent=rb_parent(node);
	sibling->rb_color=parent->rb_color;
	rb_set_color_black(parent);
	rb_set_color_black(sibling->rb_right);
	rb_LeftRotate(parent,root);
	rb_set_color_black(root->rb_node);
}
