#include "expressionObject.hpp"
#include "transformToPostfix.hpp"

#include <stdexcept>
#include <sys/resource.h>
#include "stack.hpp"


bool getpriority(const arakelyan::ExpressionObj &val, const arakelyan::ExpressionObj &otherV)
{
  return val.getPriority() <= otherV.getPriority();
}

arakelyan::Queue< arakelyan::ExpressionObj > arakelyan::transformInfixToPostfix(Queue< ExpressionObj > &infixQueue)
{
  Queue< ExpressionObj > postfixQ;
  Stack< ExpressionObj > operS;

  while (!infixQueue.empty())
  {
    auto curObj = infixQueue.front();
    infixQueue.pop();

    if (curObj.getType() == token_t::operand)
    {
      postfixQ.push(curObj);
    }
    else if (curObj.getType() == token_t::bracket)
    {
      if (curObj.getVal().oper_ == '(')
      {
        operS.push(curObj);
      }
      else if (curObj.getVal().oper_ == ')')
      {
        while ((!operS.empty()) && (operS.top().getVal().oper_ != '('))
        {
          postfixQ.push(operS.top());
          operS.pop();
        }
        if (operS.empty())
        {
          throw std::logic_error("brackets fault");
        }
        operS.pop();
      }
    }
    else if (curObj.getType() == token_t::operation)
    {
      while (!operS.empty() && (operS.top().getVal().oper_ != '(') && (getpriority(operS.top(), curObj)))
      {
        postfixQ.push(operS.top());
        operS.pop();
      }
      operS.push(curObj);
    }
  }

  while (!operS.empty())
  {
    if (operS.top().getType() == token_t::bracket)
    {
      throw std::logic_error("bracket error!");
    }
    postfixQ.push(operS.top());
    operS.pop();
  }

  return postfixQ;
}
