#include<bits/stdc++.h>
using namespace std;

class symbol_info
{
private:
    string name;
    string type;

    // Write necessary attributes to store what type of symbol it is (variable/array/function)
    string id_type; 
    // Write necessary attributes to store the type/return type of the symbol (int/float/void/...)
    string data_type;
    string return_type;
    // Write necessary attributes to store the parameters of a function
    vector<pair<string, string>> parameters; // type, name
    // Write necessary attributes to store the array size if the symbol is an array
    int array_size;

public:
    symbol_info(string name, string type)
    {
        this->name = name;
        this->type = type;
        this->id_type = "";
        this->data_type = "";
        this->return_type = "";
        this->array_size = 0;
    }
    string get_name()
    {
        return name;
    }
    string get_type()
    {
        return type;
    }
    void set_name(string name)
    {
        this->name = name;
    }
    void set_type(string type)
    {
        this->type = type;
    }
    // Write necessary functions to set and get the attributes
    void set_id_type(string id_type) { this->id_type = id_type; }
    string get_id_type() { return id_type; }

    void set_data_type(string data_type) { this->data_type = data_type; }
    string get_data_type() { return data_type; }

    void set_return_type(string return_type) { this->return_type = return_type; }
    string get_return_type() { return return_type; }

    void set_array_size(int size) { this->array_size = size; }
    int get_array_size() { return array_size; }

    void add_parameter(string type, string name) {
        parameters.push_back({type, name});
    }
    vector<pair<string, string>> get_parameters() { return parameters; }

    string getname() { return name; }
    string gettype() { return type; }
    ~symbol_info()
    {
        // Write necessary code to deallocate memory, if necessary
    }
};