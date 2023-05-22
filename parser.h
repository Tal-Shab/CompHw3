#ifndef HW3_OUTPUT_CPP_PARSER_H
#define HW3_OUTPUT_CPP_PARSER_H
#include <string>

class Node {
public:
    std::string name;

    Node(const std::string s= ""): name(s) {}
    Node(const Node& node) : name(node.name) {}
    virtual ~Node()
};

class Type : public Node {
    Type(Node* t) : Node(t) {}
};

class Exp: public Node {
public:
    std::string type;
    bool is_var;

};

class ExpList : public Node {
public:
    std::vector<Exp*> expressions;
};

class Call : public Node {
public:
    std::string type;

};

class Statement : public Node {
public:

};

class Statements : public Node {
public:

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
    FormalDecl(std::string t, Node* node) : type(t), Node(node) {}
};

class FormalsList : public Node{
    std::vector<FormalDecl> param_list;
    FormalsList(FormalDecl* dec, FormalsList* list)
};

class Formals: public Node{

};

class FuncDecl : public Node {
public:
    FuncDecl(OverRide* override, RetType* rt, Node* id, Formals* params);//TODO
};


#define YYSTYPE Node*



#endif //HW3_OUTPUT_CPP_PARSER_H
