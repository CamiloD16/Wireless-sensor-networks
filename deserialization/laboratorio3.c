#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MARKER ')'
#define N 5

struct node {
    int id;
    struct node *sibling;
    struct node *child;
    struct node *father; 
};

typedef struct node node;

node * new_node(int);
node * add_sibling(node *, node *);
node * add_child(node *, node *);
node * search_forwarder(node *, node *, int);
void serializar(node * root, FILE *file);
node * deserializar(node * root, FILE *file);
void aux1(node * root, FILE *file);
void aux2(node * root, FILE *file);
void aux3(node * root, FILE *file);
void buscar_id(node * n, int id);

unsigned char id_father = 0;

node * new_node(int id){
    node *new_node = malloc(sizeof(node));

    if ( new_node ) {
        new_node->sibling = NULL; 
        new_node->child = NULL;
        new_node->father = NULL; 
        new_node->id = id; 
    }

    return new_node;
}


node * add_sibling(node * n, node *sibling){
    if ( n == NULL ) return NULL;
	
    if(sibling){
		while (n->sibling)
        n = n->sibling;
    	printf("nodo %d hermano de nodo %d\n",n->id, sibling->id);
    	n->sibling = sibling;
        n->sibling->father =n->father;  
        return (n->sibling); 
    }
}


node * add_child(node *n, node *child){
    if ( n == NULL ) return NULL;

	if(child){
		printf("nodo %d padre de nodo %d\n",n->id, child->id);

	    if (n->child) return add_sibling(n->child, child);
	    else{
            n->child = child; 
            n->child->father = n; 
            return  (n->child); 
        } 

    }else return NULL;

}
void serializar(node * root, FILE *file) {
    if (root == NULL) return;
 
    fprintf(file, "%c ", root->id);
   	if (root->child) {
        serializar(root->child, file);
    }

    fprintf(file, "%c ", MARKER);
}
node * deserializar(node * root, FILE *file) {
    char val[3];
    if ( !fscanf(file, "%s ", &val) || atoi(val) == 0 )
       return NULL;
    root = new_node(atoi(val));
    aux2(root, file);
    return root;
}
void aux1(node * root, FILE *file) {
	if(root == NULL) {
		return;
	}
    char aux[3];
    fprintf(file, "%d ", root->id);
    aux1(root->child, file);
    fprintf(file, "%s ", MARKER);
    aux1(root->sibling, file);
}

void aux2(node * root, FILE *file) {
    aux3(root, file);
}

void aux3(node * root, FILE *file) {
    char val[3];
    if ( !fscanf(file, "%s ", &val) || atoi(val) == 0 )
        return;
    node * aux;
    aux = new_node(atoi(val));
    add_child(root, aux);
    aux3(aux, file);
    
    if ( !fscanf(file, "%s ", &val) || atoi(val) == 0 )
        return;
    node * aux1;
    aux1 = new_node(atoi(val));
    add_child(root, aux1);
    aux3(aux1, file);

    aux2(root, file); 
}

node * search_forwarder(node *n, node *transmisor,int searchNum){

	if(n == NULL) return NULL;
	
    printf("Buscando en %d\n",n->id);
    
	if(n->id == searchNum){
        printf("Nodo encontrado..\r\n");
        for (;n->father && n->father->id != transmisor->id;n=n->father); 
        id_father = n->id; 
	}
	else {
		node *searching = search_forwarder(n->child,transmisor,searchNum);
		if(searching==NULL){
			search_forwarder(n->sibling,transmisor,searchNum);	
		}
    }
}

void print_node_decendents(struct node* root){
    if(root){
	    printf("->%d\n",root->id);  
	    print_node_decendents(root->child);
	    print_node_decendents(root->sibling);
    }
}
void information(struct node* root){
	if(root){
		printf("%d ",root->id);  
		information(root->child);
        printf(") ");
        information(root->sibling);
        
	}
}
int main(int argc, char *argv[])
{       
    node *n1,*n2,*n3,*n4,*n5,*n6,*n7, *n8, *n9, *n10, *n11;
    n1 = new_node(1);
    n2 = new_node(2);
    n3 = new_node(3);
    n4 = new_node(4);
    n5 = new_node(5);
    n6 = new_node(6);
    n7 = new_node(7);
    n8 = new_node(8);
    n9 = new_node(9);
    n10 = new_node(10);
    n11 = new_node(11);
    add_child(n1, n2);
    add_child(n1, n3);
    add_child(n1, n4);
    add_child(n2, n5);
    add_child(n2, n6);
    add_child(n3, n7);
    add_child(n4, n8);
    add_child(n4, n9);
    add_child(n5, n10);
    add_child(n8, n11);

    /*
           n1 
        	|   
           n2---------n3---n4 
         	|         |  	|
        	n5--n6    n7   	n8--n9
        	|            	| 
       		n10           n11   
    
    */

    print_node_decendents(n1); 
	
    printf("\n");
    search_forwarder(n1,n1,9);
	printf("\nNodo retransmisor: %d\n\n",id_father);	
	
    FILE *file = fopen("tree.txt", "w");
    serializar(n1, file);
    fclose(file);    

    node *nodo;
    file = fopen("tree.txt", "r");
    nodo = deserializar(nodo, file);
    fclose(file);
    print_node_decendents(nodo);
   	information(n1);

    return 0;
}
