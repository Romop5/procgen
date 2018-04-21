#ifndef _STATEMENT_H
#define _STATEMENT_H
#include <iostream>
#include <map>
#include <memory>
#include <procgen/interpret/resource.h>
#include <vector>

namespace ProcGen {
/**
* @class RunStatus
* @brief Helper class for execution flow controlling
*
* This class is responsible for alterning execution flow in both
* special and usual cases.
* The instance of classes is passed down through abstract tree
* and it carries current execution state.
* In case of run-time error or early function exit, the state
* is alterned and leads to premature end of execution of statements.
*/
class RunStatus {
public:
    RunStatus() { _status = OK; }
    enum status { OK,
        RETURN_REACHED,
        RUNTIME_ERROR } _status;
    void setStatus(status _status) { this->_status = _status; }
    status getStatus() { return this->_status; }
};

class Function;
/**
* @class Statement
* @brief The inteface for execution blocks
*
* In this interpret, every statement and function is a statement itself
* and statements together make up a tree according to composite
* pattern. 
*
* Basically, there are two types of statements: functions and non-functions.
* The only significant difference is the fact that functions have some return
* value whereas non-functions executes without feedback.
*/
class Statement {
public:
    /// Execute block functionality
    virtual bool operator()(RunStatus&) = 0;
    virtual const std::string getBoxName() const { return "Statement"; }
    virtual json to_json() const { return json(getBoxName()); }
};

/**
* @class If
* @brief If-else block
*
* This is a logic block which executes either of its branch depending on the
* result of expression function.
*
* Pseudocode:
*   if ( expression ) 
*       execute ( paths [ 0 ] ) 
*   else
*       execute ( paths [ 1 ] ) 
*/
class If : public Statement {
private:
    // eval expr to choose path
    std::shared_ptr<Function> expr;
    std::map<size_t, std::shared_ptr<Statement>> paths;

public:
    virtual bool operator()(RunStatus&);
    virtual json to_json() const override;
    /**
* @brief Set predicate expression tree
* @param exp
*/
    void setExpression(std::shared_ptr<Function> exp);
    /**
* @brief Set specified path (branch)
*
* @param id
* @param path
*/
    void setPath(size_t id, std::shared_ptr<Statement> path);
};

/**
* @class While
* @brief While-loop block executing statament
*
* This block executes given statement while predicate implies TRUE
*/
class While : public Statement {
private:
    std::shared_ptr<Function> _expression;
    std::shared_ptr<Statement> _statement;

public:
    /**
* @brief Set loop condition 
* @param expression
*/
    void bindCondition(std::shared_ptr<Function> expression) { this->_expression = expression; }
    /**
* @brief Set loop statement 
*
* @param statement
*/
    void bindStatement(std::shared_ptr<Statement> statement) { this->_statement = statement; }

    virtual bool operator()(RunStatus&);
};

/**
* @class Body
* @brief Sequence of statements
*
* This block executes statements back to back.
* Pseudocode:
*   {
*       statement0;
*       statement1;
*       statement2;
*       statementN;
*   }
*/
class Body : public Statement {
private:
    std::vector<std::shared_ptr<Statement>> statements;

public:
    void appendStatement(std::shared_ptr<Statement> stat) { this->statements.push_back(stat); }
    virtual bool operator()(RunStatus&);
};

/**
* @class Return
* @brief Special return statement which alterns control flow
*
* This statement is used in composite functions when early exit of function
* is desired.
* Apart from changing RunStatus, it also executs input statement which can
* for example, set return value
*/
class Return : public Statement {
private:
    std::shared_ptr<Statement> input;

public:
    void bindInput(std::shared_ptr<Function> function) { input = std::dynamic_pointer_cast<Statement>(function); }
    virtual bool operator()(RunStatus&);
};
}
#endif
