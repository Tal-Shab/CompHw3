#ifndef HW3_OUTPUT_CPP_PARSER_H
#define HW3_OUTPUT_CPP_PARSER_H
#include <string>
#include <vector>
#include "hw3_output.hpp"
#include "symbol.h"

class Statements;
class Statement;

class Node {
public:
    std::string name;

    Node(const std::string s= ""): name(s) {}
    Node(const Node* node) : name(node.name) {}
    virtual ~Node()
};

class Type : public Node {
public:
    std::string type;
    Type(std::string t) : type(t) {}
};

class Call : public Node {
public:
    std::string type;
};

class Exp: public Node {
public:
    std::string type;
    //bool is_var;

    //exp -> (exp)
    Exp(Exp* e) : type(e->type), Node(e) {};

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
    Exp(Call* c);
};

class ExpList : public Node {
public:
    std::vector<Exp> expressions;

    //explist -> exp
    ExpList(Exp* e);

    //explist -> exp , explist
    ExpList(Exp* e, ExpList* list);
};


class Statement : public Node {
public:

    //statement -> { statements }
    Statement();

    //statement -> type ID ;
    Statement(Type* t, Node* id);

    //statement -> type ID = Exp ;
    Statement(Type* t, Node* id, Exp* e);

    //statement -> ID = Exp ;
    Statement(Node* id, Exp* e);

    //statement -> call ;
    Statement(Call* c);

    //statement -> return ;
    //statement -> break ;
    //statement -> continue ;
    Statement(Node* n);

    //statement -> return exp;
    Statement(Node* ret, Exp* e);

    //statement -> if ( exp ) statement
    Statement(Exp* e);

};

class Statements : public Node {
public:
    Statements(Statement* st);
    Statements(Statements* sts, Statement* st);

};

class Program : public Node{
    /*
public:
    Program() {}
    ~Program() = default
     */
};

class Funcs : public Node{
    /*
public:
    Funcs() {}
    ~Funcs() = default
     */
};

class OverRide: public Node{
public:
    bool isOverRide;
    OverRide(bool answer = false): isOverRide(answer) {}
};

class RetType : public Node{
public:
    std::string type;
    RetType(std::string t) : type(t) {}
};

class FormalDecl : public Node{
public:
    std::string type;
    FormalDecl(Type* t, Node* node) : type(t->type), Node(node) {}
};

class FormalsList : public Node{
    std::vector<FormalDecl> param_list;
    FormalsList(FormalDecl* dec, FormalsList* list);
    FormalsList(FormalDecl* dec);
};

class Formals: public Node{
    std::vector<FormalDecl> param_list;
    Formals(FormalsList* fl) : param_list(fl->param_list) {}
    Formals() {}
F
};

class FuncDecl : public Node {
public:
    FuncDecl(OverRide* override, RetType* rt, Node* id, Formals* params);//TODO
};


#define YYSTYPE Node*



#endif //HW3_OUTPUT_CPP_PARSER_H
