#include "symbol_info.h"

class scope_table
{
private:
    symbol_info** chains;
    int tbl_size;
    int num_chld = 0;
    int ID;
    scope_table *parent_scope = NULL;
    int hash_func(string symbol)
    {
        int sum = 0;
        for (int i = 0; i < symbol.size(); i++)
        {
            sum += (int)symbol[i];
        }
        return sum%tbl_size;
    }
public:
    scope_table(){}
    scope_table(int n, int ID)
    {
        tbl_size = n;

        chains = new symbol_info*[n];

        for(int i = 0; i < n; i++)
        {
            chains[i] = NULL;
        }
        this->ID = ID;
    }

    void set_prnt(scope_table *table)
    {
        parent_scope = table;
        if(parent_scope!=NULL) parent_scope->incrs_chld();
        //set_scope_table_ID();
    }

    scope_table* get_prnt()
    {
        return parent_scope;
    }

    // void set_scope_table_ID()
    // {
    //     if(parent_scope) ID = parent_scope->getID()+"."+to_string(parent_scope->get_num_chld());
    //     else ID = "1";
    // }

    int get_num_chld()
    {
        return num_chld;
    }

    void incrs_chld()
    {
        num_chld++;
    }

    int getID()
    {
        return ID;
    }

    symbol_info* Lookup_in_scope(string name)
    {
        int pos=0;
        int hash_val = hash_func(name);
        symbol_info *curr_sym = chains[hash_val];

        while(curr_sym != NULL)
        {
            if (curr_sym->getname() == name)
            {
                return curr_sym;
            }
            else
            {
                pos++;
                curr_sym = curr_sym->get_next();
            }
        }

        return curr_sym;
    }

    bool Insert_in_scope(string name, string type)
    {
        int pos = 0;
        symbol_info *new_sym = new symbol_info(name,type);

        int hash_val = hash_func(name);

        if(chains[hash_val]==NULL)
        {
            chains[hash_val] = new_sym;
            return true;
        }
        else
        {
            if (chains[hash_val]->getname() == name)
            {
                return false;
            }

            pos++;
            symbol_info *buffer = chains[hash_val];
            symbol_info *curr_sym = chains[hash_val]->get_next();

            while(true)
            {
                if(curr_sym == NULL)
                {
                    buffer->set_next(new_sym);
                    return true;
                }
                else
                {
                    if (curr_sym->getname() == name)
                    {
                        return false;
                    }
                    pos++;
                    buffer = curr_sym;
                    curr_sym = curr_sym->get_next();
                }
            }
        }
    }

    bool Delete_from_scope(string name)
    {
        int pos = 0;
        int hash_val = hash_func(name);
        symbol_info *curr_sym = chains[hash_val];


        if(curr_sym == NULL)
        {
            return false;
        }

        else if (curr_sym->getname() == name)
        {
            chains[hash_val] = curr_sym->get_next();
            curr_sym->set_next(NULL);
            delete curr_sym;
            curr_sym = NULL;
            return true;
        }

        else
        {
            pos++;
            symbol_info *buffer = curr_sym;
            curr_sym = curr_sym->get_next();
            while(curr_sym!=NULL)
            {
                if (curr_sym->getname() == name)
                {
                    buffer->set_next(curr_sym->get_next());
                    curr_sym->set_next(NULL);
                    delete curr_sym;
                    curr_sym = NULL;
                    return true;
                }
                else
                {
                    pos++;
                    buffer = curr_sym;
                    curr_sym = curr_sym->get_next();
                }
            }
            return false;
        }
    }

    void Print_scope(ofstream& outlog)
    {
    	string s = "";
    	s+="ScopeTable # "+to_string(ID)+"\n";
        //cout<<"ScopeTable # "<<ID<<endl;

        for(int i = 0; i < tbl_size; i++)
        {
            if(chains[i]!=NULL)
            {
            	s+=to_string(i)+" --> ";
            	//cout<<i<<" --> ";

		        symbol_info *curr_sym = chains[i];

		        while(curr_sym!=NULL)
		        {
		        	s+="\n< "+curr_sym->getname()+" : "+curr_sym->gettype()+" >\n";
                    if (curr_sym->getidtype() == "func_def")
                    {
                        s+="Function Definition\n";
                        s+="Return Type: "+curr_sym->getvartype()+"\n";
                        s+="Number of Parameters: "+to_string(curr_sym->getparamlist().size())+"\n";
                        s+="Parameter Details: ";
                        for(int i = 0; i<curr_sym->getparamlist().size(); i++)
                        {
                            s+=curr_sym->getparamlist()[i] + " " + curr_sym->getparamname()[i];
                            if(i!=curr_sym->getparamlist().size()-1) s+=", ";
                        }
                        //cout<<"Function Definition"<<endl;
                    }
                    else if (curr_sym->getidtype() == "var")
                    {
                        s+="Variable\n";
                        s+="Type: "+curr_sym->getvartype()+"\n";
                        //cout<<"Variable"<<endl;
                    }
                    else if (curr_sym->getidtype() == "array")
                    {
                        s+="Array\n";
                        s+="Type: "+curr_sym->getvartype()+"\n";
                        s+="Size: "+to_string(curr_sym->getarraysize())+"\n";
                        //cout<<"Array"<<endl;
                    }
                    else
                    {
                        s+="Error\n";
                        //cout<<"Error"<<endl;
                    }
		            //cout<<"< "<<curr_sym->getname()<<" : "<<curr_sym->gettype()<<" > ";
		            curr_sym = curr_sym->get_next();
		        }
				s+="\n";
		        //cout<<endl;
            }
        }
		s+="\n";
		outlog<<s;
        //cout<<endl;
        //return s;
    }

    ~scope_table()
    {
        //cout<<"delete scope"<<endl;
        for(int i = 0; i<tbl_size; i++)
        {
            while(chains[i]!=NULL)
            {
                symbol_info *buffer = chains[i];
                chains[i] = chains[i]->get_next();
                buffer->set_next(NULL);
                delete buffer;
                buffer = NULL;
            }
        }
        delete[] chains;
    }
};