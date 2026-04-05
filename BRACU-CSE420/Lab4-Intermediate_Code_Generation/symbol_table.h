#include "scope_table.h"

class symbol_table
{
private:
    scope_table *curr_scope = NULL;
    int scope_size = 10;
    int ID = 0;
public:
	int getID()
	{
		return curr_scope->getID();
	}
    void set_size(int n)
    {
        scope_size = n;
    }
    void enter_scope(ofstream& outlog)
    {
        ID+=1;
        scope_table *new_scope = new scope_table(scope_size, ID);
        new_scope->set_prnt(curr_scope);
        curr_scope = new_scope;
        outlog<<"New ScopeTable with ID "<<curr_scope->getID()<<" created"<<endl<<endl;
        //if(new_scope->getID() != "1")cout<<curr_scope->getID()<<" "<<(curr_scope->get_prnt())->getID()<<endl;
    }

    void exit_scope(ofstream& outlog)
    {
    	outlog<<"Scopetable with ID "<<curr_scope->getID()<<" removed"<<endl<<endl;
        scope_table *buffer = curr_scope;
        curr_scope = curr_scope->get_prnt();
        delete buffer;
        buffer = NULL;
        //cout<<curr_scope->getID()<<endl;
    }

    bool Insert_in_table(string name, string type)
    {
        if(curr_scope->Insert_in_scope(name,type)) return true;
        else return false;
    }

    bool Remove_from_table(string name)
    {
        if(curr_scope->Delete_from_scope(name)) return true;
        else return false;
    }

    symbol_info* Lookup_in_table(string name)
    {
        symbol_info *symbol = curr_scope->Lookup_in_scope(name);
        scope_table *buffer_scope = curr_scope->get_prnt();
        if(symbol==NULL)
        {
            while(buffer_scope!=NULL)
            {
                symbol = buffer_scope->Lookup_in_scope(name);
                if(symbol!=NULL) return symbol;
                buffer_scope = buffer_scope->get_prnt();
            }
        }
        
        return symbol;
    }

    void Print_current_scope()
    {
        //curr_scope->Print_scope();
    }

    void Print_all_scope(ofstream& outlog)
    {
        outlog<<"################################"<<endl<<endl;
        scope_table *buffer = curr_scope;

        while(buffer!=NULL)
        {
            buffer->Print_scope(outlog);
            buffer = buffer->get_prnt();
        }
        outlog<<"################################"<<endl<<endl;
    }

    ~symbol_table()
    {
        delete curr_scope;
    }

};
