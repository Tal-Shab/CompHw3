#include "symbol.h"
#include "hw3_output.hpp"
#include "iostream"

extern table_stack table;
extern int yylineno;

bool type_compatible(const string& right, const string& left){
    if (right != left) {
        if (!(right == "int" && left == "byte")) {
            return false;
        }
    }
    return true;
}


bool type_vector_compatible(const vector<string>& right, const vector<string>& left){
    if(right.size() != left.size()){
        return false;
    }
    for(int i=0; i<right.size(); i++){
        if(!type_compatible(right[i], left[i])){
            return false;
        }
    }
    return true;
}


///HELPER
static bool are_vectors_equal(const vector<string>& a,const  vector<string>& b) {
    if(a.size() != b.size()) {
        return false;
    }
    for(int i = 0; i < a.size(); i++) {
        if(a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

///********************* SYMBOL TABLE SCOPE ********************************///

///upon inserting, check if exists, if yes, exit program
bool symbol_table_scope::exists_in_scope(const symbol_table_entry& entry) {
    for(auto it = entries.begin(); it != entries.end(); it++) {
        if(it->name == entry.name) {
            if(it->is_func == false || entry.is_func == false) {
                output::errorDef(yylineno, entry.name); ///be careful
                exit(0);
            }
            //if we reach this, both are funcs
            if(it->is_override == false) {
                output::errorFuncNoOverride(yylineno, it->name);
                exit(0);
            }
            if(entry.is_override == false) {
                output::errorOverrideWithoutDeclaration(yylineno, it->name);
                exit(0);
            }
            //if we reach this, we know that both are override
            if(are_vectors_equal(it->params, entry.params)) {
                output::errorDef(yylineno, entry.name); ///be careful
                exit(0);
            }
        }
    }
    return false;
}

///called from insert to table stack, and called after checking exists_in_scope to all scopes
void symbol_table_scope::insert_to_scope(symbol_table_entry &entry) {
    entries.push_back(entry);
}


void symbol_table_scope::print_scope() { //TODO: remove prints
    
    ////cout << " in print_scope " << table.tables_stack[0].entries[3].name << endl;
    ////cout << this->entries.begin()->name << endl;

    for(auto it=(this->entries).begin(); it != (this->entries).end(); it++){
        ////cout <<  "in for" << endl;
        if(it->is_func) {
            ////cout <<  " !!!!!!!!!!!!!  in func  !!!!!!!!!!!!!!!!!!!" << endl;
            output::printID(it->name, 0, output::makeFunctionType(it->type, it->params));
        }
        else {
            ////cout <<  " !!!!!!!!!!!!!  in else  !!!!!!!!!!!!!!!!!!!" << endl;
            output::printID(it->name, it->offset, it->type);
        }
    }
}

static bool are_params_equal(const vector<string>& called, const vector<string>& declared) {
    if(called.size() != declared.size())
        return false;

    for(int i = 0; i < called.size() ; i++){
        if(called[i] != declared[i]){
            if(!(called[i] == "byte" && declared[i] == "int")) {
                return false;
            }
        }
    }
    return true;
}

bool symbol_table_scope::symbol_declared_in_scope(const symbol_table_entry &entry) {
    if(entry.is_func == false) {
        for(auto it = entries.begin(); it != entries.end(); it++) {
            if(entry.name == it->name && it->is_func == false)
                return true;
        }
    }
    bool found_name = false;
    int counter = 0;
    if(entry.is_func == true) {
        for(auto it = entries.begin(); it != entries.end(); it++) {
                if(entry.name == it->name) {
                    found_name = true;
                    bool answer = are_params_equal(entry.params, it->params);
                    if(answer == true)
                        counter++;
                }
        }
        if(counter > 1) {
            output::errorAmbiguousCall(yylineno, entry.name);
            exit(0);
        }
        if(counter == 1)
            return true;
        if(counter == 0 && found_name == true){
            output::errorPrototypeMismatch(yylineno, entry.name);
            exit(0);
        }
    }
    return false;
}

///this func is only for variables (id)
symbol_table_entry* symbol_table_scope::get_variable_in_scope(const string &name) {
    for(auto it = entries.begin(); it != entries.end(); it++) {
        if(name == it->name && it->is_func == false)
            return &(*it);
    }
    return nullptr;
}





///********************* TABLE STACK ********************************///
table_stack::table_stack(){
    this->offsets_stack.push(0);
    this->open_scope();
    this->insert_symbol("print","void",true,false,{"string"});
    this->insert_symbol("printi","void",true,false,{"int"});
}

void table_stack::final_check() {
    symbol_table_entry* main_enrty = get_function("main");
    if(main_enrty->type != "void" || main_enrty->params.size() > 0){
        output::errorMainMissing();
        exit(0);
    }
    this->close_scope();
}

bool table_stack::symbol_exists(const symbol_table_entry& entry) {
    for(auto it = tables_stack.begin(); it != tables_stack.end(); it++) {
        it->exists_in_scope(entry);
    }
    return true;
}

void table_stack::insert_symbol(const string &n, string t, bool func, bool override, vector <string> p) {
    //insert to table_stack
    ////cout << "in insert_symbol " << n << " " << t << " " <<  func << endl;  
    if (n == "main" && override == true) {
        output::errorMainOverride(yylineno);
        exit(0);
    }
    int insert_offset = func ? 0 : offsets_stack.top();
    symbol_table_entry entry(n, t, insert_offset, func, override, p);

    this->symbol_exists(entry);

    //if we got here, the insertion is legal
    tables_stack.back().insert_to_scope(entry);
    /*maybe .back()*/

    if (!func) {
        offsets_stack.pop();
        offsets_stack.push(insert_offset + 1);
    }
}

///we didnt check here if some are similar - like - foo(int x, int x)
void table_stack::insert_func_args(vector<string> types, vector<string> names, string retType) {
    //cout << "in insert_func_args " << endl;
    
    this->open_scope(false, retType);

    //cout << "in insert_func_args, tables_stack size (number of scopes) is " << tables_stack.size() << endl;
    int offset = -1;
    for(int i = 0; i < types.size(); i++) {
        //cout << "in for !!!!! " << endl;
        //cout << types[i] << endl;
        //cout << names[i] << endl;
        symbol_table_entry entry(names[i], types[i], offset);
        //cout << i << " " << endl;

        symbol_exists(entry);

        this->tables_stack.back().entries.push_back(entry);

        offset--;
    }
}


void table_stack::open_scope(bool is_loop, string ret_type)
{
    //cout << "in open_scope " << endl;
    symbol_table_scope new_scope(is_loop, ret_type);
    tables_stack.push_back(new_scope);
    offsets_stack.push(offsets_stack.top());
    //cout << "in open_scope, tables_stack size (number of scopes) is " <<tables_stack.size() << endl;
}

void table_stack::close_scope() {
    ////cout << "in close scope" << endl;
    output::endScope();
    symbol_table_scope to_close = tables_stack.back();
    to_close.print_scope();
    tables_stack.pop_back();
    offsets_stack.pop();
}

///in inserting, we made sure that a varibale appears only once, so its enough for use to check if a variable exists in one scope
///havent used for variable. might use for functions
bool table_stack::symbol_declared(const symbol_table_entry &entry) {
    for(auto it = this->tables_stack.begin(); it != this->tables_stack.end(); it++) {
        bool answer = it->symbol_declared_in_scope(entry);
        if(answer == true)
            return true;
    }
    if(entry.is_func == false) {
        output::errorUndef(yylineno, entry.name);
        exit(0);
    }
    if(entry.is_func) {
        output::errorUndefFunc(yylineno, entry.name);
    }
    return false;
}

symbol_table_entry* table_stack::get_variable(const string &name) {
    for(auto it = this->tables_stack.begin(); it != this->tables_stack.end();  it++) {
        symbol_table_entry *returned = it->get_variable_in_scope(name);
        if (returned != nullptr) {
            return returned;
        }
    }
    output::errorUndef(yylineno, name);
    exit(0);
}


symbol_table_entry* table_stack::get_function(const string& name, vector<string> params){
    bool found_name = false;
    int counter = 0;
    symbol_table_entry* entry = nullptr;
    for(auto it = (this->tables_stack[0].entries).begin() ; it != (this->tables_stack[0].entries).end(); it++){
        if(name == it->name){
            found_name = true;
            
            if(type_vector_compatible(it->params, params)){
                counter++;
                entry = &(*it);
            }
        }
    }
    if(counter > 1){
        output::errorAmbiguousCall(yylineno, name);
        exit(0);
    }
    if(counter == 1){
        return entry;
    }
    //changed - added name != main. without, test 15 returned mismatch on "main(int x, int y)
    if(found_name && name != "main"){
        /*counter == 0*/
        output::errorPrototypeMismatch(yylineno, name);
        exit(0);
    }
    /*counter == 0  and found name == false*/
    if(name == "main"){
        output::errorMainMissing();
        exit(0);
    }
    output::errorUndefFunc(yylineno, name);
    exit(0);    
}

bool table_stack::checkLoop() {
    for(auto it = this->tables_stack.rbegin(); it != this->tables_stack.rend(); it++) {
        if(it->is_loop)
        {
            return true;
        }
    }
    return false;
}