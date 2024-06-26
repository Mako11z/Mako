/* Each node is a :
    - BinaryExpression
    - IfStatements
    - While loop
    - Print()
    - VariableDef
    - Variable
    - Literal
    - Assignment

    Example:
    if (a < b) {int i = 0;}
    - We see a if statemnet so we create a Ifstatement ASTNode
        - We expect a '(' after the if statement and a ')' at the end
    - We consume the 'a'
        - Check if 'a' is defined
    - The we see the '<' so we create a BinaryExpression ASTNode with '<' as the operator
    - Then we see the 'b' and we must also check if it is defined as well
    - A ')' is encountered so we expect a open and clsoing '{}'
    - We then see int so we create a VariableDef ASTNode
    - 'i' is encountered and we update the VariableDef ASTNode to include the variable name 'i'
    - '=' is encountered which indicates 'i' is being initilized and we parse the next value
    - 0 is what we consume next and it is a literal so we create a Literal ASTNode for this value
    - We attach the Literal ASTNode as the child to the VariableDef ASTNode
        - Indicates 'i' = 0
    - Consume the ';' confirming the end of the variable declariation

    Example:
    y = 5 + 10 * 2
    - Recognize y as a variable name
    - We create a VariableNode to rrepresent 'y'
    - Encounter '=' so we create a Assignment AASTNode
    - The Variable node will be it's left child
    - Then we see 5 as a literal and create a literal node
    - Encounter '+' so we create a BinaryExpression node with '+' as the operator
    - 5 will be it's left child
    - Encounter 10 and create a Literal node
    - '*' multiplcation is encountered and we create another BinaryExpression node
    - The 10 will be it's left child
    - 2 is encountered and we create a Literal node
    - This will be the right child
    - Attach the BinaryExpression as the right child of Assignmemt node
*/
#include <iostream>
#include <string>

enum class ASTNodeType
{
    BinaryExpression,
    VariableDef,
    Literal,
    IfStatement,
    WhileLoop,
    Assignment,
    Print
};

struct ASTNode
{
    ASTNodeType type;
    ASTNode *left;
    ASTNode *right;
    ASTNode(ASTNodeType t) : type(t), left(nullptr), right(nullptr) {}
    virtual ~ASTNode() = default;
};

struct VariableDef : ASTNode
{
    std::string data_type;
    std::string var_name;
    VariableDef(std::string &type, std::string &name) : ASTNode(ASTNodeType::VariableDef), data_type(type), var_name(name) {}
};

struct BinaryExpressionNode : ASTNode
{
    std::string operation;
    BinaryExpressionNode(std::string &op, ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::BinaryExpression), operation(op)
    {
        left = l;
        right = r;
    }
};

struct LiteralNode : ASTNode
{
    std::string value;
    LiteralNode(std::string &val) : ASTNode(ASTNodeType::Literal), value(val) {}
};

struct AssignmemtNode : ASTNode
{
    AssignmemtNode(ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::Assignment)
    {
        left = l;
        right = r;
    }
};

struct IfStatementNode : ASTNode
{
    IfStatementNode(ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::IfStatement)
    {
        left = l;
        right = r;
    }
};

struct WhileLoopNode : ASTNode
{
    WhileLoopNode(ASTNode *l, ASTNode *r) : ASTNode(ASTNodeType::WhileLoop)
    {
        left = l;
        right = r;
    }
};

struct PrintNode : ASTNode
{
    std::string value_to_print;
    PrintNode(std::string &val) : ASTNode(ASTNodeType::Print), value_to_print(val) {}
};
