#include "symbol_info.h"

class scope_table
{
private:
    int bucket_count;
    int unique_id;
    scope_table *parent_scope = NULL;
    vector<list<symbol_info *>> table;

    int hash_function(string name)
    {
        unsigned long hash = 0;
        for (char c : name)
        {
            hash += c;
        }
        return hash % bucket_count;
    }

public:
    scope_table() : bucket_count(0), unique_id(0), parent_scope(NULL) {}
    
    scope_table(int bucket_count, int unique_id, scope_table *parent_scope)
    {
        this->bucket_count = bucket_count;
        this->unique_id = unique_id;
        this->parent_scope = parent_scope;
        table.resize(bucket_count);
    }

    scope_table *get_parent_scope() { return parent_scope; }
    int get_unique_id() { return unique_id; }

    symbol_info *lookup_in_scope(symbol_info* symbol)
    {
        int idx = hash_function(symbol->get_name());
        for (auto it = table[idx].begin(); it != table[idx].end(); ++it)
        {
            if ((*it)->get_name() == symbol->get_name())
            {
                return *it;
            }
        }
        return NULL;
    }
    
    // Overload to lookup by name string
    symbol_info *lookup_in_scope(string name)
    {
        int idx = hash_function(name);
        for (auto it = table[idx].begin(); it != table[idx].end(); ++it)
        {
            if ((*it)->get_name() == name)
            {
                return *it;
            }
        }
        return NULL;
    }

    bool insert_in_scope(symbol_info* symbol)
    {
        if (lookup_in_scope(symbol->get_name()) != NULL) return false;
        int idx = hash_function(symbol->get_name());
        table[idx].push_back(symbol);
        return true;
    }

    bool delete_from_scope(symbol_info* symbol)
    {
        int idx = hash_function(symbol->get_name());
        for (auto it = table[idx].begin(); it != table[idx].end(); ++it)
        {
            if ((*it)->get_name() == symbol->get_name())
            {
                delete *it; 
                table[idx].erase(it);
                return true;
            }
        }
        return false;
    }

    void print_scope_table(ofstream& outlog);

    ~scope_table()
    {
        for(int i=0; i<bucket_count; i++) {
            for(auto s : table[i]) {
                delete s;
            }
        }
    }
};

void scope_table::print_scope_table(ofstream& outlog)
{
    outlog << "ScopeTable # "+ to_string(unique_id) << endl;
    for(int i=0; i<bucket_count; i++) {
        if(table[i].empty()) continue;
        outlog << i << " --> " << endl;
        for(auto s : table[i]) {
            outlog << "< " << s->get_name() << " : " << s->get_type() << " >" << endl;
            
            if(s->get_id_type() == "var") {
                outlog << "Variable" << endl;
                outlog << "Type: " << s->get_data_type() << endl;
            } else if(s->get_id_type() == "array") {
                outlog << "Array" << endl;
                outlog << "Type: " << s->get_data_type() << endl;
                outlog << "Size: " << s->get_array_size() << endl;
            } else if(s->get_id_type() == "func") {
                outlog << "Function Definition" << endl;
                outlog << "Return Type: " << s->get_return_type() << endl;
                outlog << "Number of Parameters: " << s->get_parameters().size() << endl;
                outlog << "Parameter Details: ";
                auto params = s->get_parameters();
                for(size_t j=0; j<params.size(); j++) {
                    outlog << params[j].first << " " << params[j].second;
                    if(j < params.size()-1) outlog << ", ";
                }
                outlog << endl;
            }
            outlog << endl;
        }
    }
}
