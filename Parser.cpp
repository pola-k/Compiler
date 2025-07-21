#include "Parser.h"
#include "File_Handler.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

string newTemp() 
{
    return "t" + to_string(temp_counter++);
}

string newLabel() 
{
    return "L" + to_string(label_counter++);
}

bool conIdentList(vector<Token> tokens, int& num, TreeNode* parent, vector<string>& list)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == ",")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;
        if (IdentList(tokens, num, newparent, list))
        {
            return true;
        }
        else
        {
            cout << "Syntax Error in conIdentList" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
    }
    else
    {
        num = old_num;
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        list = {};
        return true;
    }
}

bool IdentList(vector<Token> tokens, int& num, TreeNode* parent, vector<string>& list)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].type == "ID")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        string identifier = tokens[num].lexeme;
        newparent->children.push_back(newNode);
        num++;

        correct = conIdentList(tokens, num, newparent, list);

        if (!correct)
        {
            cout << "Syntax Error in IdentList" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        list.insert(list.begin(), identifier);

        return true;
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool ElsePart(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == "Wagarna")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        correct = Stmt(tokens, num, newparent);
        if (correct)
        {
            return true;
        }
        else
        {
            cout << "Syntax Error in Wagarna Statement" << endl;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            num = old_num;
            return false;
        }
    }
    else
    {
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        num = old_num;
        return true;
    }
}

pair<bool, ExprAttrs> Mag_(vector<Token> tokens, int& num, TreeNode* parent, const string& expr) 
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;
    attrs.place = "";

    if (num < tokens.size() && tokens[num].lexeme == "+")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> termResult = Term(tokens, num, newparent);
        if (!termResult.first) 
        {
            cout << "Error in Term after + operator" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        string temp = newTemp();
        attrs.code = termResult.second.code;
        attrs.code += temp + " := " + expr + " + " + termResult.second.place + "\n";
        attrs.place = temp;

        pair<bool, ExprAttrs> more_mags = Mag_(tokens, num, newparent, attrs.place);
        if (!more_mags.first) 
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code += more_mags.second.code;
        attrs.place = more_mags.second.place.empty() ? attrs.place : more_mags.second.place;

        return make_pair(true, attrs);
    }
    else if (num < tokens.size() && tokens[num].lexeme == "-")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> termResult = Term(tokens, num, newparent);
        if (!termResult.first) 
        {
            cout << "Error in Term after - operator" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        string temp = newTemp();
        attrs.code = termResult.second.code;
        attrs.code += temp + " := " + expr + " - " + termResult.second.place + "\n";
        attrs.place = temp;

        pair<bool, ExprAttrs> more_mags = Mag_(tokens, num, newparent, attrs.place);
        if (!more_mags.first) 
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code += more_mags.second.code;
        attrs.place = more_mags.second.place.empty() ? attrs.place : more_mags.second.place;

        return make_pair(true, attrs);
    }
    else
    {
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        return make_pair(true, attrs);
    }
}

pair<bool, ExprAttrs> Mag(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;

    pair<bool, ExprAttrs> termResult = Term(tokens, num, newparent);
    if (!termResult.first) 
    {
        cout << "Error in Term within Mag" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }

    attrs.code = termResult.second.code;
    attrs.place = termResult.second.place;

    pair<bool, ExprAttrs> mag_ops = Mag_(tokens, num, newparent, attrs.place);
    if (!mag_ops.first) 
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }

    attrs.code += mag_ops.second.code;
    attrs.place = mag_ops.second.place.empty() ? attrs.place : mag_ops.second.place;

    return make_pair(true, attrs);
}

pair<bool, ExprAttrs> Rvalue_(vector<Token> tokens, int& num, TreeNode* parent, const string& expr)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;
    attrs.place = "";

    if (num < tokens.size() && (tokens[num].lexeme == "==" || tokens[num].lexeme == "<" || tokens[num].lexeme == ">" || tokens[num].lexeme == "<=" || tokens[num].lexeme == ">=" ||
        tokens[num].lexeme == "!=" || tokens[num].lexeme == "<>"))
    {
        string op = tokens[num].lexeme;
        TreeNode* newNode = new TreeNode(op);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> magResult = Mag(tokens, num, newparent);
        if (!magResult.first) 
        {
            cout << "Error in Mag after comparison operator" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        string temp = newTemp();
        attrs.code = magResult.second.code;
        attrs.code += temp + " := " + expr + " " + op + " " + magResult.second.place + "\n";
        attrs.place = temp;

        pair<bool, ExprAttrs> more_rvalues = Rvalue_(tokens, num, newparent, attrs.place);
        if (!more_rvalues.first) 
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code += more_rvalues.second.code;
        attrs.place = more_rvalues.second.place.empty() ? attrs.place : more_rvalues.second.place;

        return make_pair(true, attrs);
    }
    else
    {
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        return make_pair(true, attrs);
    }
}

pair<bool, ExprAttrs> Expr(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;
    attrs.isLvalue = false;

    if (num < tokens.size() && tokens[num].type == "ID") {
        string id = tokens[num].lexeme;
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        if (num < tokens.size() && tokens[num].lexeme == ":=")
        {
            attrs.isLvalue = true;
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;

            pair<bool, ExprAttrs> rhsResult = Expr(tokens, num, newparent);
            if (!rhsResult.first)
            {
                cout << "Error in right-hand side expression of assignment" << endl;
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return make_pair(false, attrs);
            }

            attrs.code = rhsResult.second.code;
            attrs.code += id + " := " + rhsResult.second.place + "\n";
            attrs.place = id;

            emit(attrs.code);

            return make_pair(true, attrs);
        }
        else
        {
            pair<bool, ExprAttrs> exprResult = Expr_(tokens, num, newparent, id);
            if (exprResult.first)
            {
                return exprResult;
            }
            else
            {
                cout << "Error in Expression" << endl;
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return make_pair(false, attrs);
            }
        }
    }
    else if (num < tokens.size() && tokens[num].lexeme == "(")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> subexprResult = Expr(tokens, num, newparent);
        if (!subexprResult.first) {
            cout << "Error in subexpression within parentheses" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code = subexprResult.second.code;
        attrs.place = subexprResult.second.place;

        if (num < tokens.size() && tokens[num].lexeme == ")")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;

            pair<bool, ExprAttrs> term_ops = Term_(tokens, num, newparent, attrs.place);
            if (!term_ops.first)
            {
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return make_pair(false, attrs);
            }
            attrs.code += term_ops.second.code;
            attrs.place = term_ops.second.place.empty() ? attrs.place : term_ops.second.place;

            pair<bool, ExprAttrs> mag_ops = Mag_(tokens, num, newparent, attrs.place);
            if (!mag_ops.first)
            {
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return make_pair(false, attrs);
            }
            attrs.code += mag_ops.second.code;
            attrs.place = mag_ops.second.place.empty() ? attrs.place : mag_ops.second.place;

            pair<bool, ExprAttrs> rval_ops = Rvalue_(tokens, num, newparent, attrs.place);

            if (!rval_ops.first)
            {
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return make_pair(false, attrs);
            }
            attrs.code += rval_ops.second.code;
            attrs.place = rval_ops.second.place.empty() ? attrs.place : rval_ops.second.place;

            return make_pair(true, attrs);
        }
        else
        {
            cout << "Missing )" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
    }
    else if (num < tokens.size() && tokens[num].type == "NUMBER")
    {
        string numVal = tokens[num].lexeme;
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        string place = numVal;

        pair<bool, ExprAttrs> term_ops = Term_(tokens, num, newparent, place);

        if (!term_ops.first)
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code = term_ops.second.code;
        attrs.place = term_ops.second.place.empty() ? place : term_ops.second.place;

        pair<bool, ExprAttrs> mag_ops = Mag_(tokens, num, newparent, attrs.place);

        if (!mag_ops.first)
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code += mag_ops.second.code;
        attrs.place = mag_ops.second.place.empty() ? attrs.place : mag_ops.second.place;

        pair<bool, ExprAttrs> rval_ops = Rvalue_(tokens, num, newparent, attrs.place);

        if (!rval_ops.first)
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code += rval_ops.second.code;
        attrs.place = rval_ops.second.place.empty() ? attrs.place : rval_ops.second.place;

        return make_pair(true, attrs);
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }
}

pair<bool, ExprAttrs> Expr_(vector<Token> tokens, int& num, TreeNode* parent, const string& id) 
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;

    if (num < tokens.size() && tokens[num].lexeme == ":=")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> rhs = Expr(tokens, num, newparent);
        if (!rhs.first)
        {
            cout << "Error in Expression" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code = rhs.second.code;
        attrs.code += id + " := " + rhs.second.place + "\n";
        attrs.place = id;

        emit(attrs.code);

        return make_pair(true, attrs);
    }
    else
    {
        attrs.place = id;

        pair<bool, ExprAttrs> term_ops = Term_(tokens, num, newparent, id);
        if (!term_ops.first)
        {
            cout << "Error in Term_" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code = term_ops.second.code;
        attrs.place = term_ops.second.place.empty() ? id : term_ops.second.place;

        pair<bool, ExprAttrs> mag_ops = Mag_(tokens, num, newparent, attrs.place);
        if (!mag_ops.first)
        {
            cout << "Error in Mag_" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code += mag_ops.second.code;
        attrs.place = mag_ops.second.place.empty() ? attrs.place : mag_ops.second.place;

        pair<bool, ExprAttrs> rval_ops = Rvalue_(tokens, num, newparent, attrs.place);
        if (!rval_ops.first)
        {
            cout << "Error in Rvalue_" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        attrs.code += rval_ops.second.code;
        attrs.place = rval_ops.second.place.empty() ? attrs.place : rval_ops.second.place;

        return make_pair(true, attrs);
    }

    num = old_num;
    parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
    return make_pair(false, attrs);
}

pair<bool, ExprAttrs> Factor(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;

    if (num < tokens.size() && tokens[num].lexeme == "(") {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> subexprResult = Expr(tokens, num, newparent);
        if (!subexprResult.first) 
        {
            cout << "Error in subexpression within Factor" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code = subexprResult.second.code;
        attrs.place = subexprResult.second.place;

        if (num < tokens.size() && tokens[num].lexeme == ")")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
            return make_pair(true, attrs);
        }
        else
        {
            cout << "Missing )" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
    }
    else if (num < tokens.size() && (tokens[num].type == "ID" || tokens[num].type == "NUMBER"))
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        attrs.place = tokens[num].lexeme;
        num++;
        return make_pair(true, attrs);
    }
    else
    {
        cout << "Syntax Error in Factor Variable" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }
}

pair<bool, ExprAttrs> Term_(vector<Token> tokens, int& num, TreeNode* parent, const string& expr) {
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;
    attrs.place = "";

    if (num < tokens.size() && tokens[num].lexeme == "*")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> factorResult = Factor(tokens, num, newparent);
        if (!factorResult.first) 
        {
            cout << "Error in Factor after * operator" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        string temp = newTemp();
        attrs.code = factorResult.second.code;
        attrs.code += temp + " := " + expr + " * " + factorResult.second.place + "\n";
        attrs.place = temp;

        pair<bool, ExprAttrs> more_terms = Term_(tokens, num, newparent, attrs.place);
        if (!more_terms.first) 
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code += more_terms.second.code;
        attrs.place = more_terms.second.place.empty() ? attrs.place : more_terms.second.place;

        return make_pair(true, attrs);
    }
    else if (num < tokens.size() && tokens[num].lexeme == "/")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        pair<bool, ExprAttrs> factorResult = Factor(tokens, num, newparent);
        if (!factorResult.first) 
        {
            cout << "Error in Factor after / operator" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        string temp = newTemp();
        attrs.code = factorResult.second.code;
        attrs.code += temp + " := " + expr + " / " + factorResult.second.place + "\n";
        attrs.place = temp;

        pair<bool, ExprAttrs> more_terms = Term_(tokens, num, newparent, attrs.place);
        if (!more_terms.first) 
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }

        attrs.code += more_terms.second.code;
        attrs.place = more_terms.second.place.empty() ? attrs.place : more_terms.second.place;

        return make_pair(true, attrs);
    }
    else
    {
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        return make_pair(true, attrs);
    }
}

pair<bool, ExprAttrs> Term(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;

    pair<bool, ExprAttrs> factorResult = Factor(tokens, num, newparent);
    if (!factorResult.first) 
    {
        cout << "Error in Factor within Term" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }

    attrs.code = factorResult.second.code;
    attrs.place = factorResult.second.place;

    pair<bool, ExprAttrs> term_ops = Term_(tokens, num, newparent, attrs.place);
    if (!term_ops.first) 
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return make_pair(false, attrs);
    }

    attrs.code += term_ops.second.code;
    attrs.place = term_ops.second.place.empty() ? attrs.place : term_ops.second.place;

    return make_pair(true, attrs);
}

pair<bool, ExprAttrs> OptExpr(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    ExprAttrs attrs;

    if (num < tokens.size() && (tokens[num].type == "ID" || tokens[num].type == "NUMBER" || tokens[num].lexeme == "(")) 
    {
        pair<bool, ExprAttrs> exprResult = Expr(tokens, num, parent);
        if (!exprResult.first) 
        {
            cout << "Error in optional expression" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return make_pair(false, attrs);
        }
        return exprResult;
    }
    else
    {
        num = old_num;
        TreeNode* newNode = new TreeNode("NULL");
        newparent->children.push_back(newNode);
        return make_pair(true, attrs);
    }
}

bool ForStmt(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == "for")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        if (num < tokens.size() && tokens[num].lexeme == "(")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing (" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        pair<bool, ExprAttrs> initResult = Expr(tokens, num, newparent);
        if (!initResult.first)
        {
            cout << "Error in initialization expression of for loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        if (num < tokens.size() && tokens[num].lexeme == "::")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing ::" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        string startLabel = newLabel();
        string endLabel = newLabel();

        emit(startLabel + ":");

        pair<bool, ExprAttrs> conditionResult = OptExpr(tokens, num, newparent);
        if (!conditionResult.first)
        {
            cout << "Error in condition expression of for loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        emit(conditionResult.second.code);

        if (!conditionResult.second.place.empty()) 
        {
            emit("if " + conditionResult.second.place + " == 0 goto " + endLabel);
        }

        if (num < tokens.size() && tokens[num].lexeme == "::")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing ::" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        pair<bool, ExprAttrs> updateResult = OptExpr(tokens, num, newparent);
        if (!updateResult.first)
        {
            cout << "Error in update expression of for loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
        ExprAttrs update = updateResult.second;

        if (num < tokens.size() && tokens[num].lexeme == ")")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing )" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        if (Stmt(tokens, num, newparent))
        {
            emit(update.code);

            emit("goto " + startLabel);

            emit(endLabel + ":");
            return true;
        }
        else
        {
            cout << "Syntax Error in For Loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool WhileStmt(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == "while")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        if (num < tokens.size() && tokens[num].lexeme == "(")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing (" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        string startLabel = newLabel();
        string endLabel = newLabel();

        emit(startLabel + ":");

        pair<bool, ExprAttrs> conditionResult = Expr(tokens, num, newparent);
        if (!conditionResult.first)
        {
            cout << "Error in condition expression of while loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        emit(conditionResult.second.code);

        emit("if " + conditionResult.second.place + " == 0 goto " + endLabel);

        if (num < tokens.size() && tokens[num].lexeme == ")")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing )" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        if (Stmt(tokens, num, newparent))
        {
            emit("goto " + startLabel);

            emit(endLabel + ":");
            return true;
        }
        else
        {
            cout << "Syntax Error in While Loop" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool IfStmt(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == "Agar")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        if (num < tokens.size() && tokens[num].lexeme == "(")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing (" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        string elseLabel = newLabel();
        string endLabel = newLabel();

        pair<bool, ExprAttrs> conditional_results = Expr(tokens, num, newparent);
        if (!conditional_results.first)
        {
            cout << "Error in Conditional Expression in If Condition" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        emit(conditional_results.second.code);

        emit("if " + conditional_results.second.place + " == 0 goto " + elseLabel);

        if (num < tokens.size() && tokens[num].lexeme == ")")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
        }
        else
        {
            cout << "Missing )" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        if (!Stmt(tokens, num, newparent))
        {
            cout << "Syntax Error in Agar Statement" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        emit("goto " + endLabel);
        emit(elseLabel + ":");

        bool hasElse = false;
        int elsePt = num;

        if (num < tokens.size() && tokens[num].lexeme == "Wagarna")
        {
            hasElse = true;

            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;

            if (!Stmt(tokens, num, newparent))
            {
                cout << "Syntax Error in Wagarna Statement" << endl;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                num = old_num;
                return false;
            }
        }

        if (!hasElse)
        {
            num = elsePt;
            bool correct = ElsePart(tokens, num, newparent);
            if (!correct)
            {
                cout << "Syntax Error in Agar Statement" << endl;
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return false;
            }
        }

        emit(endLabel + ":");
        return true;
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool StmtList_(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;

    if (Stmt(tokens, num, newparent))
    {
        bool correct = StmtList_(tokens, num, newparent);
        if (!correct)
        {
            num = old_num;
            cout << "Error in StmtList_" << endl;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
        return true;
    }

    num = old_num;
    TreeNode* newNode = new TreeNode("NULL");
    newparent->children.push_back(newNode);
    return true;
}

bool StmtList(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);

    if (StmtList_(tokens, num, newparent))
    {
        return true;
    }
    else
    {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool CompoundStmt(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;

    if (num < tokens.size() && tokens[num].lexeme == "{")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;
    }
    else
    {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }

    bool correct = StmtList(tokens, num, newparent);

    if (!correct)
    {
        cout << "Error in CompoundStmt" << endl;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }

    if (num < tokens.size() && tokens[num].lexeme == "}")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;
        return true;
    }
    else
    {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        cout << "Missing }" << endl;
        return false;
    }

}

bool Type(vector<Token> tokens, int& num, TreeNode* parent, string& data_type)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);

    if (num < tokens.size() && (tokens[num].lexeme == "Adadi" || tokens[num].lexeme == "Ashriya" || tokens[num].lexeme == "Harf" || tokens[num].lexeme == "Matn" || tokens[num].lexeme == "Mantiqi"))
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        data_type = tokens[num].lexeme;
        num++;
        return true;
    }
    else
    {
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool Declaration(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;
    string data_type = "";
    vector<string> identifier_list = {};

    if (Type(tokens, num, newparent, data_type))
    {
        if (IdentList(tokens, num, newparent, identifier_list))
        {
            if (num < tokens.size() && tokens[num].lexeme == "::")
            {
                TreeNode* newNode = new TreeNode(tokens[num].lexeme);
                newparent->children.push_back(newNode);
                num++;

                ofstream writeFile;
                writeFile.open("TAC.txt", ios::app);

                for (int i = 0; i < identifier_list.size(); i++)
                {
                    writeFile << identifier_list[i] << " : " << data_type << endl;
                }

                writeFile.close();

                return true;
            }
            else
            {
                cout << "Missing :: in Declaration" << endl;
                num = old_num;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return false;
            }
        }
        else
        {
            cout << "Syntax Error in Declaration" << endl;
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool Stmt(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;

    if (ForStmt(tokens, num, newparent))
    {
        return true;
    }
    else if (WhileStmt(tokens, num, newparent))
    {
        return true;
    }
    else if (IfStmt(tokens, num, newparent))
    {
        return true;
    }
    else if (CompoundStmt(tokens, num, newparent))
    {
        return true;
    }
    else if (Declaration(tokens, num, newparent))
    {
        return true;
    }
    else if (num < tokens.size() && tokens[num].lexeme == "::")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;
        return true;
    }
    else if (num < tokens.size() && (tokens[num].type == "ID" || tokens[num].type == "NUMBER" || tokens[num].lexeme == "("))
    {
        pair<bool, ExprAttrs> exprResult = Expr(tokens, num, parent);
        if (!exprResult.first)
        {
            num = old_num;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }

        if (num < tokens.size() && tokens[num].lexeme == "::")
        {
            TreeNode* newNode = new TreeNode(tokens[num].lexeme);
            newparent->children.push_back(newNode);
            num++;
            return true;
        }
        else
        {
            num = old_num;
            cout << "Missing :: after Expression" << endl;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return false;
        }
    }
    else
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
}

bool Arg(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    string data_type = "";
    bool correct = Type(tokens, num, newparent, data_type);

    if (!correct)
    {
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }

    if (num < tokens.size() && tokens[num].type == "ID")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;
        return true;
    }
    else
    {
        cout << "Missing Identifier in Argument" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }

}

bool Arglist_(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    if (num < tokens.size() && tokens[num].lexeme == ",")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        newparent->children.push_back(newNode);
        num++;

        if (Arg(tokens, num, newparent))
        {
            correct = Arglist_(tokens, num, newparent);
            if (!correct)
            {
                num = old_num;
                cout << "Error in ArgList_" << endl;
                parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
                return false;
            }
            return true;
        }
        else
        {
            num = old_num;
            cout << "Error in ArgList_" << endl;
            parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
            return true;
        }
    }

    TreeNode* newNode = new TreeNode("NULL");
    newparent->children.push_back(newNode);
    num = old_num;
    return true;
}

bool Arglist(vector<Token> tokens, int& num, TreeNode* parent)
{
    TreeNode* newparent = new TreeNode(__func__);
    parent->children.push_back(newparent);
    int old_num = num;
    bool correct = false;

    correct = Arg(tokens, num, newparent);

    if (!correct)
    {
        cout << "Error in ArgList" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }

    correct = Arglist_(tokens, num, newparent);
    if (!correct)
    {
        cout << "Error in ArgList" << endl;
        num = old_num;
        parent->children.erase(remove(parent->children.begin(), parent->children.end(), newparent), parent->children.end());
        return false;
    }
    return true;
}

bool Function(vector<Token> tokens, int num, TreeNode*& parent)
{
    temp_counter = 0;
    label_counter = 0;
    string data_type = "";
    bool correct = Type(tokens, num, parent, data_type);
    
    if (!correct)
    {
        parent = nullptr;
        return false;
    }

    if (num < tokens.size() && tokens[num].type == "ID")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        parent->children.push_back(newNode);
        string functionName = tokens[num].lexeme;
        num++;
        emit("Function Begin: " + functionName);
    }
    else
    {
        cout << "Syntax Rule Not Followed for Function Declaration\nIdentifier Name is Missing" << endl;
        parent = nullptr;
        return false;
    }

    if (num < tokens.size() && tokens[num].lexeme == "(")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        parent->children.push_back(newNode);
        num++;
    }
    else
    {
        cout << "Syntax Rule Not Followed for Function Declaration\n( is Missing" << endl;
        parent = nullptr;
        return false;
    }

    correct = Arglist(tokens, num, parent);

    if (!correct)
    {
        cout << "Syntax Rule Not Followed for Function Declaration\nArguments are Missing" << endl;
        parent = nullptr;
        return false;
    }

    if (num < tokens.size() && tokens[num].lexeme == ")")
    {
        TreeNode* newNode = new TreeNode(tokens[num].lexeme);
        parent->children.push_back(newNode);
        num++;
    }
    else
    {
        cout << "Syntax Rule Not Followed for Function Declaration\n) is Missing" << endl;
        parent = nullptr;
        return false;
    }

    correct = CompoundStmt(tokens, num, parent);

    if (!correct)
    {
        cout << "Syntax Rule Not Followed for Function Body" << endl;
        parent = nullptr;
        return false;
    }

    return num >= tokens.size();
}

TreeNode* recursive_Descent_Parser(vector<Token> tokens)
{
    Tree descent_tree;
    descent_tree.root = new TreeNode("Function");

    if (tokens.empty())
    {
        return nullptr;
    }

    if (Function(tokens, 0, descent_tree.root))
    {
        return descent_tree.root;
    }
    else
    {
        return nullptr;
    }
}