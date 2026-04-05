#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <map>

using namespace std;

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp, int &temp_count, int &label_count) const = 0;
};

// Expression node types

class ExprNode : public ASTNode {
protected:
    string node_type; // Type information (int, float, void, etc.)
public:
    ExprNode(string type) : node_type(type) {}
    virtual string get_type() const { return node_type; }
};

// Variable node (for ID references)

class VarNode : public ExprNode {
private:
    string name;
    ExprNode *index; // For array access, nullptr for simple variables

public:
    VarNode(string name, string type, ExprNode *idx = nullptr)
        : ExprNode(type), name(name), index(idx) {}

    ~VarNode() { if(index) delete index; }

    bool has_index() const { return index != nullptr; }

    string generate_index_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                               int &temp_count, int &label_count) const {
        // TODO: Implement this method
        // Should generate code to calculate the array index and return the temp variable
        if (!index) return "";
        string index_temp = index->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        int scale = (node_type == "float" || node_type == "double") ? 8 : 4;
        string scale_temp = "t" + to_string(temp_count++);
        outcode << scale_temp << " = " << scale << endl;
        string offset_temp = "t" + to_string(temp_count++);
        outcode << offset_temp << " = " << index_temp << " * " << scale_temp << endl;
        return offset_temp;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for variable access or array access
        string temp = "t" + to_string(temp_count++);

        if (!index && symbol_to_temp.find(name) != symbol_to_temp.end()) {
             return symbol_to_temp[name];
        }

        if (index) {
            string offset = generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << temp << " = " << name << "[" << offset << "]" << endl;
        } else {
            outcode << temp << " = " << name << endl;
            symbol_to_temp[name] = temp;
        }
        return temp;
    }

    string get_name() const { return name; }
};

// Constant node

class ConstNode : public ExprNode {
private:
    string value;

public:
    ConstNode(string val, string type) : ExprNode(type), value(val) {}

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for constant values
        string temp = "t" + to_string(temp_count++);
        outcode << temp << " = " << value << endl;
        return temp;
    }
};

// Binary operation node

class BinaryOpNode : public ExprNode {
private:
    string op;
    ExprNode *left;
    ExprNode *right;

public:
    BinaryOpNode(string op, ExprNode *left, ExprNode *right, string result_type)
        : ExprNode(result_type), op(op), left(left), right(right) {}

    ~BinaryOpNode() {
        delete left;
        delete right;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for binary operations
        string lt = left ? left->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string rt = right ? right->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string temp = "t" + to_string(temp_count++);
        outcode << temp << " = " << lt << " " << op << " " << rt << endl;
        return temp;
    }
};

// Unary operation node

class UnaryOpNode : public ExprNode {
private:
    string op;
    ExprNode *expr;

public:
    UnaryOpNode(string op, ExprNode *expr, string result_type)
        : ExprNode(result_type), op(op), expr(expr) {}

    ~UnaryOpNode() { delete expr; }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for unary operations
        string et = expr ? expr->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string temp = "t" + to_string(temp_count++);
        if (op == "!" || op == "-" || op == "+") {
            outcode << temp << " = " << op << et << endl;
        } else {
            outcode << temp << " = " << op << " " << et << endl;
        }
        return temp;
    }
};

// Assignment node

class AssignNode : public ExprNode {
private:
    VarNode *lhs;
    ExprNode *rhs;

public:
    AssignNode(VarNode *lhs, ExprNode *rhs, string result_type)
        : ExprNode(result_type), lhs(lhs), rhs(rhs) {}

    ~AssignNode() {
        delete lhs;
        delete rhs;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for assignment operations
        string right_temp = rhs ? rhs->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        if (lhs->has_index()) {
            string offset = lhs->generate_index_code(outcode, symbol_to_temp, temp_count, label_count);
            outcode << lhs->get_name() << "[" << offset << "] = " << right_temp << endl;
        } else {
            outcode << lhs->get_name() << " = " << right_temp << endl;

            if (symbol_to_temp.find(lhs->get_name()) != symbol_to_temp.end()) {
                symbol_to_temp.erase(lhs->get_name());
            }
        }
        return right_temp;
    }
};

// Statement node types

class StmtNode : public ASTNode {
public:
    virtual string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                                 int &temp_count, int &label_count) const = 0;
};

// Expression statement node

class ExprStmtNode : public StmtNode {
private:
    ExprNode *expr;

public:
    ExprStmtNode(ExprNode *e) : expr(e) {}
    ~ExprStmtNode() { if(expr) delete expr; }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for expression statements
        if (expr) expr->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        return "";
    }
};

// Block (compound statement) node

class BlockNode : public StmtNode {
private:
    vector<StmtNode *> statements;

public:
    ~BlockNode() {
        for (auto stmt : statements) {
            delete stmt;
        }
    }

    void add_statement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for all statements in the block
        for (auto stmt : statements) {
            if (stmt) stmt->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

// If statement node

class IfNode : public StmtNode {
private:
    ExprNode *condition;
    StmtNode *then_block;
    StmtNode *else_block; // nullptr if no else part

public:
    IfNode(ExprNode *cond, StmtNode *then_stmt, StmtNode *else_stmt = nullptr)
        : condition(cond), then_block(then_stmt), else_block(else_stmt) {}

    ~IfNode() {
        delete condition;
        delete then_block;
        if (else_block) delete else_block;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for if-else statements
        string cond = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        string Ltrue = "L" + to_string(label_count++);
        string Lfalse = "L" + to_string(label_count++);
        string Lend = "L" + to_string(label_count++);
        outcode << "if " << cond << " goto " << Ltrue << endl;
        outcode << "goto " << Lfalse << endl;
        outcode << Ltrue << ":" << endl;
        if (then_block) then_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto " << Lend << endl;
        outcode << Lfalse << ":" << endl;
        if (else_block) else_block->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << Lend << ":" << endl;
        return "";
    }
};

// While statement node

class WhileNode : public StmtNode {
private:
    ExprNode *condition;
    StmtNode *body;

public:
    WhileNode(ExprNode *cond, StmtNode *body_stmt)
        : condition(cond), body(body_stmt) {}

    ~WhileNode() {
        delete condition;
        delete body;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for while loops
        string Lbegin = "L" + to_string(label_count++);
        string Lbody = "L" + to_string(label_count++);
        string Lend = "L" + to_string(label_count++);
        outcode << Lbegin << ":" << endl;
        string cond = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        outcode << "if " << cond << " goto " << Lbody << endl;
        outcode << "goto " << Lend << endl;
        outcode << Lbody << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto " << Lbegin << endl;
        outcode << Lend << ":" << endl;
        return "";
    }
};

// For statement node

class ForNode : public StmtNode {
private:
    ExprNode *init;
    ExprNode *condition;
    ExprNode *update;
    StmtNode *body;

public:
    ForNode(ExprNode *init_expr, ExprNode *cond_expr, ExprNode *update_expr, StmtNode *body_stmt)
        : init(init_expr), condition(cond_expr), update(update_expr), body(body_stmt) {}

    ~ForNode() {
        if (init) delete init;
        if (condition) delete condition;
        if (update) delete update;
        delete body;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for for loops
        if (init) init->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        string Lbegin = "L" + to_string(label_count++);
        string Lbody = "L" + to_string(label_count++);
        string Lend = "L" + to_string(label_count++);
        outcode << Lbegin << ":" << endl;
        string cond = condition ? condition->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        outcode << "if " << cond << " goto " << Lbody << endl;
        outcode << "goto " << Lend << endl;
        outcode << Lbody << ":" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        if (update) update->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << "goto " << Lbegin << endl;
        outcode << Lend << ":" << endl;
        return "";
    }
};

// Return statement node

class ReturnNode : public StmtNode {
private:
    ExprNode *expr;

public:
    ReturnNode(ExprNode* e) : expr(e) {}
    ~ReturnNode() { if (expr) delete expr; }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for return statements
        string val = expr ? expr->generate_code(outcode, symbol_to_temp, temp_count, label_count) : "";
        outcode << "return " << val << endl;
        return val;
    }
};

// Declaration node

class DeclNode : public StmtNode {
private:
    string type;
    vector<pair<string, int>> vars; // Variable name and array size (0 for regular vars)

public:
    DeclNode(string t) : type(t) {}
    
    void add_var(string name, int array_size = 0) {
        vars.push_back(make_pair(name, array_size));
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for variable declarations
        for (auto &var : vars) {
            if (var.second > 0) {
                outcode << "// Declaration: " << type << " " << var.first << "[" << var.second << "]" << endl;
            } else {
                outcode << "// Declaration: " << type << " " << var.first << endl;
            }
        }
        return "";
    }

    string get_type() const { return type; }
    const vector<pair<string, int>> &get_vars() const { return vars; }
};

// Function declaration node

class FuncDeclNode : public ASTNode {
private:
    string return_type;
    string name;
    vector<pair<string, string>> params; // Parameter type and name
    BlockNode *body;

public:
    FuncDeclNode(string ret_type, string n) : return_type(ret_type), name(n), body(nullptr) {}
    ~FuncDeclNode() { if (body) delete body; }
    
    void add_param(string type, string name) {
        params.push_back(make_pair(type, name));
    }
    
    void set_body(BlockNode* b) {
        body = b;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override {
        // TODO: Implement this method
        // Should generate code for function declarations
        outcode << "// Function: " << return_type << " " << name << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            outcode << params[i].first << " " << params[i].second;
            if (i + 1 < params.size()) outcode << ", ";
        }
        outcode << ")" << endl;
        if (body) body->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        outcode << endl;
        return "";
    }
};

// Helper class for function arguments

class ArgumentsNode : public ASTNode {
private:
    vector<ExprNode *> args;

public:
    ~ArgumentsNode() {
        // Don't delete args here - they'll be transferred to FuncCallNode
    }

    void add_argument(ExprNode* arg) {
        if (arg) args.push_back(arg);
    }

    ExprNode* get_argument(int index) const {
        if (index >= 0 && index < args.size()) {
            return args[index];
        }
        return nullptr;
    }
    
    size_t size() const {
        return args.size();
    }
    
    const vector<ExprNode*>& get_arguments() const {
        return args;
    }

    string generate_code(ofstream &outcode, map<string, string> &symbol_to_temp,
                         int &temp_count, int &label_count) const override
    {
        // This node doesn't generate code directly
        return "";
    }
};

// Function call node

class FuncCallNode : public ExprNode {
private:
    string func_name;
    vector<ExprNode *> arguments;

public:
    FuncCallNode(string name, string result_type)
        : ExprNode(result_type), func_name(name) {}
    
    ~FuncCallNode() {
        for (auto arg : arguments) {
            delete arg;
        }
    }
    
    void add_argument(ExprNode* arg) {
        if (arg) arguments.push_back(arg);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for function calls
        vector<string> temps;
        for (auto arg : arguments) {
            if (arg) {
                string t = arg->generate_code(outcode, symbol_to_temp, temp_count, label_count);
                temps.push_back(t);
            }
        }
        for (auto &t : temps) {
            outcode << "param " << t << endl;
        }
        string ret = "t" + to_string(temp_count++);
        outcode << ret << " = call " << func_name << ", " << temps.size() << endl;
        return ret;
    }
};

// Program node (root of AST)

class ProgramNode : public ASTNode{
private:
    vector<ASTNode *> units;

public:
    ~ProgramNode() {
        for (auto unit : units) {
            delete unit;
        }
    }
    
    void add_unit(ASTNode* unit) {
        if (unit) units.push_back(unit);
    }
    
    string generate_code(ofstream& outcode, map<string, string>& symbol_to_temp,
                        int& temp_count, int& label_count) const override {
        // TODO: Implement this method
        // Should generate code for the entire program
        for (auto unit : units) {
            if (unit) unit->generate_code(outcode, symbol_to_temp, temp_count, label_count);
        }
        return "";
    }
};

#endif // AST_H