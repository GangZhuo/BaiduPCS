#ifdef DMALLOC
#include <dmalloc.h>
#endif
#include"misc.h"
#include"stack.h"

/* Source: http://web.mit.edu/~emin/www.old/source_code/red_black_tree/index.html */

/*  CONVENTIONS:  All data structures for red-black trees have the prefix */
/*                "rb_" to prevent name conflicts. */
/*                                                                      */
/*                Function names: Each word in a function name begins with */
/*                a capital letter.  An example funcntion name is  */
/*                CreateRedTree(a,b,c). Furthermore, each function name */
/*                should begin with a capital letter to easily distinguish */
/*                them from variables. */
/*                                                                     */
/*                Variable names: Each word in a variable name begins with */
/*                a capital letter EXCEPT the first letter of the variable */
/*                name.  For example, int newLongInt.  Global variables have */
/*                names beginning with "g".  An example of a global */
/*                variable name is gNewtonsConstant. */

/* comment out the line below to remove all the debugging assertion */
/* checks from the compiled code.  */
#define DEBUG_ASSERT 1

typedef struct rb_red_blk_node {
  void* key;
  void* info;
  int red; /* if red=0 then the node is black */
  struct rb_red_blk_node* left;
  struct rb_red_blk_node* right;
  struct rb_red_blk_node* parent;
} rb_red_blk_node;


/* Compare(a,b) should return 1 if *a > *b, -1 if *a < *b, and 0 otherwise */
/* Destroy(a) takes a pointer to whatever key might be and frees it accordingly */
typedef struct rb_red_blk_tree {
  int  (*Compare)(const void* a, const void* b, void *state); 
  void (*DestroyKey)(void* a, void *state);
  void (*DestroyInfo)(void* a, void *state);
  void (*PrintKey)(const void* a, void *state);
  void (*PrintInfo)(void* a, void *state);
  int  (*EnumerateInfo)(void*, void*); /*返回0表示继续执行，返回非0值表示中断执行。*/
  void *compareState;
  void *destroyKeyState;
  void *destroyInfoState;
  void *printKeyState;
  void *printInfoState;
  void *enumerateInfoState;
  /*  A sentinel is used for root and for nil.  These sentinels are */
  /*  created when RBTreeCreate is caled.  root->left should always */
  /*  point to the node which is the root of the tree.  nil points to a */
  /*  node which should always be black but has aribtrary children and */
  /*  parent and no key or info.  The point of using these sentinels is so */
  /*  that the root and nil nodes do not require special cases in the code */
  rb_red_blk_node* root;             
  rb_red_blk_node* nil;              
} rb_red_blk_tree;

rb_red_blk_tree* RBTreeCreate(int (*CompFunc)(const void*, const void*, void*),
	void (*DestFunc)(void*, void*),
	void (*InfoDestFunc)(void*, void*),
	void (*PrintFunc)(const void*, void*),
	void (*PrintInfo)(void*, void*));
rb_red_blk_node * RBTreeInsert(rb_red_blk_tree*, void* key, void* info);
void RBTreePrint(rb_red_blk_tree*);
int  RBTreeEnumerateInfo(rb_red_blk_tree* tree);
void RBDelete(rb_red_blk_tree* , rb_red_blk_node* );
void RBTreeDestroy(rb_red_blk_tree*);
rb_red_blk_node* TreePredecessor(rb_red_blk_tree*,rb_red_blk_node*);
rb_red_blk_node* TreeSuccessor(rb_red_blk_tree*,rb_red_blk_node*);
rb_red_blk_node* RBExactQuery(rb_red_blk_tree*, void*);
stk_stack * RBEnumerate(rb_red_blk_tree* tree,void* low, void* high);
void NullFunction(void*);
