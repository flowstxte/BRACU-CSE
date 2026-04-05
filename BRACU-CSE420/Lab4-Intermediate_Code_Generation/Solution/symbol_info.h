#ifndef SYMBOL_INFO_H
#define SYMBOL_INFO_H

#include <bits/stdc++.h>
using namespace std;

// Forward declaration of ASTNode
class ASTNode;

class symbol_info
{
private:
    string sym_name;
    string sym_type;
    string ID_type; //var, array, func_dec, func_def
    string var_type; //int, float, void, error
    int array_size;
    vector<string> param_list;//for functions
    vector<string> param_name;
    symbol_info *next_sym;
    ASTNode* ast_node; // Pointer to AST node
public:
    //symbol_info(){}
    symbol_info(string name, string type)
    {
        sym_name = name;
        sym_type = type;
        next_sym = NULL;
        ast_node = NULL;
    }

    void set_next(symbol_info *symbol)
    {
        next_sym = symbol;
    }

    symbol_info* get_next()
    {
        return next_sym;
    }

    string getname()
    {
        return sym_name;
    }
    string gettype()
    {
        return sym_type;
    }
    
    string getvartype()
    {
        return var_type;
    }
    
    void setvartype(string tp)
    {
    	var_type = tp;
    }
    
    string getidtype()
    {
        return ID_type;
    }
    
    void setidtype(string tp)
    {
    	ID_type = tp;
    }
    
    int getarraysize()
    {
        return array_size;
    }
    
    void setarraysize(int sz)
    {
    	array_size = sz;
    }
    
    void setparamlist(vector<string> list)
    {
    	param_list = list;
    }
    
    vector<string> getparamlist()
    {
    	return param_list;
    }
    
    vector<string> getparamname()
    {
    	return param_name;
    }
    
    void setparamname(vector<string> list)
    {
    	param_name = list;
    }
    
    int getparamsize()
    {
    	return param_list.size();
    }

    // New methods for AST support
    void set_ast_node(ASTNode* node)
    {
        ast_node = node;
    }

    ASTNode* get_ast_node()
    {
        return ast_node;
    }

    ~symbol_info()
    {
        delete next_sym;
        param_list.clear();
        param_name.clear();
        // Don't delete ast_node here - will be managed separately
    }
};

#endif // SYMBOL_INFO_H