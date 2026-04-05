%{

#include "symbol_table.h"
#include "ast.h"
#include "three_addr_code.h"
#include <iostream>
#include <fstream>
#include <string>

/* Define the type for all grammar symbols */
#define YYSTYPE symbol_info*

extern FILE *yyin;
int yyparse(void);
int yylex(void);
extern YYSTYPE yylval;

symbol_table *symtbl = new symbol_table();
ProgramNode* ast_root = new ProgramNode();

int lines = 1;
int errors = 0;
ofstream outlog, outerror, outcode;

string varlist=""; //for variable declarartion list
vector<string>paramlist; //for parameter list fot func dec and func def
vector<string>paramname; //for func def	
vector<string>arglist; //to store types of function argument

int is_func = 0; //is compound statement in function definition

string ret_type, func_name, func_ret_type;

void yyerror(char *s)
{
	outlog<<"At line "<<lines<<" "<<s<<endl<<endl;
	outerror<<"At line "<<lines<<" "<<s<<endl<<endl;
	errors++;
	
	varlist = "";
	paramlist.clear();
	paramname.clear();
	arglist.clear();
	is_func = 0;
	ret_type = "";
	func_name = "";
	func_ret_type = "";
}

%}

/* Declare tokens */
%token IF ELSE FOR WHILE DO BREAK INT CHAR FLOAT DOUBLE VOID RETURN SWITCH CASE DEFAULT CONTINUE PRINTLN ADDOP MULOP INCOP DECOP RELOP ASSIGNOP LOGICOP NOT LPAREN RPAREN LCURL RCURL LTHIRD RTHIRD COMMA SEMICOLON CONST_INT CONST_FLOAT ID

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

start : program
	{
		outlog<<"At line no: "<<lines<<" start : program "<<endl<<endl;
		outlog<<"Symbol Table"<<endl<<endl;
		
		symtbl->Print_all_scope(outlog);
		
		$$ = $1;
		// Root of AST is the program node
		ast_root = (ProgramNode*)$1->get_ast_node();
	}
	;

program : program unit
	{
		outlog<<"At line no: "<<lines<<" program : program unit "<<endl<<endl;
		outlog<<$1->getname()+"\n"+$2->getname()<<endl<<endl;
		
		$$ = new symbol_info($1->getname()+"\n"+$2->getname(),"program");
		
		// Create/update AST node for program
		ProgramNode* prog;
		if($1->get_ast_node()) {
			prog = (ProgramNode*)$1->get_ast_node();
		} else {
			prog = new ProgramNode();
		}
		
		// Add the unit to the program
		if($2->get_ast_node()) {
			prog->add_unit($2->get_ast_node());
		}
		
		$$->set_ast_node(prog);
	}
	| unit
	{
		outlog<<"At line no: "<<lines<<" program : unit "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
		
		$$ = new symbol_info($1->getname(),"program");
		
		// Create AST node for program with a single unit
		ProgramNode* prog = new ProgramNode();
		if($1->get_ast_node()) {
			prog->add_unit($1->get_ast_node());
		}
		$$->set_ast_node(prog);
	}
	;

unit : var_declaration
	 {
		outlog<<"At line no: "<<lines<<" unit : var_declaration "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
		
		$$ = new symbol_info($1->getname(),"unit");
		$$->set_ast_node($1->get_ast_node());
	 }
     | func_definition
     {
		outlog<<"At line no: "<<lines<<" unit : func_definition "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
		
		$$ = new symbol_info($1->getname(),"unit");
		$$->set_ast_node($1->get_ast_node());
	 }
	 | error
	 {
	 	$$ = new symbol_info("","unit");
	 }
     ;

func_definition : type_specifier id_name LPAREN parameter_list RPAREN enter_func compound_statement
		{	
			outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN parameter_list RPAREN compound_statement "<<endl<<endl;
			outlog<<$1->getname()<<" "<<$2->getname()<<"("+$4->getname()+")\n"<<$7->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+" "+$2->getname()+"("+$4->getname()+")\n"+$7->getname(),"func_def");	
			
			// Create AST node for function definition
			FuncDeclNode* func = new FuncDeclNode($1->getname(), $2->getname());
			
			// Add parameters
			for(int i = 0; i < paramlist.size(); i++) {
				if(paramname[i] != "_null_") {
					func->add_param(paramlist[i], paramname[i]);
				}
			}
			
			// Set body
			if($7->get_ast_node()) {
				func->set_body((BlockNode*)$7->get_ast_node());
			}
			
			$$->set_ast_node(func);
			
			if(symtbl->getID()!=1)
			{
				symtbl->Remove_from_table($2->getname());
			}
			
			paramlist.clear();
			paramname.clear();	
		}
		| type_specifier id_name LPAREN RPAREN enter_func compound_statement
		{
			
			outlog<<"At line no: "<<lines<<" func_definition : type_specifier ID LPAREN RPAREN compound_statement "<<endl<<endl;
			outlog<<$1->getname()<<" "<<$2->getname()<<"()\n"<<$6->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+" "+$2->getname()+"()\n"+$6->getname(),"func_def");	
			
			// Create AST node for function definition
			FuncDeclNode* func = new FuncDeclNode($1->getname(), $2->getname());
			
			// Set body
			if($6->get_ast_node()) {
				func->set_body((BlockNode*)$6->get_ast_node());
			}
			
			$$->set_ast_node(func);
			
			if(symtbl->getID()!=1)
			{
				symtbl->Remove_from_table($2->getname());
			}
			
			paramlist.clear();
			paramname.clear();	
		}
 		;

enter_func : {
				//if(symtbl->getID()!="1") goto end2; //not in global scope , doesnt work because if not inserted lots of errors come in compound statement
				
				is_func=1;//compound statement is coming in function definition. enter parameter variables.
				
				if(paramlist.size()!=0) //check parameters
				{
					for(int i = 0; i < paramlist.size();i++)
					{
						if(paramname[i]=="_null_")
						{
							outerror<<"At line no: "<<lines<<" Parameter "<<i+1<<"'s name not given in function definition of "<<func_name<<endl<<endl;
							outlog<<"At line no: "<<lines<<" Parameter "<<i+1<<"'s name not given in function definition of "<<func_name<<endl<<endl;
							errors++;
						}
					}
				}
				
				//check if function already present and do error checking
				if(symtbl->Insert_in_table(func_name,"ID"))
				{
					(symtbl->Lookup_in_table(func_name))->setvartype(func_ret_type);
					(symtbl->Lookup_in_table(func_name))->setidtype("func_def");
					(symtbl->Lookup_in_table(func_name))->setparamlist(paramlist);//initialize parameters
					(symtbl->Lookup_in_table(func_name))->setparamname(paramname);
				}
				else
				{
					outerror<<"At line no: "<<lines<<" Multiple declaration of function "<<func_name<<endl<<endl;
					outlog<<"At line no: "<<lines<<" Multiple declaration of function "<<func_name<<endl<<endl;
					errors++;
					// (symtbl->Lookup_in_table(func_name))->setidtype("func_def");
				}
					
				if((symtbl->Lookup_in_table(func_name))->getvartype() != func_ret_type)
				{
					outerror<<"At line no: "<<lines<<" Return type mismatch of function "<<func_name<<endl<<endl;
					outlog<<"At line no: "<<lines<<" Return type mismatch of function "<<func_name<<endl<<endl;
					errors++;
				}
				
				//end2:
				//;
            }
            ;

parameter_list : parameter_list COMMA type_specifier ID
		{
			outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier ID "<<endl<<endl;
			outlog<<$1->getname()+","+$3->getname()+" "+$4->getname()<<endl<<endl;
					
			$$ = new symbol_info($1->getname()+","+$3->getname()+" "+$4->getname(),"param_list");
			
			if(count(paramname.begin(),paramname.end(),$4->getname()))
			{
				outerror<<"At line no: "<<lines<<" Multiple declaration of variable "<<$4->getname()<<" in parameter of "<<func_name<<endl<<endl;
				outlog<<"At line no: "<<lines<<" Multiple declaration of variable "<<$4->getname()<<" in parameter of "<<func_name<<endl<<endl;
				errors++;
			}
			
			paramlist.push_back($3->getname());
			paramname.push_back($4->getname());
		}
		| parameter_list COMMA type_specifier
		{
			outlog<<"At line no: "<<lines<<" parameter_list : parameter_list COMMA type_specifier "<<endl<<endl;
			outlog<<$1->getname()+","+$3->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+","+$3->getname(),"param_list");
			
			paramlist.push_back($3->getname());
			paramname.push_back("_null_");
		}
 		| type_specifier ID
 		{
			outlog<<"At line no: "<<lines<<" parameter_list : type_specifier ID "<<endl<<endl;
			outlog<<$1->getname()<<" "<<$2->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+" "+$2->getname(),"param_list");
			
			paramlist.push_back($1->getname());
			paramname.push_back($2->getname());
		}
		| type_specifier
		{
			outlog<<"At line no: "<<lines<<" parameter_list : type_specifier "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"param_list");
			
			paramlist.push_back($1->getname());
			paramname.push_back("_null_");
		}
 		;

compound_statement : LCURL enter_scope_variables statements RCURL
			{ 
 		    	outlog<<"At line no: "<<lines<<" compound_statement : LCURL statements RCURL "<<endl<<endl;
				outlog<<"{\n"+$3->getname()+"\n}"<<endl<<endl;
				
				$$ = new symbol_info("{\n"+$3->getname()+"\n}","comp_stmnt");
				
				// Set AST node for compound statement
				$$->set_ast_node($3->get_ast_node());
				
				symtbl->Print_all_scope(outlog);
			    symtbl->exit_scope(outlog);
 		    }
 		    | LCURL enter_scope_variables RCURL
 		    { 
 		    	outlog<<"At line no: "<<lines<<" compound_statement : LCURL RCURL "<<endl<<endl;
				outlog<<"{\n}"<<endl<<endl;
				
				$$ = new symbol_info("{\n}","comp_stmnt");
				
				// Create empty block node
				BlockNode* block = new BlockNode();
				$$->set_ast_node(block);
				
				symtbl->Print_all_scope(outlog);
			    symtbl->exit_scope(outlog);
 		    }
 		    ;
enter_scope_variables :
			{
				symtbl->enter_scope(outlog);
				
				if(is_func == 1)
				{
					if(paramname.size()!=0)
					{
						for(int i = 0; i < paramname.size(); i++)
						{
							if(paramname[i]!="_null_")
							{
								symtbl->Insert_in_table(paramname[i],"ID");
								(symtbl->Lookup_in_table(paramname[i]))->setidtype("var");
								(symtbl->Lookup_in_table(paramname[i]))->setvartype(paramlist[i]);
							}
							
						}
					}
					is_func=0; //variable entered.if more compound statements come in func efinitions, don't enter the function variables.
				}
				
			}
 		    ;
 		    
var_declaration : type_specifier declaration_list SEMICOLON
		 {
			outlog<<"At line no: "<<lines<<" var_declaration : type_specifier declaration_list SEMICOLON "<<endl<<endl;
			outlog<<$1->getname()<<" "<<varlist<<";"<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+" "+varlist+";","var_dec");
			
			if($1->getname()=="void")
			{
				outerror<<"At line no: "<<lines<<" variable type can not be void "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" variable type can not be void "<<endl<<endl;
				errors++;
				$1 = new symbol_info("error","type"); //variable is declared void so pass error instead
			}
			
			// Create AST node for variable declaration
			DeclNode* declNode = new DeclNode($1->getname());
			
			// Parse the varlist to add variables to the declaration node
			stringstream _varlist(varlist);
			string varname;
			
			while(getline(_varlist,varname,','))
			{
				if(varname.find("[") == string::npos) // normal variable
				{
					declNode->add_var(varname, 0);
					
					if(symtbl->Insert_in_table(varname,"ID"))
					{
						(symtbl->Lookup_in_table(varname))->setvartype($1->getname());
						(symtbl->Lookup_in_table(varname))->setidtype("var");
					}
					else
					{
						outerror<<"At line no: "<<lines<<" Multiple declaration of variable "<<varname<<endl<<endl;
						outlog<<"At line no: "<<lines<<" Multiple declaration of variable "<<varname<<endl<<endl;
						errors++;
					}
				}
				else // array
				{
					stringstream _varname(varname);
					string name, size;
					
					getline(_varname,name,'['); // get array name
					getline(_varname,size,']'); // get array size
					
					declNode->add_var(name, stoi(size));
					
					if(symtbl->Insert_in_table(name,"ID"))
					{
						(symtbl->Lookup_in_table(name))->setvartype($1->getname());
						(symtbl->Lookup_in_table(name))->setidtype("array");
						(symtbl->Lookup_in_table(name))->setarraysize(stoi(size));
					}
					else
					{
						outerror<<"At line no: "<<lines<<" Multiple declaration of variable "<<name<<endl<<endl;
						outlog<<"At line no: "<<lines<<" Multiple declaration of variable "<<name<<endl<<endl;
						errors++;
					}
				}
			}
			
			$$->set_ast_node(declNode);
			varlist = "";
		 }
 		 ;

type_specifier : INT
		{
			outlog<<"At line no: "<<lines<<" type_specifier : INT "<<endl<<endl;
			outlog<<"int"<<endl<<endl;
			
			$$ = new symbol_info("int","type");
			ret_type = "int";
	    }
 		| FLOAT
 		{
			outlog<<"At line no: "<<lines<<" type_specifier : FLOAT "<<endl<<endl;
			outlog<<"float"<<endl<<endl;
			
			$$ = new symbol_info("float","type");
			ret_type = "float";
	    }
 		| VOID
 		{
			outlog<<"At line no: "<<lines<<" type_specifier : VOID "<<endl<<endl;
			outlog<<"void"<<endl<<endl;
			
			$$ = new symbol_info("void","type");
			ret_type = "void";
	    }
 		;

declaration_list : declaration_list COMMA id_name
		  {
 		  	string name = $3->getname();
 		  	outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID "<<endl<<endl;
 		  	
 		  	varlist=varlist+","+name;
 		  	
			outlog<<varlist<<endl<<endl;
			
 		  }
 		  | declaration_list COMMA id_name LTHIRD CONST_INT RTHIRD //array after some declaration
 		  {
 		  	string name = $3->getname();
 		  	string size = $5->getname();
 		  	outlog<<"At line no: "<<lines<<" declaration_list : declaration_list COMMA ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
 		  	
 		  	varlist=varlist+","+name+"["+size+"]";
 		  	
			outlog<<varlist<<endl<<endl;
			
 		  }
 		  |id_name
 		  {
 		  	string name = $1->getname();
 		  	outlog<<"At line no: "<<lines<<" declaration_list : ID "<<endl<<endl;
			outlog<<name<<endl<<endl;
			
			varlist+=name;
 		  }
 		  | id_name LTHIRD CONST_INT RTHIRD //array
 		  {
 		  	string name = $1->getname();
 		  	string size = $3->getname();
 		  	outlog<<"At line no: "<<lines<<" declaration_list : ID LTHIRD CONST_INT RTHIRD "<<endl<<endl;
			outlog<<name+"["+size+"]"<<endl<<endl;
			
			varlist=varlist+name+"["+size+"]";
 		  }
 		  ;
id_name : ID
		  {
		   	$$ = new symbol_info($1->getname(),"ID");
		   	func_name = $1->getname();
		   	func_ret_type = ret_type;
		  }
 		  ;

statements : statement
	   {
	    	outlog<<"At line no: "<<lines<<" statements : statement "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"stmnts");
			
			// Create block for statements
			BlockNode* block = new BlockNode();
			if($1->get_ast_node()) {
				block->add_statement((StmtNode*)$1->get_ast_node());
			}
			$$->set_ast_node(block);
	   }
	   | statements statement
	   {
	    	outlog<<"At line no: "<<lines<<" statements : statements statement "<<endl<<endl;
			outlog<<$1->getname()<<"\n"<<$2->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+"\n"+$2->getname(),"stmnts");
			
			// Update block with new statement
			BlockNode* block = (BlockNode*)$1->get_ast_node();
			if($2->get_ast_node()) {
				block->add_statement((StmtNode*)$2->get_ast_node());
			}
			$$->set_ast_node(block);
	   }
	   | error
	   {
	  		$$ = new symbol_info("","stmnts");
			BlockNode* block = new BlockNode();
			$$->set_ast_node(block);
	   }  
	   | statements error
	   {
	   		$$ = new symbol_info($1->getname(),"stmnts");
			$$->set_ast_node($1->get_ast_node());
	   }
	   ;
	   
statement : var_declaration
	  {
	    	outlog<<"At line no: "<<lines<<" statement : var_declaration "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"stmnt");
			$$->set_ast_node($1->get_ast_node());
	  }
	  | func_definition
	  {
	  		outlog<<"At line no: "<<lines<<" Function definition must be in the global scope "<<endl<<endl;
	  		outerror<<"At line no: "<<lines<<" Function definition must be in the global scope "<<endl<<endl;
	  		errors++;
	  		$$ = new symbol_info("","stmnt");
	  		
	  }
	  | expression_statement
	  {
	    	outlog<<"At line no: "<<lines<<" statement : expression_statement "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"stmnt");
			$$->set_ast_node($1->get_ast_node());
	  }
	  | compound_statement
	  {
	    	outlog<<"At line no: "<<lines<<" statement : compound_statement "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"stmnt");
			$$->set_ast_node($1->get_ast_node());
	  }
	  | FOR LPAREN expression_statement expression_statement expression RPAREN statement
	  {
	    	outlog<<"At line no: "<<lines<<" statement : FOR LPAREN expression_statement expression_statement expression RPAREN statement "<<endl<<endl;
			outlog<<"for("<<$3->getname()<<$4->getname()<<$5->getname()<<")\n"<<$7->getname()<<endl<<endl;
			
			$$ = new symbol_info("for("+$3->getname()+$4->getname()+$5->getname()+")\n"+$7->getname(),"stmnt");
			
			// Create AST node for for loop
			ForNode* forNode = new ForNode(
				(ExprNode*)$3->get_ast_node(),
				(ExprNode*)$4->get_ast_node(),
				(ExprNode*)$5->get_ast_node(),
				(StmtNode*)$7->get_ast_node()
			);
			$$->set_ast_node(forNode);
	  }
	  | IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE
	  {
	    	outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement "<<endl<<endl;
			outlog<<"if("<<$3->getname()<<")\n"<<$5->getname()<<endl<<endl;
			
			$$ = new symbol_info("if("+$3->getname()+")\n"+$5->getname(),"stmnt");
			
			// Create AST node for if statement (without else)
			IfNode* ifNode = new IfNode(
				(ExprNode*)$3->get_ast_node(),
				(StmtNode*)$5->get_ast_node()
			);
			$$->set_ast_node(ifNode);
	  }
	  | IF LPAREN expression RPAREN statement ELSE statement
	  {
	    	outlog<<"At line no: "<<lines<<" statement : IF LPAREN expression RPAREN statement ELSE statement "<<endl<<endl;
			outlog<<"if("<<$3->getname()<<")\n"<<$5->getname()<<"\nelse\n"<<$7->getname()<<endl<<endl;
			
			$$ = new symbol_info("if("+$3->getname()+")\n"+$5->getname()+"\nelse\n"+$7->getname(),"stmnt");
			
			// Create AST node for if-else statement
			IfNode* ifNode = new IfNode(
				(ExprNode*)$3->get_ast_node(),
				(StmtNode*)$5->get_ast_node(),
				(StmtNode*)$7->get_ast_node()
			);
			$$->set_ast_node(ifNode);
	  }
	  | WHILE LPAREN expression RPAREN statement
	  {
	    	outlog<<"At line no: "<<lines<<" statement : WHILE LPAREN expression RPAREN statement "<<endl<<endl;
			outlog<<"while("<<$3->getname()<<")\n"<<$5->getname()<<endl<<endl;
			
			$$ = new symbol_info("while("+$3->getname()+")\n"+$5->getname(),"stmnt");
			
			// Create AST node for while loop
			WhileNode* whileNode = new WhileNode(
				(ExprNode*)$3->get_ast_node(),
				(StmtNode*)$5->get_ast_node()
			);
			$$->set_ast_node(whileNode);
	  }
	  | PRINTLN LPAREN id_name RPAREN SEMICOLON
	  {
	    	outlog<<"At line no: "<<lines<<" statement : PRINTLN LPAREN ID RPAREN SEMICOLON "<<endl<<endl;
			outlog<<"printf("<<$3->getname()<<");"<<endl<<endl; 
			
			if(symtbl->Lookup_in_table($3->getname()) == NULL)
			{
				outerror<<"At line no: "<<lines<<" Undeclared variable "<<$3->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" Undeclared variable "<<$3->getname()<<endl<<endl;
				errors++;
			}
			
			$$ = new symbol_info("printf("+$3->getname()+");","stmnt");
			
			// Could add a PrintNode to AST if needed
			// For now, create a basic expression statement
			VarNode* var = new VarNode($3->getname(), 
			                         symtbl->Lookup_in_table($3->getname()) ? 
			                         symtbl->Lookup_in_table($3->getname())->getvartype() : "error");
			ExprStmtNode* printNode = new ExprStmtNode(var);
			$$->set_ast_node(printNode);
	  }
	  | RETURN expression SEMICOLON
	  {
	    	outlog<<"At line no: "<<lines<<" statement : RETURN expression SEMICOLON "<<endl<<endl;
			outlog<<"return "<<$2->getname()<<";"<<endl<<endl;
			
			$$ = new symbol_info("return "+$2->getname()+";","stmnt");
			
			// Create AST node for return statement
			ReturnNode* returnNode = new ReturnNode((ExprNode*)$2->get_ast_node());
			$$->set_ast_node(returnNode);
	  }
	  ;
	  
expression_statement : SEMICOLON
			{
				outlog<<"At line no: "<<lines<<" expression_statement : SEMICOLON "<<endl<<endl;
				outlog<<";"<<endl<<endl;
				
				$$ = new symbol_info(";","expr_stmt");
				
				// Create empty expression statement
				ExprStmtNode* exprStmt = new ExprStmtNode(nullptr);
				$$->set_ast_node(exprStmt);
	        }			
			| expression SEMICOLON 
			{
				outlog<<"At line no: "<<lines<<" expression_statement : expression SEMICOLON "<<endl<<endl;
				outlog<<$1->getname()<<";"<<endl<<endl;
				
				$$ = new symbol_info($1->getname()+";","expr_stmt");
				
				// Create expression statement from expression
				ExprStmtNode* exprStmt = new ExprStmtNode((ExprNode*)$1->get_ast_node());
				$$->set_ast_node(exprStmt);
	        }
			;
	  
variable : id_name 	
      {
	    outlog<<"At line no: "<<lines<<" variable : ID "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
			
		$$ = new symbol_info($1->getname(),"varbl");
		
		if(symtbl->Lookup_in_table($1->getname()) == NULL)
		{
			outerror<<"At line no: "<<lines<<" Undeclared variable "<<$1->getname()<<endl<<endl;
			outlog<<"At line no: "<<lines<<" Undeclared variable "<<$1->getname()<<endl<<endl;
			errors++;
			
			$$->setvartype("error");; //not found set error type
		}
		else if((symtbl->Lookup_in_table($1->getname()))->getidtype() != "var") //variable is not a normal variable
		{
			if((symtbl->Lookup_in_table($1->getname()))->getidtype() == "array")
			{
				outerror<<"At line no: "<<lines<<" variable is of array type : "<<$1->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" variable is of array type : "<<$1->getname()<<endl<<endl;
				errors++;
			}
			else if((symtbl->Lookup_in_table($1->getname()))->getidtype() == "func_def") 
			{
				outerror<<"At line no: "<<lines<<" variable is of function type : "<<$1->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" variable is of function type : "<<$1->getname()<<endl<<endl;
				errors++;
			}
			else if((symtbl->Lookup_in_table($1->getname()))->getidtype() == "func_dec") 
			{
				outerror<<"At line no: "<<lines<<" variable is of function type : "<<$1->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" variable is of function type : "<<$1->getname()<<endl<<endl;
				errors++;
			}
			
			
			$$->setvartype("error");; //doesnt match set error type
		}
		else $$->setvartype((symtbl->Lookup_in_table($1->getname()))->getvartype());  //set variable type as id type
		
		// Create AST node for variable
		VarNode* varNode = new VarNode($1->getname(), $$->getvartype());
		$$->set_ast_node(varNode);
	 }	
	 | id_name LTHIRD expression RTHIRD 
	 {
	 	outlog<<"At line no: "<<lines<<" variable : ID LTHIRD expression RTHIRD "<<endl<<endl;
		outlog<<$1->getname()<<"["<<$3->getname()<<"]"<<endl<<endl;
		
		$$ = new symbol_info($1->getname()+"["+$3->getname()+"]","varbl");
		
		if(symtbl->Lookup_in_table($1->getname()) == NULL)
		{
			outerror<<"At line no: "<<lines<<" Undeclared variable "<<$1->getname()<<endl<<endl;
			outlog<<"At line no: "<<lines<<" Undeclared variable "<<$1->getname()<<endl<<endl;
			errors++;
			
			$$->setvartype("error");; //not found set error type
		}
		else if((symtbl->Lookup_in_table($1->getname()))->getidtype() != "array") //variable is not an array
		{
			outerror<<"At line no: "<<lines<<" variable is not of array type : "<<$1->getname()<<endl<<endl;
			outlog<<"At line no: "<<lines<<" variable is not of array type : "<<$1->getname()<<endl<<endl;
			errors++;
			
			$$->setvartype("error");; //doesnt match set error type
		}
		else if($3->getvartype()!="int") // get type of expression of array index
		{
			outerror<<"At line no: "<<lines<<" array index is not of integer type : "<<$1->getname()<<endl<<endl;
			outlog<<"At line no: "<<lines<<" array index is not of integer type : "<<$1->getname()<<endl<<endl;
			errors++;
			
			$$->setvartype("error");
		}
		else
		{
			$$->setvartype((symtbl->Lookup_in_table($1->getname()))->getvartype());
		}
		
		// Create AST node for array access
		VarNode* varNode = new VarNode($1->getname(), $$->getvartype(), (ExprNode*)$3->get_ast_node());
		$$->set_ast_node(varNode);
	 }
	 ;
	 
expression : logic_expression //expr can be void
	   {
	    	outlog<<"At line no: "<<lines<<" expression : logic_expression "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"expr");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
	   }
	   | variable ASSIGNOP logic_expression 	
	   {
	    	outlog<<"At line no: "<<lines<<" expression : variable ASSIGNOP logic_expression "<<endl<<endl;
			outlog<<$1->getname()<<"="<<$3->getname()<<endl<<endl;

			$$ = new symbol_info($1->getname()+"="+$3->getname(),"expr");
			$$->setvartype($1->getvartype());
			
			if($1->getvartype() == "void" || $3->getvartype() == "void") //if any of them is a void
			{
				outerror<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			else if($1->getvartype() == "int" && $3->getvartype() == "float") // assignment of float into int
			{
				outerror<<"At line no: "<<lines<<" Warning: Assignment of float value into variable of integer type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" Warning: Assignment of float value into variable of integer type "<<endl<<endl;
				errors++;
				
				$$->setvartype("int");
			}
			
			if($1->getvartype() == "error" || $3->getvartype() == "error") //if any of them is a error
			{
				$$->setvartype("error");
			}
			
			// Create AST node for assignment
			AssignNode* assignNode = new AssignNode(
				(VarNode*)$1->get_ast_node(),
				(ExprNode*)$3->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(assignNode);
	   }
	   ;
			
logic_expression : rel_expression //lgc_expr can be void
	     {
	    	outlog<<"At line no: "<<lines<<" logic_expression : rel_expression "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"lgc_expr");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
	     }	
		 | rel_expression LOGICOP rel_expression 
		 {
	    	outlog<<"At line no: "<<lines<<" logic_expression : rel_expression LOGICOP rel_expression "<<endl<<endl;
			outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+$2->getname()+$3->getname(),"lgc_expr");
			$$->setvartype("int");
			
			//do type checking of both side of logicop
			
			if($1->getvartype() == "void" || $3->getvartype() == "void") //if any of them is a void
			{
				outerror<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			
			if($1->getvartype() == "error" || $3->getvartype() == "error") //if any of them is a error
			{
				$$->setvartype("error");
			}
			
			// Create AST node for logical operation
			BinaryOpNode* logicNode = new BinaryOpNode(
				$2->getname(),
				(ExprNode*)$1->get_ast_node(),
				(ExprNode*)$3->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(logicNode);
	     }	
		 ;
			
rel_expression	: simple_expression //rel_expr can be void
		{
	    	outlog<<"At line no: "<<lines<<" rel_expression : simple_expression "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"rel_expr");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
	    }
		| simple_expression RELOP simple_expression
		{
	    	outlog<<"At line no: "<<lines<<" rel_expression : simple_expression RELOP simple_expression "<<endl<<endl;
			outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+$2->getname()+$3->getname(),"rel_expr");
			$$->setvartype("int");
			
			//do type checking of both side of relop
			
			if($1->getvartype() == "void" || $3->getvartype() == "void") //if any of them is a void
			{
				outerror<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			
			if($1->getvartype() == "error" || $3->getvartype() == "error") //if any of them is a error
			{
				$$->setvartype("error");
			}
			
			// Create AST node for relational operation
			BinaryOpNode* relNode = new BinaryOpNode(
				$2->getname(),
				(ExprNode*)$1->get_ast_node(),
				(ExprNode*)$3->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(relNode);
	    }
		;
				
simple_expression : term //simp_expr can be void
          {
	    	outlog<<"At line no: "<<lines<<" simple_expression : term "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"simp_expr");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
			
	      }
		  | simple_expression ADDOP term 
		  {
	    	outlog<<"At line no: "<<lines<<" simple_expression : simple_expression ADDOP term "<<endl<<endl;
			outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+$2->getname()+$3->getname(),"simp_expr");
			$$->setvartype($1->getvartype());
			
			//do type checking of both side of addop
			
			if($1->getvartype() == "void" || $3->getvartype() == "void") //if any of them is a void
			{
				outerror<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			else if($1->getvartype() == "float" || $3->getvartype() == "float") //if any of them is a float
			{
				$$->setvartype("float");
			}
			else $$->setvartype("int");
			
			if($1->getvartype() == "error" || $3->getvartype() == "error") //if any of them is a error
			{
				$$->setvartype("error");
			}
			
			// Create AST node for addition/subtraction
			BinaryOpNode* addopNode = new BinaryOpNode(
				$2->getname(),
				(ExprNode*)$1->get_ast_node(),
				(ExprNode*)$3->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(addopNode);
	      }
		  ;
					
term :	unary_expression //term can be void because of un_expr->factor
     {
	    	outlog<<"At line no: "<<lines<<" term : unary_expression "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"term");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
			
	 }
     |  term MULOP unary_expression
     {
	    	outlog<<"At line no: "<<lines<<" term : term MULOP unary_expression "<<endl<<endl;
			outlog<<$1->getname()<<$2->getname()<<$3->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+$2->getname()+$3->getname(),"term");
			$$->setvartype($1->getvartype());
			
			//do type checking of both side of mulop
			if($1->getvartype() == "void" || $3->getvartype() == "void") //if any of them is a void
			{
				outerror<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type "<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			else if($1->getvartype() == "float" || $3->getvartype() == "float") //if any of them is a float
			{
				$$->setvartype("float");
			}
			else $$->setvartype("int");
			
			//check if both int for modulous
			if($2->getname() == "%")
			{
				if($1->getvartype() == "int" && $3->getvartype() == "int")
				{
					if($3->getname()=="0")
					{
						outerror<<"At line no: "<<lines<<" Modulus by 0 "<<endl<<endl;
						outlog<<"At line no: "<<lines<<" Modulus by 0 "<<endl<<endl;
						errors++;
						
						$$->setvartype("error");
					}
					else $$->setvartype("int");
				}
				else if($1->getvartype() == "float" || $3->getvartype() == "float")
				{
					outerror<<"At line no: "<<lines<<" Modulus operator on non integer type "<<endl<<endl;
					outlog<<"At line no: "<<lines<<" Modulus operator on non integer type "<<endl<<endl;
					errors++;
					
					$$->setvartype("error");
				}
			}
			
			if($2->getname() == "/") //divide by 0
			{
				if($3->getname()=="0")
				{
					outerror<<"At line no: "<<lines<<" Divide by 0 "<<endl<<endl;
					outlog<<"At line no: "<<lines<<" Divide by 0 "<<endl<<endl;
					errors++;
					
					$$->setvartype("error");
				}
			}
			if($1->getvartype() == "error" || $3->getvartype() == "error") //if any of them is a error
			{
				$$->setvartype("error");
			}
			
			// Create AST node for multiplication/division/modulus
			BinaryOpNode* mulopNode = new BinaryOpNode(
				$2->getname(),
				(ExprNode*)$1->get_ast_node(),
				(ExprNode*)$3->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(mulopNode);
	 }
     ;

unary_expression : ADDOP unary_expression  // un_expr can be void because of factor
		 {
	    	outlog<<"At line no: "<<lines<<" unary_expression : ADDOP unary_expression "<<endl<<endl;
			outlog<<$1->getname()<<$2->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname()+$2->getname(),"un_expr");
			$$->setvartype($2->getvartype());
			
			if($2->getvartype()=="void")
			{
				outerror<<"At line no: "<<lines<<" operation on void type : "<<$2->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type : "<<$2->getname()<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			
			// Create AST node for unary plus/minus
			UnaryOpNode* unaryNode = new UnaryOpNode(
				$1->getname(),
				(ExprNode*)$2->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(unaryNode);
	     }
		 | NOT unary_expression 
		 {
	    	outlog<<"At line no: "<<lines<<" unary_expression : NOT unary_expression "<<endl<<endl;
			outlog<<"!"<<$2->getname()<<endl<<endl;
			
			$$ = new symbol_info("!"+$2->getname(),"un_expr");
			$$->setvartype("int");
			
			if($2->getvartype()=="void")
			{
				outerror<<"At line no: "<<lines<<" operation on void type : "<<$2->getname()<<endl<<endl;
				outlog<<"At line no: "<<lines<<" operation on void type : "<<$2->getname()<<endl<<endl;
				errors++;
				
				$$->setvartype("error");
			}
			
			// Create AST node for logical NOT
			UnaryOpNode* notNode = new UnaryOpNode(
				"!",
				(ExprNode*)$2->get_ast_node(),
				$$->getvartype()
			);
			$$->set_ast_node(notNode);
	     }
		 | factor 
		 {
	    	outlog<<"At line no: "<<lines<<" unary_expression : factor "<<endl<<endl;
			outlog<<$1->getname()<<endl<<endl;
			
			$$ = new symbol_info($1->getname(),"un_expr");
			$$->setvartype($1->getvartype());
			$$->set_ast_node($1->get_ast_node());
			
			//outlog<<$1->getvartype()<<endl;
	     }
		 ;
	
factor	: variable  // factor can be void
    {
	    outlog<<"At line no: "<<lines<<" factor : variable "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
			
		$$ = new symbol_info($1->getname(),"fctr");
		$$->setvartype($1->getvartype());
		$$->set_ast_node($1->get_ast_node());
	}
	| id_name LPAREN argument_list RPAREN
	{
	    outlog<<"At line no: "<<lines<<" factor : ID LPAREN argument_list RPAREN "<<endl<<endl;
	    outlog<<$1->getname()<<"("<<$3->getname()<<")"<<endl<<endl;
	
	    $$ = new symbol_info($1->getname()+"("+$3->getname()+")","fctr");
	    $$->setvartype("error");
	
	    int flag = 0;
	
	    // Type checking (existing code)
	    if(symtbl->Lookup_in_table($1->getname())==NULL) //undeclared function
	    {
	        outerror<<"At line no: "<<lines<<" Undeclared function: "<<$1->getname()<<endl<<endl;
	        outlog<<"At line no: "<<lines<<" Undeclared function: "<<$1->getname()<<endl<<endl;
	        errors++;
	    }
	    else
	    {
	        if((symtbl->Lookup_in_table($1->getname()))->getidtype()=="func_dec") //declared but not defined
	        {
	            outerror<<"At line no: "<<lines<<" Undefined function: "<<$1->getname()<<endl<<endl;
	            outlog<<"At line no: "<<lines<<" Undefined function: "<<$1->getname()<<endl<<endl;
	            errors++;
	        }
	        else if((symtbl->Lookup_in_table($1->getname()))->getidtype()=="func_def")
	        {
	            vector<string> templist = (symtbl->Lookup_in_table($1->getname()))->getparamlist();
	
	            if(arglist.size()!=templist.size()) //number of prameters don't match
	            {
	                outerror<<"At line no: "<<lines<<" Inconsistencies in number of arguments in function call: "<<$1->getname()<<endl<<endl;
	                outlog<<"At line no: "<<lines<<" Inconsistencies in number of arguments in function call: "<<$1->getname()<<endl<<endl;
	                errors++;
	            }
	            else if(templist.size()!=0)
	            {
	                for(int i = 0; i < templist.size(); i++)
	                {
	                    if(arglist[i]!=templist[i])
	                    {
	                        if(arglist[i] == "int" && templist[i] == "float") {}
	                        else if(arglist[i]!="error")
	                        {
	                            flag = 1;
	                            outerror<<"At line no: "<<lines<<" "<<"argument "<<i+1<<" type mismatch in function call: "<<$1->getname()<<endl<<endl;
	                            outlog<<"At line no: "<<lines<<" "<<"argument "<<i+1<<" type mismatch in function call: "<<$1->getname()<<endl<<endl;
	                            errors++;
	                        }
	                    }
	                }                   
	            }
	            if(!flag) $$->setvartype((symtbl->Lookup_in_table($1->getname()))->getvartype());
	        }
	    }
	
	    // Create function call node
	    FuncCallNode* funcCall = new FuncCallNode($1->getname(), $$->getvartype());
	
	    // Get arguments from the ArgumentsNode if it exists
	    if ($3->get_ast_node()) {
	        ArgumentsNode* argsNode = dynamic_cast<ArgumentsNode*>($3->get_ast_node());
	        if (argsNode) {
	            // Add each argument to the function call
	            for (auto arg : argsNode->get_arguments()) {
	                funcCall->add_argument(arg);
	            }
	        }
	    }
	
	    $$->set_ast_node(funcCall);
	
	    arglist.clear();
	}
	| LPAREN expression RPAREN
	{
	   	outlog<<"At line no: "<<lines<<" factor : LPAREN expression RPAREN "<<endl<<endl;
		outlog<<"("<<$2->getname()<<")"<<endl<<endl;
		
		$$ = new symbol_info("("+$2->getname()+")","fctr");
		$$->setvartype($2->getvartype());
		$$->set_ast_node($2->get_ast_node()); // Pass through the expression AST
	}
	| CONST_INT 
	{
	    outlog<<"At line no: "<<lines<<" factor : CONST_INT "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
			
		$$ = new symbol_info($1->getname(),"fctr");
		$$->setvartype("int");
		
		// Create AST node for integer constant
		ConstNode* intNode = new ConstNode($1->getname(), "int");
		$$->set_ast_node(intNode);
	}
	| CONST_FLOAT
	{
	    outlog<<"At line no: "<<lines<<" factor : CONST_FLOAT "<<endl<<endl;
		outlog<<$1->getname()<<endl<<endl;
			
		$$ = new symbol_info($1->getname(),"fctr");
		$$->setvartype("float");
		
		// Create AST node for float constant
		ConstNode* floatNode = new ConstNode($1->getname(), "float");
		$$->set_ast_node(floatNode);
	}
	| variable INCOP 
	{
	    outlog<<"At line no: "<<lines<<" factor : variable INCOP "<<endl<<endl;
		outlog<<$1->getname()<<"++"<<endl<<endl;
			
		$$ = new symbol_info($1->getname()+"++","fctr");
		$$->setvartype($1->getvartype());
		
		// Create AST nodes for increment
		// For x++, equivalent to (x = x + 1)
		VarNode* varNode = (VarNode*)$1->get_ast_node();
		ConstNode* oneNode = new ConstNode("1", "int");
		BinaryOpNode* addNode = new BinaryOpNode("+", varNode, oneNode, $1->getvartype());
		AssignNode* assignNode = new AssignNode(varNode, addNode, $1->getvartype());
		$$->set_ast_node(assignNode);
	}
	| variable DECOP
	{
	    outlog<<"At line no: "<<lines<<" factor : variable DECOP "<<endl<<endl;
		outlog<<$1->getname()<<"--"<<endl<<endl;
			
		$$ = new symbol_info($1->getname()+"--","fctr");
		$$->setvartype($1->getvartype());
		
		// Create AST nodes for decrement
		// For x--, equivalent to (x = x - 1)
		VarNode* varNode = (VarNode*)$1->get_ast_node();
		ConstNode* oneNode = new ConstNode("1", "int");
		BinaryOpNode* subNode = new BinaryOpNode("-", varNode, oneNode, $1->getvartype());
		AssignNode* assignNode = new AssignNode(varNode, subNode, $1->getvartype());
		$$->set_ast_node(assignNode);
	}
	;
	
argument_list : arguments
              {
                    outlog<<"At line no: "<<lines<<" argument_list : arguments "<<endl<<endl;
                    outlog<<$1->getname()<<endl<<endl;
                        
                    $$ = $1; // Pass through the arguments node
              }
              |
              {
                    outlog<<"At line no: "<<lines<<" argument_list :  "<<endl<<endl;
                    outlog<<""<<endl<<endl;
                        
                    $$ = new symbol_info("","arg_list");
                    // Create empty arguments node
                    ArgumentsNode* args = new ArgumentsNode();
                    $$->set_ast_node(args);
              }
              ;
    
arguments : arguments COMMA logic_expression
          {
                outlog<<"At line no: "<<lines<<" arguments : arguments COMMA logic_expression "<<endl<<endl;
                outlog<<$1->getname()<<","<<$3->getname()<<endl<<endl;
                        
                $$ = new symbol_info($1->getname()+","+$3->getname(),"arg");
                
                // Get existing arguments node or create new one
                ArgumentsNode* args;
                if ($1->get_ast_node()) {
                    args = dynamic_cast<ArgumentsNode*>($1->get_ast_node());
                } else {
                    args = new ArgumentsNode();
                }
                
                // Add the new argument
                if ($3->get_ast_node()) {
                    args->add_argument(dynamic_cast<ExprNode*>($3->get_ast_node()));
                }
                
                $$->set_ast_node(args);
                arglist.push_back($3->getvartype());
          }
          | logic_expression
          {
                outlog<<"At line no: "<<lines<<" arguments : logic_expression "<<endl<<endl;
                outlog<<$1->getname()<<endl<<endl;
                        
                $$ = new symbol_info($1->getname(),"arg");
                
                // Create a new arguments node with single argument
                ArgumentsNode* args = new ArgumentsNode();
                if ($1->get_ast_node()) {  // FIXED: Changed from $3 to $1
                    args->add_argument(dynamic_cast<ExprNode*>($1->get_ast_node()));
                }
                
                $$->set_ast_node(args);
                arglist.push_back($1->getvartype());
          }
          ;
 

%%

int main(int argc, char *argv[])
{
	if(argc != 2) 
	{
		cout<<"Please input file name"<<endl;
		return 0;
	}
	yyin = fopen(argv[1], "r");
	outlog.open("log.txt", ios::trunc);
	outerror.open("error.txt", ios::trunc);
	outcode.open("code.txt", ios::trunc);
	
	if(yyin == NULL)
	{
		cout<<"Couldn't open file"<<endl;
		return 0;
	}
	
	// First pass: Parse the input and build AST
	cout << "==== Pass 1: Parsing input and building AST ====" << endl;
	outlog << "==== Pass 1: Parsing input and building AST ====" << endl;
	
	symtbl->enter_scope(outlog);
	yyparse();
	
	outlog << endl << "Symbol Table after first pass:" << endl;
	symtbl->Print_all_scope(outlog);
	
	// Only proceed to second pass if no errors
	if (errors == 0 && ast_root) {
		cout << "==== Pass 2: Generating Three-Address Code from AST ====" << endl;
		outlog << endl << "==== Pass 2: Generating Three-Address Code from AST ====" << endl;
		
		// Generate three-address code (second pass)
		outlog << "Generating Three-Address Code..." << endl;
		ThreeAddrCodeGenerator tacGen(ast_root, outcode);
		tacGen.generate();
		
		outlog << "Three-Address Code Generation Complete" << endl;
		cout << "Three-Address Code Generation Complete. Output written to code.txt" << endl;
	} else {
		cout << "Three-Address Code generation skipped due to errors" << endl;
		outlog << endl << "Three-Address Code generation skipped due to errors" << endl;
		outcode << "// Three-Address Code generation failed due to errors" << endl;
	}
	
	outlog<<endl<<"Total lines: "<<lines<<endl;
	outlog<<"Total errors: "<<errors<<endl;
	outerror<<"Total errors: "<<errors<<endl;
	
	outlog.close();
	outerror.close();
	outcode.close();
	
	fclose(yyin);
	
	return 0;
}