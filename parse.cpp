/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include "parse.h"
#include <vector>
#include <algorithm>

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

static bool semiMissingFlag = false;
static bool parError = false;

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
    if (!status) {
        ParseError(line, "Incorrect Declaration Section.");
        return false;
    }
    //Read in Program Body
    status = ProgBody(in,line);
    if (!status) {
        if (semiMissingFlag) {
            ParseError(line - 1, "Incorrect Program Body.");
            return false;
        }
        if (parError) {
            ParseError(line -1, "Incorrect Program Body.");
            return false;
        }
        ParseError(line, "Incorrect Program Body.");
        return false;
    }
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
            return false;
        }

        t = Parser::GetNextToken(in, line);
        if (t == IDENT) {
            Parser::PushBackToken(t);
        }

    }

    return status;
}

bool DeclStmt(istream& in, int& line) {
    LexItem t = Parser::GetNextToken(in,line);

    while (t != COLON) {
        if (t != COMMA) {
            if (defVar.count(t.GetLexeme()) >= 1) {
                ParseError(line, "Variable Redefinition");
                ParseError(line, "Incorrect variable in Declaration Statement.");
                return false;
            } else {
                defVar.insert({t.GetLexeme(), false});
                t = Parser::GetNextToken(in,line);
                if (t != COMMA && t != COLON) {
                    ParseError(line, "Unrecognized Input Pattern");
                    cout << '(' << t.GetLexeme() << ')' << endl;
                    ParseError(line, "Incorrect variable in Declaration Statement.");
                    return false;
                }
                Parser::PushBackToken(t);
            }
        }
        t = Parser::GetNextToken(in,line);
    }

    //Read in type
    t = Parser::GetNextToken(in,line);

    if (t != INTEGER && t!= REAL && t != STRING) {
        ParseError(line, "Incorrect Declaration Type.");
        return false;
    }

    //Read in semicolon at the end of stmt
    t = Parser::GetNextToken(in,line);
    return true;
}

bool ProgBody(istream& in, int& line) {
    bool status;
    LexItem t = Parser::GetNextToken(in,line);
    while (t != END && t != DONE) {
        Parser::PushBackToken(t);

        //Read in statement
        status = Stmt(in,line);
        if (!status) {
            if (parError) {
                ParseError(line -1, "Syntactic error in Declaration Block.");
                return false;
            }
            ParseError(line, "Syntactic error in Program Body.");
            return false;
        }

        //Read in semicolon
        t = Parser::GetNextToken(in,line);
        if (t != SEMICOL) {
            ParseError(line - 1, "Missing semicolon in Statement.");
            semiMissingFlag = true;
            return false;
        }
        t = Parser::GetNextToken(in,line);
    }

    if (t == END) {
        return true;
    } else if (t == DONE) {
        ParseError(line, "Syntactic error in Program Body.");
        return false;
    }

    return status;
}

//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt
bool Stmt(istream& in, int& line) {
    bool status = true;
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
    bool status = true;

    //Read in LPAREN
    LexItem t = Parser::GetNextToken(in, line);
    if(t != LPAREN) {
        ParseError(line, "Missing LPAREN.");
        status = false;
    }

    //Read in the logical expression
    status = LogicExpr(in, line);

    //Read in the RPAREN
    t = Parser::GetNextToken(in, line);
    if(t != RPAREN) {
        ParseError(line, "Missing RPAREN.");
        status = false;
    }

    //Read in the THEN
    t = Parser::GetNextToken(in, line);
    if(t != THEN) {
        ParseError(line, "Missing THEN.");
        status = false;
    }

    //Read in the statement
    status = Stmt(in, line);
    if (!status) {
        if (parError) {
            ParseError(line - 1, "Missing Statement for If-Stmt Then-Part");
            return false;
        }
        ParseError(line, "Missing Statement for If-Stmt Then-Part");
        status = false;
    }

    //Read in the else
    t = Parser::GetNextToken(in, line);
    if(t == ELSE) {
        status = Stmt(in, line);
    } else {
        Parser::PushBackToken(t);
    }

    return status;
}

bool ForStmt(istream& in, int& line) {
    bool status = false;

    //Read in var
    status = Var(in,line);

    //Read in assop
    LexItem t = Parser::GetNextToken(in,line);

    //Read in iconst
    t = Parser::GetNextToken(in,line);

    //Read in TO | DOWNTO
    t = Parser::GetNextToken(in,line);
    if (t != TO && t!= DOWNTO) {
        ParseError(line, "For Statement Syntax Error");
        return false;
    }

    //Read in iconst
    t = Parser::GetNextToken(in,line);
    if (t != ICONST) {
        ParseError(line, "Missing Termination Value in For Statement.");
        return false;
    }

    //Read in Do
    t = Parser::GetNextToken(in,line);

    //Read in stmt
    status = Stmt(in,line);

    return status;
}

bool AssignStmt(istream& in, int& line) {
    bool status = false;
    //Read in the VAR/IDENT
    status = Var(in,line);
    if (!status) {
        ParseError(line,"Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }

    //Read in the ASSOP
    LexItem t = Parser::GetNextToken(in,line);
    if (t != ASSOP) {
        ParseError(line, "Missing Assignment Operator");
        return false;
    }

    //Read in the Assignment Expression
    status = Expr(in, line);
    if (!status) {
        if (parError) {
            ParseError(line - 1, "Missing Expression in Assignment Statment");
            return false;
        }
        ParseError(line, "Missing Expression in Assignment Statment");
        return false;
    }
    return status;
}

bool Var(istream& in, int& line) {
    //Read in the variable
    LexItem t = Parser::GetNextToken(in,line);
    if (t != IDENT) {
        ParseError(line, "Var() did not read in an IDENT");
        return false;
    }

    if (defVar.count(t.GetLexeme()) == 0) {
        ParseError(line, "Undeclared Variable");
        return false;
    }

    return true;
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
    bool status = false;
    status = Expr(in,line);

    LexItem t = Parser::GetNextToken(in,line);

    if (t != EQUAL && t != GTHAN && t != LTHAN) {
        ParseError(line, "Missing expression after relational operator");
        return false;
    }

    status = Expr(in,line);

    return status;
}

bool Expr(istream& in, int& line) {
    bool status = false;
    status = Term(in, line);

    LexItem t = Parser::GetNextToken(in,line);
    if (t == PLUS || t == MINUS) {
        return Expr(in,line);
    } else {
        Parser::PushBackToken(t);
        return status;
    }
}

bool Term(istream& in, int& line) {
    bool status = false;
    status = SFactor(in, line);

    LexItem t = Parser::GetNextToken(in,line);
    if (t == MULT || t == DIV) {
        return Term(in,line);
    } else {
        Parser::PushBackToken(t);
        return status;
    }
    return false;
}

bool SFactor(istream& in, int& line) {
    bool status = false;
    LexItem t = Parser::GetNextToken(in, line);
    if (t == MINUS) {
        return Factor(in, line, 0);
    } else if (t == PLUS) {
        return Factor(in,line, 1);
    } else {
        Parser::PushBackToken(t);
        return Factor(in,line, 1);
    }
    return status;
}

bool Factor(istream& in, int& line, int sign) {
    LexItem t = Parser::GetNextToken(in,line);

    if (t == IDENT) {
        return true;
    } else if (t == ICONST) {
        return true;
    } else if (t == RCONST) {
        return true;
    } else if (t == SCONST) {
        return true;
    } else if (t == LPAREN) {
        bool status = Expr(in,line);
        if (!status) {
            ParseError(line, "Error in Nested Expression");
            return false;
        }

        t = Parser::GetNextToken(in,line);
        if (t != RPAREN) {
            ParseError(line, "Missing ) after expression");
            parError = true;
            return false;
        }
    } else {
        ParseError(line, "Error in factor statement");
        return false;
    }
    return false;
}