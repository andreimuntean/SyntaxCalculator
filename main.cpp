/*
Version 0.5 - Alpha - Not at all polished or optimized
NOTE: You have to compile this in C++11.
COPYRIGHT ANDREI MUNTEAN 2013
*/

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>
#include <vector>

using namespace std;

struct task { string syntax; };

vector<string> constants = { "pi", "e", "gr" };
vector<string> constantsValues = { "3.1415926535897932",
                                   "2.7182818284590452",
                                   "1.6180339887498948" };

char operators[4] = { '*', '/', '+', '+' };

vector<string> functions = { "power", "sqrt" };
vector<task> taskList;


void initialize(string &syntax)
{
    fstream f;
    f.open("computation.txt");
    getline(f, syntax);
    f.close();
}

void displayTasks()
{
    cout << "\n\n";
    for (short i = 0; i < taskList.size(); ++i)
    {
        cout << i << ": " << taskList[i].syntax << "\n";
    }
}

void addTask(string syntax)
{
    task t;
    t.syntax = syntax;
    taskList.push_back(t);
}

void divideIntoTasks(string &syntax)
{
    short i, j;

    for (i = syntax.size() - 2; i >= 0; --i)
    {
        // Searches for brackets.
        if (syntax[i] == '(')
        {
            bool seekingBracket = true;

            for (j = i; j < syntax.size() && seekingBracket; ++j)
            {
                if (syntax[j] == ')')
                {
                    // Divides them into tasks.
                    addTask(syntax.substr(i + 1, j - i - 1));
                    syntax.erase(i, j - i + 1);
                    stringstream ss;
                    ss << "{" << taskList.size() - 1 << "}";
                    syntax.insert(i, ss.str());
                    seekingBracket = false;
                }
            }

            if (seekingBracket) throw 0;

            // String size changed, must readjust.
            i = syntax.size() - 1;
        }
    }
    addTask(syntax);
    syntax = "";
}

bool isConstant(string syntax, size_t const_position, size_t const_length)
{
    locale loc;
    if (syntax.size() > const_position + const_length)
    {
        if (isalpha(syntax.at(const_position + const_length), loc))
            return false;
    }
    else if (const_position > 0)
    {
        if (isalpha(syntax.at(const_position - 1), loc))
            return false;
    }
    return true;
}

void assignConstants(task &t)
{
    size_t currentConstant = 0;

    while (true)
    {
        short position = t.syntax.find(constants[currentConstant], 0);
        while (position != -1)
        {
            if (isConstant(t.syntax, position, constants[currentConstant].size()))
            {
                t.syntax.replace(position, constants[currentConstant].size(), constantsValues[currentConstant]);
            }
            position = t.syntax.find(constants[currentConstant], position + 1);
        }
        if (++currentConstant >= constants.size())
            break;
    }
}

bool isLetter(char x)
{
    locale loc;
    return isalpha(x, loc);
}

vector<double> stringToArgs(string syntax)
{
    vector<double> args;

    double temp;
    short begin = 0;
    short end = syntax.find(',', 0);

    while (end != -1)
    {
        stringstream ss1;
        ss1 << syntax.substr(begin, end - begin);
        ss1 >> temp;
        args.push_back(temp);
        begin = end + 1;
        end = syntax.find(',', end + 1);
    }

    stringstream ss2;
    ss2 << syntax.substr(begin, syntax.size() - begin);
    ss2 >> temp;
    args.push_back(temp);

    return args;
}

string performFunction(string name, vector<double> args)
{
    double result;
    if (name == "power")
        result = pow(args.at(0), args.at(1));
    else if (name == "sqrt")
        result = sqrt(args.at(0));

    stringstream ss;
    ss << result;

    return ss.str();
}

void solveFunctions(task &t)
{
    short i;
    short begin = -1, end1 = -1, end2;
    string name, value;
    for (i = 0; i < t.syntax.size(); ++i)
    {
        if (isLetter(t.syntax.at(i)))
        {
            if (begin == -1) begin = i;
        }
        else
        {
            if (begin != -1)
            {
                if (end1 == -1)
                {
                    end1 = i;
                    name = t.syntax.substr(begin, end1 - begin);
                }

                if (i == t.syntax.size() - 1 || (!isdigit(t.syntax.at(i)) && t.syntax.at(i) != ',' && t.syntax.at(i) != '.' && t.syntax.at(i) != '-'))
                {
                    end2 = i;
                    if (i == t.syntax.size() - 1)
                        ++end2;
                    value = t.syntax.substr(end1, end2 - end1);
                    t.syntax.replace(begin, end2 - begin, performFunction(name, stringToArgs(value)));
                    i = 0;
                    begin = -1;
                    end1 = -1;
                }
            }
        }
    }
}

bool isNumber(char x)
{
    char numbers[13] = {'0', '1', '2', '3', '4', '5',
                        '6', '7', '8', '9', '.', '-' };
    for (char & i : numbers)
        if (x == i)
            return true;

    return false;
}

double performOperation(double x, double y, char _operator)
{
    if (_operator == '*') return x * y;
    else if (_operator == '/') return x / y;
    else if (_operator == '+') return x + y;
}

void solveOperations(task &t)
{
    short i, j;
    short operationLevel = 0;
    while (true)
    {
        for (i = 0; i < t.syntax.size(); ++i)
        {
            if (t.syntax.at(i) == operators[0 + 2 * operationLevel] || t.syntax.at(i) == operators[1 + 2 * operationLevel])
            {
                for (j = i - 1; j >= 0 && isNumber(t.syntax.at(j)); --j);

                short x_index = j + 1, x_length = i - j - 1;

                for (j = i + 1; j < t.syntax.size() && isNumber(t.syntax.at(j)); ++j);

                short y_index = i + 1, y_length = j - i - 1;
                stringstream result;
                result << performOperation(atof(t.syntax.substr(x_index, x_length).c_str()),
                                           atof(t.syntax.substr(y_index, y_length).c_str()),
                                           t.syntax.at(i));
                t.syntax.replace(x_index, x_length + y_length + 1, result.str());

                i = 0;
            }
        }

        ++operationLevel;

        if (operationLevel > 1)
        {
            break;
        }
    }
}

void assignTasks(task &t)
{
    short start = t.syntax.find('{', 0);
    short end = t.syntax.find('}', start + 1);

    while (start != -1 && end != -1)
    {
        size_t length = end - start + 1;

        size_t index;
        stringstream ss(t.syntax.substr(start + 1, length - 2));
        ss >> index;

        t.syntax.replace(start, length, taskList[index].syntax);

        start = t.syntax.find('{', end + 1);
        end = t.syntax.find('}', start + 1);
    }
}

void cleanTask(task &t)
{
    short position = t.syntax.find(" ", 0);
    while (position != -1)
    {
        t.syntax.erase(position, 1);
        position = t.syntax.find(" ", position + 1);
    }

    position = t.syntax.find("--", 0);
    while (position != -1)
    {
        t.syntax.replace(position, 2, "+");
        position = t.syntax.find("--", position + 1);
    }

    position = t.syntax.find("**", 0);
    while (position != -1)
    {
        t.syntax.replace(position, 2, "*");
        position = t.syntax.find("**", position);
    }

    position = t.syntax.find("//", 0);
    while (position != -1)
    {
        t.syntax.replace(position, 2, "/");
        position = t.syntax.find("//", position);
    }

    position = t.syntax.find("-", 0);
    while (position != -1)
    {
        if (position > 0 && isdigit(t.syntax.at(position - 1))) t.syntax.replace(position, 1, "+-");
        else --position;
        position = t.syntax.find("-", position + 2);
    }
}

void conquerTasks()
{
    for (task & t : taskList)
    {
        assignTasks(t);
        assignConstants(t);
        cleanTask(t);
        solveFunctions(t);
        solveOperations(t);
    }
}

void pause()
{
    cout << "\n\n";
    system("PAUSE");
    cout << "\n";
}

void displayResult()
{
    cout << endl << endl << " = " << taskList.at(taskList.size() - 1).syntax;
}

void divideAndConquer(string &syntax)
{
    try
    {
        divideIntoTasks(syntax);
        conquerTasks();
    }
    catch (int e)
    {
        cout << "Error";
    }
}

int main()
{
    string syntax;
    initialize(syntax);
    cout << "Syntax: " << syntax;
    divideAndConquer(syntax);
    displayResult();
    pause();

    return 0;
}
