/**
 *  @file main.cpp
 *  @brief The main implementation file for the postfix to infix
 *  algorithm assignment.
 *  @copyright (c) 2015 Robert MacGregor
 */

#include <cmath>            // pow

#include <string>           // string
#include <iostream>         // cout, cerr
#include <stdexcept>        // underflow_error, overflow_error, runtime_error
#include <unordered_set>    // unordered_set
#include <unordered_map>    // unordered_map

// Range tables used for isNumeric and isAlphabetical
#define UPPERCASE_LOWER 65
#define UPPERCASE_UPPER 90

#define LOWERCASE_LOWER 97
#define LOWERCASE_UPPER 122

#define NUMERIC_LOWER 48
#define NUMERIC_UPPER 57

using namespace std;

//! An eumeration representing our possible exit states.
enum RETURN_STATUS
{
    //! Nothing went wrong.
    RETURN_GOOD = 1,
    //! We memory leaked.
    RETURN_MEMLEAKED = 2,
    //! We had an underflow error.
    RETURN_UNDERFLOW = 4,
    //! We had an overflow error.
    RETURN_OVERFLOW = 8,
    //! We had some sort of runtime error.
    RETURN_RUNTIME = 16,
}; // End Enumeration RETURN_STATUS

/**
 *  @brief A basic stack implementation with FILO semantics.
 *  @param storedType The data type to store in our stack.
 */
template <typename storedType>
class Stack
{
    // Public Methods
    public:
        /**
         *  @brief Constructor accepting a maximum element count.
         *  @param maxElementCount The maximum number of elements to store in
         *  this new stack.
         */
        Stack(const size_t &maxElementCount = 256) : mElementCount(0), mMaxElementCount(maxElementCount)
        {
            if (!maxElementCount)
            {
                mMemory = NULL;
                return;
            }

            mMemory = new storedType[maxElementCount];
        }

        /**
         *  @brief Copy constructor accepting a reference to an existing Stack that stores
         *  the same data type as this Stack.
         *  @param stack A reference to an existing Stack that stores the same data type as this
         *  Stack.
         */
        Stack(const Stack<storedType> &stack) : mElementCount(stack.mElementCount), mMaxElementCount(stack.mMaxElementCount)
        {
            if (!stack.mMemory || stack.mMaxElementCount == 0)
                return;

            mMemory = new storedType[stack.mMaxElementCount];
            memcpy(mMemory, stack.mMemory, stack.mMaxElementCount);
        }

        //! Standard destructor.
        ~Stack(void)
        {
            if (mMemory)
                delete[] mMemory;

            mMemory = NULL;
        }

        /**
         *  @brief Reads an element of off the top of the Stack.
         *  @return A reference to the element currently residing at the top
         *  of the stack.
         *  @throw std::underflow_error Thrown when there is nothing left on the
         *  Stack to read.
         */
        storedType &top(void) const
        {
            if (mElementCount <= 0)
                throw underflow_error("Stack Underflow");

            return mMemory[mElementCount - 1];
        }

        /**
         *  @brief Pushes an element to the top of the Stack.
         *  @param input The value to push to the top of the Stack.
         *  @throw std::overflow_error Thrown when there is no room left
         *  on the Stack to push.
         */
        void push(const storedType &input)
        {
            if (mElementCount >= mMaxElementCount)
                throw overflow_error("Stack Overflow");

            mMemory[mElementCount++] = input;
        }

        /**
         *  @brief Pops an element off of the top of the Stack.
         *  @throw std::underflow_error Thrown where is nothing left
         *  to pop off of the Stack.
         */
        void pop(void)
        {
            if (mElementCount <= 0)
                throw underflow_error("Stack Underflow");

            mElementCount--;
        }

        /**
         *  @brief Returns whether or not this Stack is currently full.
         *  @return A boolean representing whether or not this Stack is
         *  currently full.
         */
        bool isFull(void) const
        {
            return mElementCount >= mMaxElementCount;
        }

        /**
         *  @brief Returns whether or not this Stack is currently empty.
         *  @return A boolean representing whether or not this Stack is
         *  currently empty.
         */
        bool isEmpty(void) const
        {
            return mElementCount <= 0;
        }

        /**
         *  @brief Returns the number of elements contained in this Stack.
         *  @return The number of elements currently contained in this Stack.
         */
        const size_t &getElementCount(void) { return mElementCount; }

    // Private Members
    private:
        //! The current number of elements in this Stack.
        size_t mElementCount;
        //! The maximum number of elements stored in this Stack.
        const size_t mMaxElementCount;
        //! A pointer to the memory block that this Stack will index.
        storedType *mMemory;
};

//! A forward declaration of our ExpressionComponent class.
class ExpressionComponent;

/**
 *  @brief An std::unordered_set containing all allocated ExpressionComponent instances.
 *  @note This is used to ensure that all memory in the program has been deallocated in
 *  error cases.
 */
static unordered_set<ExpressionComponent *> sExpressionComponentSet;

/**
 *  @brief The ExpressionComponent class is a representation of an individual
 *  piece of our input expression.
 *  @detail This can represent an operand, operator or an entire sub expression
 *  of the current expression. When the algorithm starts, one of these is created
 *  to represent the expression as a whole. When a sub-expression is encountered,
 *  a new one is created and the algorithm begins running as if that sub-expression
 *  was the main expression. This creates a tree of expressions that will eventually
 *  be recursed to actually produce our output postfix expression.
 *  @note To determine whether or not an ExpressionComponent is a subexpression, we
 *  simply check to see if the child Stack (mStack) is empty. If it is not empty, then
 *  it is a subexpression that should be recursed. Otherwise, it is just an operator
 *  or an operand (contained in mValue) that should just be put into our output postfix
 *  expression.
 */
class ExpressionComponent
{
    // Public Methods
    public:
        /**
         *  @brief A constructor accepting a size for the child Stack.
         *  @param parent A pointer to the parent ExpressionComponent.
         *  @param value The operand or operator value to associate with this ExpressionComponent.
         *  @param stackSize The size for our child Stack.
         */
        ExpressionComponent(ExpressionComponent *parent, const char &value, const size_t &stackSize = 0) : mParent(parent),
        mValue(value), mStack(stackSize)
        {
            sExpressionComponentSet.insert(sExpressionComponentSet.end(), this);
        }

        //! Standard destructor.
        ~ExpressionComponent(void)
        {
            sExpressionComponentSet.erase(this);
        }

        /**
         *  @brief Gets the operator or operand value associated with this ExpressionComponent.
         *  @return The operator or operand value associated with this ExpressionComponent.
         *  @note This will be some garbage value if !getStack().isEmpty() is true. In that case,
         *  this will be a subexpression and mStack should be recursed.
         */
        char &getValue(void) { return mValue; }

        /**
         *  @brief Gets the child Stack.
         *  @return A reference to the child Stack stored in this ExpressionComponent.
         *  @note If getStack().isEmpty() is true then this is merely an operator operand
         *  value. In that case, the return getValue() should be thrown into the postfix
         *  expression output.
         */
        Stack<ExpressionComponent *> &getStack(void) { return mStack; }

        /**
         *  @brief Returns a pointer to the parent ExpressionComponent.
         *  @return A pointer to the parent ExpressionComponent.
         *  @note This will return NULL if this ExpressionComponent is at the uppermost level.
         */
        ExpressionComponent *getParent(void) { return mParent; }

        /**
         *  @brief Returns whether or not this is a sub expression.
         *  @return A boolean representing whether or not this is a sub expression.
         *  @note This is a shortcut to !getStack().isEmpty()
         */
         bool isExpression(void) { return !mStack.isEmpty(); }

    // Private Members
    private:
        /**
         *  @brief A pointer to the parent ExpressionComponent.
         *  @note This is NULL at the uppermost level expression.
         */
        ExpressionComponent *mParent;

        /**
         *  @brief The operator or operand value this ExpressionComponent represents.
         *  @note This is not used if !mStack.isEmpty() is true which means that is a subexpression
         *  to be recursed.
         */
        char mValue;

        /**
         *  @brief The child Stack associated with this ExpressionComponent.
         *  @note This is not used if mStack.isEmpty() is true which means this ExpressionComponent
         *  is merely an operator operand value.
         */
        Stack<ExpressionComponent *> mStack;
};

/**
 *  @brief Returns whether or not the input is an operator symbol.
 *  @param input The character to test.
 *  @return A boolean representing whether or not input is an operator symbol.
 */
static bool isOperator(const char &input)
{
    switch (input)
    {
        case '*':
        case '+':
        case '-':
        case '/':
        case '^':
            return true;
    }

    return false;
}

/**
 *  @brief Returns whether or not second has greater or equal operator precedence to first.
 *  @param first The first operator to check.
 *  @param second The second operator to compare.
 *  @return A boolean representing whether or not second has a greater or equal operator precedence
 *  to first.
 */
static bool hasEqualOrGreaterPrecedence(const char &first, const char &second)
{
    // Same operator, equal precedence.
    if (first == second)
        return true;

    bool result = false;

    // Here we abuse fall-through a little bit to determine precedence.
    switch (second)
    {
        case '^':
            result = !result ? first == '*' : result;
        case '*':
            result = !result ? first == '/' : result;
        case '/':
            result = !result ? first == '+' : result;
        case '+':
            result = !result ? first == '-' : result;
        // Subtraction is implicity handled as it has no precedence over anything else.
    }

    return result;
}

/**
 *  @brief Recurses down an expression, deleting memory along the way.
 *  @param topLevel The uppermost level of an arbitrary expression to start
 *  recursion at.
 *  @param output A reference to a string to write the result to.
 */
static void expressionRecurse(ExpressionComponent *topLevel, string &output)
{
    Stack<ExpressionComponent *> &stack = topLevel->getStack();

    // Anything left in the stack is dumped into the result
    while (!stack.isEmpty())
    {
        // What is on top?
        ExpressionComponent *component = stack.top();

        // If we're not a subexpression, just output the value.
        if (!component->isExpression())
            output += component->getValue();
        // Otherwise, we should recurse down this sub-expression.
        else
            expressionRecurse(component, output);

        // Kill the current component and pop the stack.
        delete component;
        stack.pop();
    }
}

/**
 *  @brief Converts an infix notation string to postfix notation.
 *  @param infix The infix notation string to convert.
 *  @return An std::string containing the postfix variation.
 *  @throw std::overflow_error Thrown by the internal Stack implementation
 *  when there is too many items pushed to a given Stack instance.
 *  @throw std::underflow_error Thrown by the internal Stack implementation
 *  when there is not enough items on the Stack to pop or read.
 *  @throw std::runtime_error Thrown when the algorithm is given an infix
 *  expression that cannot be converted because there is problems with the
 *  input. This is usually because there is problems with parantheses balancing.
 */
static string infixToPostFix(const char *infix)
{
    string postfix; // Create the result string

    // Initialize a top-level expression
    ExpressionComponent *topLevelExpression = new ExpressionComponent(NULL, 0, 256);

    // The current expression we're focused on
    ExpressionComponent *currentExpression = topLevelExpression;

    // Scan from left to right
    long int iteration = -1;
    // How far are we buried in sub-expressions? (should be zero when we're done)
    size_t expressionDepth = 0;

    while (infix[++iteration] != 0x00)
    {
        // What is the current stack?
        Stack<ExpressionComponent *> &stack = currentExpression->getStack();

        // What is our current character?
        const char &currentCharacter = infix[iteration];

        // Faster determination for opening and ending expressions.
        switch (currentCharacter)
        {
            // Ignore whitespace.
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                continue;

            // Starting a new expression?
            case '(':
            {
                // Create a sub-expression and push it to our stack.
                ExpressionComponent *subExpression = new ExpressionComponent(currentExpression, 0, 256);
                stack.push(subExpression);

                currentExpression = subExpression;

                ++expressionDepth;

                continue;
            }

            // Ending the current expression?
            case ')':
            {
                // Recurse down the current expression
                expressionRecurse(currentExpression, postfix);
                // Focus on our parent expression now.
                currentExpression = currentExpression->getParent();

                // The top level expression has a NULL parent. So we encountered closing parantheses where they shouldn't be.
                if (!currentExpression)
                    throw runtime_error("Encountered unexpected closing parantheses in infix expression!");

                --expressionDepth;

                continue;
            }
        }

        // New perform the regular logic
        if (isOperator(currentCharacter))
        {
            // If there's anything on the stack that has higher (or equal) precedence, pop it and write to postfix
            // This would be an optimizatipn point to reverse the expression ordering but stack.top() is not guaranteed
            // to work as it is possible for the stack to be empty. Thus, stack.top() will create an std::underflow_error.
            while (!stack.isEmpty() && hasEqualOrGreaterPrecedence(currentCharacter, stack.top()->getValue()))
            {
                ExpressionComponent *topComponent = stack.top();

                postfix += topComponent->getValue();
                delete topComponent;

                stack.pop();
            }

            // Push the current operator to the stack
            ExpressionComponent *newComponent = new ExpressionComponent(currentExpression, currentCharacter);

            stack.push(newComponent);
        }
        else
            postfix += currentCharacter; // We got an operand, write it to the output
    }

    // Warn if we have errors with the parantheses
    if (expressionDepth != 0)
        cerr << "WARNING: There are " << expressionDepth << " unclosed opening parantheses. Output may not be correct." << endl;

    // Recurse down the line to finish outputting as we can have stuff left after the main algorithm
    expressionRecurse(topLevelExpression, postfix);

    // Top level will be the only thing left at this point because expressionRecurse cleans up as it goes.
    delete topLevelExpression;

    return postfix;
}

/**
 *  @brief Helper method that determines if the input value is a alphabetical.
 *  @param input The character to check.
 *  @return A boolean representing whether or not the input is a alphabetical value.
 */
bool isAlphabetical(const char &input)
{
    return (input >= UPPERCASE_LOWER && input <= UPPERCASE_UPPER) || (input >= LOWERCASE_LOWER && input <= LOWERCASE_UPPER);
}

/**
 *  @brief Helper method that determines if the input value is a numeric.
 *  @param input The character to check.
 *  @return A boolean representing whether or not the input is a numeric value.
 */
bool isNumeric(const char &input)
{
    return input >= NUMERIC_LOWER && input <= NUMERIC_UPPER;
}

//! A helper typedef for an "opcode" (operator) method pointer.
typedef float (*rpnOpcodeMethodPointer)(Stack<float> &stack);

/**
 *  @brief Operator definition for '+' in the evaluator.
 *  @param stack The stack to use when pulling operands.
 *  @return The result of the calculation.
 */
float rpnAdd(Stack<float> &stack)
{
    const float lhs = stack.top();
    stack.pop();

    const float rhs = stack.top();
    stack.pop();

    return lhs + rhs;
}

/**
 *  @brief Operator definition for '-' in the evaluator.
 *  @param stack The stack to use when pulling operands.
 *  @return The result of the calculation.
 */
float rpnSubtract(Stack<float> &stack)
{
    const float rhs = stack.top();
    stack.pop();

    const float lhs = stack.top();
    stack.pop();

    return lhs - rhs;
}

/**
 *  @brief Operator definition for '*' in the evaluator.
 *  @param stack The stack to use when pulling operands.
 *  @return The result of the calculation.
 */
float rpnMultiply(Stack<float> &stack)
{
    const float rhs = stack.top();
    stack.pop();

    const float lhs = stack.top();
    stack.pop();

    return lhs * rhs;
}

/**
 *  @brief Operator definition for '/' in the evaluator.
 *  @param stack The stack to use when pulling operands.
 *  @return The result of the calculation.
 */
float rpnDivide(Stack<float> &stack)
{
    const float rhs = stack.top();
    stack.pop();

    const float lhs = stack.top();
    stack.pop();

    return lhs / rhs;
}

/**
 *  @brief Operator definition for '^' in the evaluator.
 *  @param stack The stack to use when pulling operands.
 *  @return The result of the calculation.
 */
float rpnPow(Stack<float> &stack)
{
    const float rhs = stack.top();
    stack.pop();

    const float lhs = stack.top();
    stack.pop();

    return pow(lhs, rhs);
}

/**
 *  @brief Evaluates the input postfix expression using a variable map for the variable
 *  values and returns the result.
 *  @param input The input postfix string.
 *  @param rpnVariables An std::unordered_map to map out variables to values.
 *  @throw std::runtime_error Thrown when an unresolved variable or an unknown operator
 *  is encountered.
 *  @return The value of the expression.
 */
float evaluatePostfix(const string &input, unordered_map<char, float> &rpnVariables)
{
    // Initialize the RPN opcode table
    static unordered_map<char, rpnOpcodeMethodPointer> rpnOpcodes;

    // If the opcode table is empty, then this is the first init.
    if (rpnOpcodes.empty())
    {
        rpnOpcodes['+'] = rpnAdd;
        rpnOpcodes['-'] = rpnSubtract;
        rpnOpcodes['*'] = rpnMultiply;
        rpnOpcodes['/'] = rpnDivide;
        rpnOpcodes['^'] = rpnPow;
    }

    // Initialize an empty stack.
    Stack<float> rpnStack(256);

    // Begin iteration
    for (auto it = input.begin(); it != input.end(); it++)
    {
        const char &currentCharacter = *it;

        // FIXME (Robert MacGregor): Random blank characters?
        if (currentCharacter == 0)
            continue;

        // If this character not an operator, push it to the stack
        if (rpnOpcodes.count(currentCharacter) == 0)
        {
            float currentValue;

            // If it is an alphabetical character, perform a lookup
            if (isAlphabetical(currentCharacter))
                if (rpnVariables.count(currentCharacter) == 0)
                    throw runtime_error("Encountered unresolved variable");
                else
                    currentValue = rpnVariables[currentCharacter];
            else
            {
                // If this isn't some numeric, then we've encountered some unknown operator
                if (!isNumeric(currentCharacter))
                    throw runtime_error("Encountered unknown operator");

                // Hacky hack to convert a char into a usable float
                string temp;
                temp += currentCharacter;
                currentValue = atof(temp.data());
            }

            // Push it to the stack
            rpnStack.push(currentValue);
        }
        // Now its got to be an operand, so its go time.
        else
        {
            const float result = rpnOpcodes[currentCharacter](rpnStack);
            rpnStack.push(result);
        }
    }

    // There should only be one element on the stack now.
    if (rpnStack.getElementCount() != 1)
        throw runtime_error("Stack Size is not Equal to One Upon Completion");

    return rpnStack.top();
}

/**
 *  @brief Evaluates the postfix input (without a variable table)
 *  and returns the result.
 *  @param input The input postfix expression to evaluate.
 *  @return The result of the calculations.
 */
float evaluatePostfix(const string &input)
{
    unordered_map<char, float> noVariables;

    return evaluatePostfix(input, noVariables);
}


/**
 *  @brief Main entry point.
 *  @param argc The number of space delineated command line parameters
 *  passed into the program.
 *  @param argv An array of strings representing the space delineated
 *  command line parameters.
 */
int main(int argc, char *argv[])
{
    // The input infix expression.
    const char *infix = "(A + B) / (C + D)";

    // Initialize our return code to good in hopes that nothing does blow up :)
    int returnCode = RETURN_GOOD;

    // Get the result and print
    string result;
    try
    {
        cout << "Attempting to convert '" << infix << "' to postfix notation." << endl;
        result = infixToPostFix(infix);

        cout << "Compiled Result: " << result << endl;
    }
    // overflow_error can be thrown by internal Stack implementation.
    catch (overflow_error &e)
    {
        cerr << "ERROR: Failed to convert the infix expression to postfix notation due to a Stack overflow! (Check your input.)" << endl;
        returnCode |= RETURN_OVERFLOW;
    }
    // underflow_error can be thrown by the internal Stack implementation.
    catch (underflow_error &e)
    {
        cerr << "ERROR: Failed to convert the infix expression to postfix notation due to a Stack underflow! (Check your input.)" << endl;
        returnCode |= RETURN_UNDERFLOW;
    }
    // runtime_error can be thrown by the algorthim itself.
    catch(runtime_error &e)
    {
        cerr << "Failed to convert the infix expression to postfix notation due a runtime error! (Check your input.) " << endl;
        cerr << "What: " << e.what() << endl;
        returnCode |= RETURN_RUNTIME;
    }

    // Make sure we cleaned up properly?
    if (sExpressionComponentSet.size() != 0)
    {
        cerr << "WARNING: Failed to deallocate " << sExpressionComponentSet.size() << " ExpressionComponent instances!" << endl;

        // Make a copy of the expression set because iterators will be invalidated
        unordered_set<ExpressionComponent *> temporarySet = sExpressionComponentSet;

        // The deletion here also removes it from sExpressionComponentSet which invalidates iterators in sExpressionComponentSet
        for (auto it = temporarySet.begin(); it != temporarySet.end(); it++)
            delete *it;

        cerr << "Cleaned up that leaked memory." << endl;

        returnCode |= RETURN_MEMLEAKED;
    }

    // If we're good, eval the output
    if (returnCode & RETURN_GOOD)
    {
        cout << "Evaluating the postfix expression ..." << endl;

        // Set up the variable map.
        unordered_map<char, float> variableMap;
        variableMap['A'] = 1.0f;
        variableMap['B'] = 2.0f;
        variableMap['C'] = 3.0f;
        variableMap['D'] = 4.0f;

        // Print out the variable values
        for (auto it = variableMap.begin(); it != variableMap.end(); it++)
        {
            pair<const char, float> &currentPair = *it;

            cout << "\t For " << currentPair.first << " = " << currentPair.second << endl;
        }

        // Calculate the result and push it out
        try
        {
            cout << "End Result: " << evaluatePostfix(result, variableMap) << endl;
        }
        catch (runtime_error &e)
        {
            cerr << "Failed to evaluate postfix expression due to a runtime error." << endl;
            cerr << "What: " << e.what() << endl;

            returnCode |= RETURN_RUNTIME;
        }
    }

    // We're done.
    return returnCode;
}
