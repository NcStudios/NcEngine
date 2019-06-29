#include <iostream>
#include <memory>
using namespace std;


void SwapPtrs(int*, int*);
void SwapValues(int*, int*);

class Foo
{
    public:
        Foo(){
            val = 4;
        }
        int val;
};



int main(int argc, char** argv){

    Foo foo;
    cout << foo.val;


    return 0;
}


void SwapPtrs(int *a, int *b){
    int *temp = a;
    a = b;
    b = temp;

}

void SwapValues(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void SwapRef(int &a, int &b){
    int temp = a;

}


