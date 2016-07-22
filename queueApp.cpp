#include <iostream>

#include "Queue.h"

using namespace std;

int main(int argc, char *argv[])
{
    Queue<int> q1;

    for (int i = 10; i < 100; i += 10)
    {
        q1.add(i);
        cout << q1 << endl;
    }

    while (!q1.isEmpty())
    {
        cout << q1 << endl;
        q1.pop();
    }

    cout << "Job Done" << endl;

    return 1;
}
