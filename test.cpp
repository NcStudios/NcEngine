#include <iostream>

template <typename T>

class Node{
    public:
        Node(T val) { value = val; }
        ~Node(){}

        T GetValue() { return value; }

        void PrintValue() { 
            std::cout << value << '\n';
        }

        bool Compare(T val){ 
            return value > val ? true : false;
        }

        void AddValue(T val){
            if (ContainsValue(val)) {
                return;
            }
                
            if (Compare(val)){
                if (rightChild == nullptr){
                    rightChild = new Node<T>(val);
                }
                else{
                    rightChild->AddValue(val);
                }    
            }

            else{
                if (leftChild == nullptr){
                    leftChild = new Node<T>(val);
                }  
                else{
                    rightChild->AddValue(val);
                }    
            }
        }


        bool ContainsValue(T val){
            if (val == value){
                return true;
            } 

            if (Compare(val)){
                return rightChild == nullptr ? false : rightChild->ContainsValue(val);
            }

            else{
                return leftChild == nullptr ? false : leftChild->ContainsValue(val);
            }
        }

        void Meaningless(){
            rightChild->PrintValue();
            leftChild->PrintValue();
        }

    private:
        T value;
        Node *rightChild = nullptr;
        Node *leftChild  = nullptr;
};


enum MENUOPTIONS {ADD = 1, SEARCH, EXIT};

template <typename T>
void MAIN(T);
auto GetValueFromUser();
void PrintMenu();
void DoMenuChoice(int);
template <typename T>
void AddValue(Node<T> &);
template <typename T>
void SearchValue(Node<T> &);


int main(){


    MAIN(5);
    return 0;
}

template <typename T>
void MAIN(T initVal){




    int *int_p = new(int);
    char *char_p = new(char);
    void *void_p;

    *int_p = 1;
    *char_p = 'a';

    std::cout << "int pointer size: " << sizeof(int_p) << std::endl
         << "int size: "         << sizeof(*int_p) << std::endl
         << "char pointer size: " << sizeof(char_p) << std::endl
         << "char size: "         << sizeof(*char_p) << std::endl
         << "void pointer size: " << sizeof(void_p) << std::endl;

    /*
    Node<T> TestNode(initVal);

    int menuChoice;

    PrintMenu();
    std::cin >> menuChoice;

    while(menuChoice != 3){

        switch(menuChoice){
            case ADD:
                AddValue(TestNode);
                break;
            case SEARCH:
                SearchValue(TestNode);
                break;
        }

        PrintMenu();
        std::cin >> menuChoice;
    
    }
    */
}

auto GetValueFromUser(){
    std::cout << "Enter a value: " << '\n';
    int val;
    std::cin >> val;
    return val;
}


void PrintMenu(){
    std::cout << "1. Add Value"    << '\n'
              << "2. Search Value" << '\n'
              << "3. Exit"         << '\n' << '\n';
}

template <typename T>
void AddValue(Node<T> &node){
    T val = GetValueFromUser();
    node.AddValue(val);
    std::cout << '\n';
}

template <typename T>
void SearchValue(Node<T> &node){
    T val = GetValueFromUser();
    bool result = node.ContainsValue(val);
    if (result){
        std::cout << "Tree contains: " << val << '\n' << '\n';
    }
    else{
        std::cout << "Tree does not contain: " << val << '\n' << '\n';
    }

}