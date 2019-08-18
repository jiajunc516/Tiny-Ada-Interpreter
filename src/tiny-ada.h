/*
ICS 142A Homework 9
Name: JIAJUN CHANG
ID: 73041847
*/
#include <string.h>
#include <stdlib.h>
#include <string>

#define AND 	258
#define NOT 	259
#define ASSIGN 	263
#define TRUE 	1
#define FALSE 	0
#define UNINITIALIZED -1

typedef int token;
typedef int integer;

enum type_kind {undefined_type, integer_type, boolean_type};

typedef struct symbol_block* symbol;

struct symbol_block {
    char * name;
    type_kind type;
    integer value;

    symbol_block(char * n, type_kind t, integer v) :
        name(n), type(t), value(v)
    {
    }
};

typedef struct table_block* table;
struct table_block {
    symbol info;
    table tail;

    table_block(symbol h, table t) :
        info(h), tail(t)
    {
    }

    table_block(table t) { // scope marker
    	info = 0;
    	tail = t;
    }

};

// ***<<< LEFT FOR YOU TO DEFINE >>>***
// ***<<< type symtab is linked_stack of table >>>***

class symbol_table {
    table head;
public:
    void enter_symbol (const char *name, symbol h) {
    	char * nName = strdup(name);
    	bool push = true;
    	table temp = head;
    	while (temp != 0)
    	{
    		if (temp->info == 0)
    			break;
    		else
    		{
    			if (!strcmp(temp->info->name, nName))
    			{
    				temp->info->type = h->type;
    				temp->info->value = h->value;
    				push = false;
    				break;
    			}
    		}
    		temp = temp->tail;
    	}
    	if (push)
    		head = new table_block(new symbol_block(nName, h->type, UNINITIALIZED), head);
    }
    symbol find_symbol (char * name) {
    	for (table temp = head; temp; temp = temp->tail)
    	{
    		if (temp->info != 0)
    			if (!strcmp(temp->info->name, name))
    				return temp->info;
    	}
    	return new symbol_block(name, undefined_type, UNINITIALIZED);
    }
    void enter_scope() {
    	head = new table_block(head);
    }
    void exit_scope() {
    	while (head->info != 0)
    	{
    		head = head->tail;
    	}
    	if (head != 0)
    	{
    		head = head->tail;
    	}
    }
    symbol_table() { // constructor for a table
        head = 0;
        enter_symbol("integer", new symbol_block(strdup("integer"), integer_type, UNINITIALIZED));
        enter_symbol("boolean", new symbol_block(strdup("boolean"), boolean_type, UNINITIALIZED));
        enter_symbol("true", new symbol_block(strdup("true"), boolean_type, TRUE));
        enter_symbol("false", new symbol_block(strdup("false"), boolean_type, FALSE));
    }
};

const char* toString(type_kind ty) {
	if (ty == integer_type)
		return "integer";
	else if (ty == boolean_type)
		return "boolean";
	else
		return "undefined_type";
}

type_kind type_check(token op, type_kind left, type_kind right) {
	switch (op)
	{
		case '<':
			if (left == right && left != undefined_type)
				return boolean_type;
			cout << "ERROR: Cannot compare " << toString(left) << " with " << toString(right) << endl;
			exit(EXIT_FAILURE);
			break;
		case AND: // And
			if (left == boolean_type && right == boolean_type)
				return boolean_type;
			cout << "ERROR: Cannot do " << toString(left) << " And " << toString(right) << endl;
			exit(EXIT_FAILURE);
			break;
		case NOT: // Not
			if (right == boolean_type)
				return boolean_type;
			cout << "ERROR: Cannot do " << " Not " << toString(right) << endl;
			exit(EXIT_FAILURE);
			break;
		case '+':
		case '*':
			if (left == integer_type && right == integer_type)
				return integer_type;
			cout << "ERROR: Cannot compute" << toString(left) << " with " << toString(right) << endl;
			exit(EXIT_FAILURE);
			break;
		case ASSIGN: // Assign
			if (left == right && left != undefined_type)
				return left;
			cout << "ERROR: Cannot assign " << toString(right) << " to " << toString(left) << endl;
			exit(EXIT_FAILURE);
			break;
		default:
			cout << "ERROR: Undefined operator" << endl;
			exit(EXIT_FAILURE);
			break;
	}
}

symbol_table symtab;

// ***<<< Node >>>***

typedef struct eval_result_block* eval_result;
struct eval_result_block {
    type_kind type;
    integer value;

    eval_result_block(type_kind t, integer v) :
        type(t), value(v)
    {
    }

};

typedef struct node_block* node;

struct node_block {

    virtual eval_result eval() {
        cerr << "Error: You forgot to implement eval for this node type\n";
    }
    virtual void pprint(int indent = 0) {
        cerr << "Error: You forgot to implement pprint for this node type\n";
    }
    virtual void enter(type_kind ty, integer v) {
    	cerr << "Error: You forgot to implement enter for this node type\n";
    }

};

typedef struct node_pair* node_list;

struct node_pair {

    node head;
    node_list tail;
    
    node_pair(node h, node_list t) :
        head(h), tail(t)
    {
    }

    eval_result eval()
    {
    	return head->eval();
    }
    void pprint(int indent)
    {
    	head->pprint(indent);
    }
};

struct paren_node : node_block {

	node child;
	paren_node(node c) :
		child(c)
	{
	}

	virtual eval_result eval()
	{
		return child->eval();
	}
	virtual void pprint(int indent)
	{
		cout << "(";
		child->pprint(indent);
		cout << ")";
	}
};

struct binary_node : node_block {

    token bin_op;
    node left, right;

    binary_node(token b_op, node l, node r) :
        bin_op(b_op), left(l), right(r)
    {
    }

    virtual eval_result eval() 
    {
    	eval_result l = left->eval();
    	eval_result r = right->eval();
    	type_kind ty = type_check(bin_op, l->type, r->type);
    	if (l->value == UNINITIALIZED || r->value == UNINITIALIZED)
    	{
    		cout << "ERROR: Cannot compute uninitialized varible" << endl;
    		exit(EXIT_FAILURE);
    	}

    	integer rval;
    	switch (bin_op)
    	{
    		case '<':
    			if (l->value < r->value)
    				rval = TRUE;
    			else
    				rval = FALSE;
    			break;
    		case AND:
    			if (l->value == TRUE && r->value == TRUE)
    				rval = TRUE;
    			else
    				rval = FALSE;
    			break;
    		case '+':
    			rval = l->value + r->value;
    			break;
    		case '*':
    			rval = l->value * r->value;
    			break;
    		default:
    			rval = UNINITIALIZED;
    			break;
    	}
    	return new eval_result_block(ty, rval);
    }
    virtual void pprint(int indent) 
    {
    	left->pprint(indent);
    	switch (bin_op)
    	{
    		case '<':
    			cout << " < ";
    			break;
    		case AND:
    			cout << " and ";
    			break;
    		case '+':
    			cout << " + ";
    			break;
    		case '*':
    			cout << " * ";
    			break;
    		default:
    			break;
    	}
    	right->pprint(indent);
    }
};

struct unary_node : node_block {

    token un_op;
    node operand;

    unary_node(token u_op, node op) :
        un_op(u_op), operand(op)
    {
    }

    virtual eval_result eval() 
    {
    	eval_result r = operand->eval();
    	type_kind ty = type_check(un_op, r->type, r->type);
    	if (r->value == UNINITIALIZED)
    	{
    		cout << "ERROR: Cannot compute uninitialized varible" << endl;
    		exit(EXIT_FAILURE);
    	}

    	integer rval;
    	if (r->value == TRUE)
    		rval = FALSE;
    	else if (r->value == FALSE)
    		rval = TRUE;
    	return new eval_result_block(ty, rval);
    }
    virtual void pprint(int indent) 
    {
    	cout << "not ";
    	operand->pprint(indent);
    }

};

struct integer_node : node_block {
    
    int value;

    integer_node(int v) :
        value(v)
    {
    }

    virtual eval_result eval() 
    {
    	return new eval_result_block(integer_type, value);
    }
    virtual void pprint(int indent) 
    {
    	cout << value;
    }

};

struct ident_node : node_block {

    char * id_name;

    ident_node(char * n) :
        id_name(n)
    {
    }

    virtual eval_result eval() 
    {
    	symbol h = symtab.find_symbol(id_name);
    	return new eval_result_block(h->type, h->value);
    }
    virtual void pprint(int indent) 
    {
    	cout << id_name;
    }
    virtual void enter(type_kind ty, integer v)
    {
    	//cout << "Entering " << id_name << ": " << v << endl;
    	symtab.enter_symbol(id_name, new symbol_block(id_name, ty, v));
    }

};

struct assign_node : node_block {

    char * dest;
    node src;

    assign_node(char * d, node s) :
        dest(d), src(s)
    {
    }

    virtual eval_result eval() 
    {
    	eval_result result = src->eval();
    	type_kind ty = type_check(ASSIGN, symtab.find_symbol(dest)->type, result->type);
    	if (ty == integer_type && result->value == UNINITIALIZED)
    	{
    		cout << "ERROR: Cannot assign uninitialized varible" << endl;
    		exit(EXIT_FAILURE);
    	}

    	symtab.enter_symbol(dest, new symbol_block(dest, ty, result->value));
    	return result;
    }
    virtual void pprint(int indent) 
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	cout << ws << dest << " := ";
    	src->pprint(indent);
    	cout << ";" << endl;
    }

};

struct var_decl_node : node_block {
    char * name;
    char * type;

    var_decl_node(char * n, char * t) :
        name(n), type(t)
    {
    }

    virtual eval_result eval() 
    {
    	symtab.enter_symbol(name, symtab.find_symbol(type));
    }
    virtual void pprint(int indent) 
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	cout << ws << name << ": " << type << ";" << endl;
    }
};

struct declare_node : node_block {
    node_list decls;
    node_list stmts;

    declare_node(node_list d, node_list s) :
        decls(d), stmts(s)
    {
    }

    virtual eval_result eval() 
    {
    	symtab.enter_scope();
    	for (node_list temp = decls; temp; temp = temp->tail)
    		temp->eval();
    	for (node_list temp = stmts; temp; temp = temp->tail)
    		temp->eval();
    	symtab.exit_scope();
    }
    virtual void pprint(int indent) 
    {
    	indent++;
    	string ws;
    	for (int i = 0; i < indent-1; i++)
    		ws += "    ";

    	cout << ws << "declare" << endl;
    	for (node_list temp = decls; temp; temp = temp->tail)
    		temp->pprint(indent);

    	cout << ws << "begin" << endl;
    	for (node_list temp = stmts; temp; temp = temp->tail)
    		temp->pprint(indent);

    	cout << ws << "end;" << endl;
    }
};

struct if_node : node_block {
    node if_cond;
    node_list if_stmts;
    node if_next;

    if_node(node cond, node_list s, node n):
        if_cond(cond), if_stmts(s), if_next(n)
    {
    }

    virtual eval_result eval() 
    {
    	if (if_cond)
    	{
	    	eval_result r = if_cond->eval();
	    	if (r->type == boolean_type)
	    	{
	    		if (r->value == TRUE)
	    		{
	    			for (node_list temp = if_stmts; temp; temp = temp->tail)
	    				temp->eval();
	    		}
	    		else if (if_next)
	    			if_next->eval();
	    	}
	    	else
	    	{
	    		cout << "ERROR: Type " << toString(r->type) << " is not valid in if cond" << endl;
	    		exit(EXIT_FAILURE);
	    	}
	    }
	    else
	    {
		    for (node_list temp = if_stmts; temp; temp = temp->tail)
		    	temp->eval();
		}
    }
    virtual void pprint(int indent) 
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	if (if_cond)
    	{
	    	cout << ws << "if ";
	    	if_cond->pprint(indent);
	    	cout << " then" << endl;
	    }
	    else
	    	cout << ws << "else" << endl;
    	for (node_list temp = if_stmts; temp; temp = temp->tail)
    		temp->pprint(indent+1);
    	if (if_next)
    		if_next->pprint(indent);
    	if (!if_cond && !if_next)
    		cout << ws << "end if;" << endl;
    }
};

struct elsif_node : if_node {
	node if_cond;
    node_list if_stmts;
    node if_next;

    elsif_node(node cond, node_list s, node n):
        if_node(cond, s, n), if_cond(cond), if_stmts(s), if_next(n)
    {
    }
    virtual void pprint(int indent)
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	cout << ws << "elsif ";
	    if_cond->pprint(indent);
	    cout << " then" << endl;
	    for (node_list temp = if_stmts; temp; temp = temp->tail)
    		temp->pprint(indent+1);
    	if (if_next)
    		if_next->pprint(indent);
    }
};

struct while_node : node_block {
    node while_cond;
    node_list while_stmts;

    while_node(node cond, node_list s) :
        while_cond(cond), while_stmts(s)
    {
    }

    virtual eval_result eval() 
    {
    	eval_result r = while_cond->eval();
    	if (r->type == boolean_type)
    	{
    		if (r->value == TRUE)
    		{
    			for (node_list temp = while_stmts; temp; temp = temp->tail)
	    			temp->eval();
	    		this->eval();
    		}
    	}
    	else
    	{
    		cout << "ERROR: Type " << toString(r->type) << " is not valid in while cond" << endl;
	    	exit(EXIT_FAILURE);
    	}
    }
    virtual void pprint(int indent) 
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	cout << ws << "while ";
    	while_cond->pprint(indent);
    	cout << " loop" << endl;
    	for (node_list temp = while_stmts; temp; temp = temp->tail)
    		temp->pprint(indent+1);
    	cout << ws << "end loop;" << endl;
    }

};

struct output_node : node_block {
    char * function;
    node argument;

    output_node(char * f, node arg) :
        function(f), argument(arg)
    {
    }

    virtual eval_result eval() 
    {
    	eval_result r = argument->eval();
    	int v = r->value;
    	if (r->type == undefined_type)
    	{
    		cout << "ERROR: Cannot I/O " << toString(undefined_type);
    		exit(EXIT_FAILURE);
    	}
    	if (!strcmp(function, "get"))
    	{
    		cout << "input >> ";
    		cin >> v;
    		if (r->type == integer_type)
    			argument->enter(r->type, v);
    		else if (v == FALSE)
    			argument->enter(r->type, FALSE);
    		else {
    			argument->enter(r->type, TRUE);
    			v = TRUE;
    		}
    	}
    	else if (!strcmp(function, "put"))
    	{
    		if (r->value == UNINITIALIZED)
	    	{
	    		cout << "ERROR: Cannot put uninitialized varible" << endl;
	    		exit(EXIT_FAILURE);
	    	}
    		cout << r->value << endl;
    	}

    	return new eval_result_block(r->type, v);
    }
    virtual void pprint(int indent) 
    {
    	string ws;
    	for (int i = 0; i < indent; i++)
    		ws += "    ";
    	cout << ws << function << "(";
    	argument->pprint(indent);
    	cout << ");" << endl;
    }

};
