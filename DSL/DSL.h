#ifndef _HDSL
#define _HDSL

#define OPERATION_VALUE(op)   ((NodeValue){.oper = (op)})
#define OPERATOR_VALUE(op)    ((NodeValue){.optr = (op)})
#define NUM_VALUE(n)          ((NodeValue){.num  = (n)})
#define VAR_VALUE(index)      ((NodeValue){.var  = (index)})


#define _NUM(n)           NewNode(NUM,       NUM_VALUE(n),     nullptr, nullptr)
#define _VAR(x)           NewNode(VAR,       VAR_VALUE(x),     nullptr, nullptr)

#define _ADD(a, b)        NewNode(OPERATION, OPERATION_VALUE(ADD),  a, b)
#define _SUB(a, b)        NewNode(OPERATION, OPERATION_VALUE(SUB),  a, b)
#define _MUL(a, b)        NewNode(OPERATION, OPERATION_VALUE(MUL),  a, b)
#define _DIV(a, b)        NewNode(OPERATION, OPERATION_VALUE(DIV),  a, b)
#define _POW(a, b)        NewNode(OPERATION, OPERATION_VALUE(POW),  a, b)

#define _EQ (a, b)         NewNode(OPERATION, OPERATION_VALUE(EQ),   a, b)
#define _NEQ(a, b)         NewNode(OPERATION, OPERATION_VALUE(NEQ),  a, b)
#define _LT (a, b)         NewNode(OPERATION, OPERATION_VALUE(LT),   a, b)
#define _LE (a, b)         NewNode(OPERATION, OPERATION_VALUE(LE),   a, b)
#define _GT (a, b)         NewNode(OPERATION, OPERATION_VALUE(GT),   a, b)
#define _GE (a, b)         NewNode(OPERATION, OPERATION_VALUE(GE),   a, b)

#define _ASSIGN(a, b)      NewNode(OPERATOR,  OPERATOR_VALUE(OP_ASSIGN), a, b)
#define _IF    (a, b)      NewNode(OPERATOR,  OPERATOR_VALUE(OP_IF)    , a, b)
#define _WHILE (a, b)      NewNode(OPERATOR,  OPERATOR_VALUE(OP_WHILE),  a, b)



#endif //_HDSL
