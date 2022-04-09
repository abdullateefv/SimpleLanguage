/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include "parse.h"
#include <vector>

map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool Prog(istream& in, int& line) {
    bool status = true;

    //Read in Program token
    LexItem t = Parser::GetNextToken(in,line);
    if (t != PROGRAM) {
        ParseError(line, "Missing PROGRAM.");
        return false;
    }

    //Read in Program name
    t = Parser::GetNextToken(in,line);
    if (t != IDENT) {
        ParseError(line, "Missing Program Name");
        return false;
    }
    defVar.insert({t.GetLexeme(), true});

    //Read in semicolon
    t = Parser::GetNextToken(in,line);

    //Read in Declaration Block
    status = DeclBlock(in,line);
    if (!status)
        ParseError(line, "Incorrect Declaration Section.");

    return status;
}

bool DeclBlock(istream& in, int& line) {
    bool status;

    //Read in VAR Start of DeclBlock
    LexItem t = Parser::GetNextToken(in,line);
    if (t != VAR) {
        ParseError(line, "Non-recognizable Declaration Block.");
        return false;
    }

    //Read in Declaration statements
    while (t != BEGIN) {
        status = DeclStmt(in, line);
        if (!status) {
            ParseError(line, "Syntactic error in Declaration Block.");
            break;
        }
        t = Parser::GetNextToken(in, line);
    }

    return status;
}
bool DeclStmt(istream& in, int& line) {
    vector<string> identifierList;
    LexItem t = Parser::GetNextToken(in,line);
    while (t != COLON) {
        if (t != COMMA) {
            identifierList.push_back(t.GetLexeme());
            cout << t.GetLexeme() << endl;
        }
        t = Parser::GetNextToken(in,line);
    }

    if (t != INTEGER && t!= REAL && t != STRING) {
        ParseError(line, "Incorrect Declaration Type.");
        return false;
    }

    //Read in semicolon at the end of stmt
    Parser::GetNextToken(in,line);
    return true;
}
bool ProgBody(istream& in, int& line) {
    return false;
}
//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream& in, int& line) {
    bool status;
    //cout << "in ContrlStmt" << endl;
    LexItem t = Parser::GetNextToken(in, line);

    switch( t.GetToken() ) {

        case WRITELN:
            status = WriteLnStmt(in, line);
            //cout << "After WriteStmet status: " << (status? true:false) <<endl;
            break;

        case IF:
            status = IfStmt(in, line);
            break;

        case IDENT:
            Parser::PushBackToken(t);
            status = AssignStmt(in, line);

            break;

        case FOR:
            status = ForStmt(in, line);

            break;


        default:
            Parser::PushBackToken(t);
            return false;
    }

    return status;
}//End of Stmt
//WriteStmt:= wi, ExpreList
bool WriteLnStmt(istream& in, int& line) {
    LexItem t;
    //cout << "in WriteStmt" << endl;

    t = Parser::GetNextToken(in, line);
    if( t != LPAREN ) {

        ParseError(line, "Missing Left Parenthesis");
        return false;
    }

    bool ex = ExprList(in, line);

    if( !ex ) {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if(t != RPAREN ) {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    //Evaluate: print out the list of expressions values

    return ex;
}
bool IfStmt(istream& in, int& line) {
    return false;
}
bool ForStmt(istream& in, int& line) {
    return false;
}
bool AssignStmt(istream& in, int& line) {
    return false;
}
bool Var(istream& in, int& line) {
    return false;
}
//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
    bool status = false;
    //cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line);
    if(!status){
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA) {
        //cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        //cout << "after calling ExprList" << endl;
    }
    else if(tok.GetToken() == ERR){
        ParseError(line, "Unrecognized Input Pattern");
        cout << "(" << tok.GetLexeme() << ")" << endl;
        return false;
    }
    else{
        Parser::PushBackToken(tok);
        return true;
    }
    return status;
}
bool LogicExpr(istream& in, int& line) {
    return false;
}
bool Expr(istream& in, int& line) {
    return false;
}
bool Term(istream& in, int& line) {
    return false;
}
bool SFactor(istream& in, int& line) {
    return false;
}
bool Factor(istream& in, int& line, int sign) {
    return false;
}




