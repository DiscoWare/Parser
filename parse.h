#include <string>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

void Driver();

struct Token
{
    Token(const string& token, const string& lexeme) {token_ = token; lexeme_ = lexeme; };
    Token() {};

    string token_;
    string lexeme_;
};

string workingString;
stack<char> Stack;
string Table[8][9] = {{"A;", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "D;", "C"},
                      {"i=E", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID"},
                      {"iF", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID"},
                      {"INVALID", "+i", "-i", "*i", "INVALID", "INVALID", "EPSILON", "INVALID", "INVALID"},
                      {"SU", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "SU", "SU"},
                      {"TU", "INVALID", "INVALID", "INVALID", "INVALID", "EPSILON", "EPSILON", "TU", "TU"},
                      {"INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "EPSILON", "INVALID", "ti=E", "INVALID"},
                      {"INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "k(ici)"}};

vector<Token> tokenVec;
vector<Token>::const_iterator currentToken;

int convertToIndex(Token token)
{
    if (token.token_ == "IDENTIFIER" || token.token_ == "INT_LITERAL")
    {
        return 0;
    }
    else if (token.lexeme_ == "+")
    {
        return 1;
    }
    else if (token.lexeme_ == "-")
    {
        return 2;
    }
    else if (token.lexeme_ == "*")
    {
        return 3;
    }
    else if (token.lexeme_ == "=")
    {
        return 4;
    }
    else if (token.lexeme_ == "$")
    {
        return 5;
    }
    else if (token.lexeme_ == ";")
    {
        return 6;
    }
    else if (token.lexeme_ == "int")
    {
        return 7;
    }
    else if (token.lexeme_ == "if" || token.lexeme_ == "while")
    {
        return 8;
    }
    return -1;
};

int convertNonTerminal(char c)
{
    if (c == 'S')
        return 0;
    else if (c == 'A')
        return 1;
    else if (c == 'E')
        return 2;
    else if (c == 'F')
        return 3;
    else if (c == 'T')
        return 4;
    else if (c == 'U')
        return 5;
    else if (c == 'D')
        return 6;
    else if (c == 'C')
        return 7;
    else
        return -1;
}

void printStack()
{
    stack<char> newStack = Stack;
    while (!Stack.empty())
    {
        cout << "| " << Stack.top() << " |" << endl;
        Stack.pop();
    }
    Stack = newStack;

}

void tokenize()
{
    Stack.push('$');
    Stack.push('T');

    ifstream in;
    in.open("output.txt");
    string str;
    Token token;
    while (in >> str)
    {
        token.token_ = str;
        in >> str;
        in >> str;
        token.lexeme_ = str;
        tokenVec.push_back(token);
    }
    in.close();
    tokenVec.push_back(Token("SEPARATOR", "$"));
    currentToken = tokenVec.begin();
}

void reversePush(const string& str)
{
    if (str == "EPSILON")
        return;
    for (int i = str.size() - 1; i >= 0; i--)
    {
        Stack.push(str[i]);
    }
}

void dumpStack()
{
    cout << "Contents of Stack:\n";
    while (!Stack.empty())
    {
        cout << Stack.top() << endl;
        Stack.pop();
    }
}

void handleTerminal(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_.size() == 1 && currentToken->lexeme_[0] == Stack.top())
    {
        Stack.pop();
        cout << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        currentToken++;
    }
    else
    {
        string stackString; 
        stackString.push_back(Stack.top());
        throw std::invalid_argument("ERROR: EXPECTED " + stackString + " BUT GOT " + currentToken->lexeme_);
    }
}

void handleIdentifier(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->token_ == "IDENTIFIER" || currentToken->token_ == "INT_LITERAL")
    {
        cout << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
    }
    else
        cout << "ERROR: EXPECTED IDENTIFIER OR LITERAL BUT RECEIVED: " << currentToken->lexeme_ << endl;
        
    Stack.pop();
    currentToken++;
}

void handleCompare(vector<Token>::const_iterator& currentToken)
{
    if (Stack.top() == 'c')
    {
        if (currentToken->lexeme_ == ">" || currentToken->lexeme_ == "<" || currentToken->lexeme_ == "==" )
            cout << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        else
            cerr << "ERROR: EXPECTED COMPARE OPERATOR BUT RECEIVED: " << currentToken->lexeme_ << endl;
    }
    else if (Stack.top() == 'k')
    {
        if (currentToken->lexeme_ == "if" || currentToken->lexeme_ == "while")
            cout << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
        else
            cerr << "ERROR: EXPECTED COMPARE KEYWORD BUT RECEIVED: " << currentToken->lexeme_ << endl;
    }
    else 
        cerr << "ERROR. EXPECTED c or k ON THE STACK\n";
    Stack.pop();
    currentToken++;

}

void handleType(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_ == "int" || currentToken->lexeme_ == "float")
        cout << "\nToken: " << currentToken->token_ << "    Lexeme: " << currentToken->lexeme_ << endl;
    else
        cout << "ERROR: EXPECTED VARIABLE TYPE BUT RECEIVED: " << currentToken->lexeme_ << endl;
    Stack.pop();
    currentToken++;
}

void finishFile(vector<Token>::const_iterator& currentToken)
{
    if (currentToken->lexeme_ == "$")
        cout << "FILE PARSED SUCCESSFULLY\n";
    else
        cout << "PARSER FAILED. EOF SYMBOL NOT ON TOP OF STACK\n";
    Stack.pop();
}

void printNonTerminalInfo()
{
    string message;
    switch (Stack.top())
    {
        case 'S': 
            message = "<Statement> -> <Assignment> | <Declaration> | <Compare>";
            break;
        case 'A': 
            message = "<Assignment> -> id = <Expression>";
            break;
        case 'E': 
            message = "<Expression> -> id <Expression Prime>";
            break;
        case 'F': 
            message = "<Expression Prime> -> +id | -id | *id | epsilon";
            break;
        case 'T': 
            message = "<Statements> -> <Statement> <Statements Prime>";
            break;
        case 'U': 
            message = "<Statements Prime> -> <Statements> <Statements Prime> | epsilon";
            break;
        case 'D': 
            message = "<Declarative> -> type id = <Expression>";
            break;
        case 'C': 
            message = "<Compare Statement> -> compare_keyword ( id compare_operator id )";
            break;
        default:
            cerr << "ERROR: NON-TERMINAL ON TOP OF STACK: " << Stack.top();
    }
    cout << "     " << message << endl;
}

void handleNonTerminal(vector<Token>::const_iterator& currentToken)
{
    printNonTerminalInfo();
    workingString = Table[convertNonTerminal(Stack.top())][convertToIndex(*currentToken)];
    if (workingString != "INVALID")
    {
        Stack.pop();
        if (workingString == "EPSILON")
        {
            return;
        }
        reversePush(workingString);
    }
    else
    {
        cout << "ERROR. GOT INVALID TABLE VALUE WHILE WORKING ON LEXEME: " << currentToken->lexeme_ 
             << ". Top of Stack: " << Stack.top() << endl;
        dumpStack();
    }
}

void Driver()
{
    while (!Stack.empty())
    {
        // printStack();
        if (Stack.top() == '+' || Stack.top() == '*' || Stack.top() == '=' || Stack.top() == '-' 
                   || Stack.top() == ';' || Stack.top() == ')' || Stack.top() == '(')
            handleTerminal(currentToken);
        else if (Stack.top() == 'i')
            handleIdentifier(currentToken);
        else if (Stack.top() == 'c' || Stack.top() == 'k')
            handleCompare(currentToken);
        else if (Stack.top() == '$')
            finishFile(currentToken);
        else if (Stack.top() == 't')
            handleType(currentToken);
        else
            handleNonTerminal(currentToken); 
    }

}