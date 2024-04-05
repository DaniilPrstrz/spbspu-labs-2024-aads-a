#include "inputToPostfix.hpp"

#include <string>
#include <sstream>

void rebdev::makePostfixQueue(queue< node > & postfixQueue, std::istream & in)
{
  Stack< node > operationStack;

  while (!in.eof())
  {
    std::string inputStr;
    in >> inputStr;

    std::istringstream iss(inputStr);
    long long num = 0;
    iss >> num;

    if (!iss)
    {
      pushOperation(operationStack, postfixQueue, inputStr[0]);
    }
    else
    {
      postfixQueue.push(num);
    }
  }

  while (operationStack.size() > 0)
  {
    node topNode = operationStack.drop();

    if (topNode.getPriority() == 3)
    {
      throw std::logic_error("Error with ( or )");
    }

    postfixQueue.push(topNode);
  }
}

void rebdev::pushOperation(Stack< node > & operationStack, queue< node > & postfixQueue, char operation)
{
  if  (operation == ')')
  {
    node topNode = operationStack.drop();
    while (topNode != '(')
    {
      postfixQueue.push(topNode);
      topNode = operationStack.drop();
    }
  }
  else if (operation == '(')
  {
    operationStack.push(node(operation));
  }
  else
  {
    node newNode(operation);
    if (operationStack.size() > 0)
    {
      node topNode = operationStack.drop();
      while (newNode >= topNode)
      {
        postfixQueue.push(topNode);
        topNode = operationStack.drop();
        if (operationStack.size() == 0) break;
      }
    }
    operationStack.push(newNode);
  }
}
