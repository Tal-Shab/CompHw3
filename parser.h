#ifndef HW3_OUTPUT_CPP_PARSER_H
#define HW3_OUTPUT_CPP_PARSER_H
#include <string>
#include <vector>
#include "symbol.h"
#include "hw3_output.hpp"

class Node;
class Statements;
class Statement;
class Type;
class Call;
class Exp;
class ExpList;
class Statement;
class Statements;
class Program;
class Funcs;
class OverRide;
class RetType;
class FormalDecl;
class FormalsList;
class Formals;
class FuncDecl;


#define YYSTYPE Node*

extern char* yytext;

class Node {
public:
    std::string name;

    Node(const std::string s= ""): name(s) {}
    //Node(const Node* node) : name(node->name) {}
    virtual ~Node(){}
};

class Type : public Node {
public:
    std::string type;

    /*Type -> INT | BYTE | BOOL */
    Type(std::string t) : type(t) {}

    ~Type() = default;
};

class Call : public Node {
public:
    std::string type;

    /*Call -> ID (ExpList)
    Call -> ID ()*/
    Call(Node* id, ExpList* params = nullptr);

    ~Call() = default;
};

class Exp: public Node {
public:
    std::string type;

    //exp -> (exp)
    Exp(Exp* e) : type(e->type), Node(e->name) {};

    //exp -> exp binop/relop/and/or exp
    Exp(Exp* e1, Node* op, Exp* e2);

    //exp -> not exp
    Exp(Node* op, Exp* e);

    //exp -> num b
    Exp(Node* n, Node* b);

    //exp -> num/string/true/false
    Exp(Node* n, std::string type);

    //exp -> (type) exp
    Exp(Type* t, Exp* e);

    //exp -> id
    Exp(Node* id);

    //exp -> call
    Exp(Call* c) : Node(c->name), type(c->type){}

    ~Exp() = default;
};

/*this was added to check the type of exp before going in statement.
checking the type in statement led to a wrong yyline printing.*/
void check_exp(Exp* exp);


class ExpList : public Node {
public:
    std::vector<Exp> expressions;

    //explist -> exp
    ExpList(Exp* e);

    //explist -> exp , explist
    ExpList(Exp* e, ExpList* list);

    ~ExpList() = default;
};


class Statement : public Node {
public:

    //statement -> { statements }
    Statement() = default;

    //statement -> type ID ;
    Statement(Type* t, Node* id);

    //statement -> type ID = Exp ;
    Statement(Type* t, Node* id, Exp* e);

    /*statement -> ID = Exp ;
    /statement -> return exp;*/
    Statement(Node* id, Exp* e);

    //statement -> call ;
    Statement(Call* c) : Node(c->type){}

    //statement -> return ;
    //statement -> break ;
    //statement -> continue ;
    Statement(Node* n);

    //statement -> if ( exp ) statement
    Statement(Exp* e);

    ~Statement() = default;

};

class Statements : public Node {
public:

    //Statements -> Statement
    Statements(Statement* st) : Node(st->name) {}

    //Statements -> Statements Statement
    Statements(Statements* sts, Statement* st) : Node(st->name) {}

    ~Statements() = default;

};

class Program : public Node{
public:

    //Program -> Funcs
    Program() {}

    ~Program() = default;
};

class Funcs : public Node{
public:

    //Funcs -> FuncDecl Funcs | epsilon
    Funcs() {}
    ~Funcs() = default;
};

class OverRide: public Node{
public:
    bool isOverRide;

    //OverRide -> OVERRIDE | epsilon
    OverRide(bool answer = false): isOverRide(answer) {}
    ~OverRide() = default;
};

class RetType : public Node{
public:
    std::string type;

    //RetType -> Type | VOID 
    RetType(std::string t) : type(t) {}
    ~RetType() = default;
};

class FormalDecl : public Node{
public:
    std::string type;

    //FuncDecl -> OverRide RetType ID ( Formals )  { Statements }
    FormalDecl(Type* t, Node* node) : type(t->type), Node(node->name) {}
    ~FormalDecl() = default;
};

class FormalsList : public Node{
public:
    std::vector<FormalDecl> param_list;

    //FormalsList -> FormalDecl , FormalsList
    FormalsList(FormalDecl* dec, FormalsList* list);

    //FormalsList -> FormalDecl
    FormalsList(FormalDecl* dec);
    ~FormalsList() = default;
};

class Formals: public Node{
public:
    std::vector<FormalDecl> param_list;

    //Formals -> FormalsList
    Formals(FormalsList* fl, int l_n) : param_list(fl->param_list) {}

    //Formals -> epsilon
    Formals() {}
    ~Formals() = default;

};

class FuncDecl : public Node {
public:

    ////FormalDecl -> Type ID
    FuncDecl(OverRide* override, RetType* rt, Node* id, Formals* params);
    ~FuncDecl() = default;
};




#endif //HW3_OUTPUT_CPP_PARSER_H
