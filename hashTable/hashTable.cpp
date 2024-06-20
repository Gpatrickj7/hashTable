// HashTable.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <climits>
#include <string>
#include <time.h>
#include <algorithm>
#include <vector>


using namespace std;

//global defintions


const unsigned int CAPACITY = 5000;



//structure for holding course info
struct Course {

    string courseNumber;   //the unique identifier for courses
    string courseName;
    string coursePrereq;   //prereqs also have an identifier and are separate courses within the file 

};


class HashTable {

private:

    struct Node {

        Course course;
        unsigned int key;
        Node *next;

        //default constructor
        Node() {

            key = UINT_MAX;
            next = nullptr;
        }

        //initialize with a course
        Node(Course aCourse) : Node() {

            course = aCourse;
        }


        //initialize with a course and key
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) {

            key = aKey;

        }
    };
    
    //vector to store nodes
    vector<Node> nodes;

    unsigned int tableSize = CAPACITY;

    unsigned int hash(int key);

public:

    HashTable();
    HashTable(unsigned int size);
    virtual ~HashTable();
    void Insert(Course course);
    void PrintAll();
    void Remove(string courseNumber);
    Course Search(string courseNumber);
    size_t Size();
};


HashTable::HashTable() {

    //initialize the structs for holding courses
    nodes.resize(tableSize);
}

HashTable::HashTable(unsigned int size) {
    // invoke local tableSize to size with this->

    this->tableSize = size;


    // resize nodes size
    nodes.resize(size);

}

HashTable::~HashTable() {

    //Implements logic to free storage when class is destroyed
    nodes.erase(nodes.begin());

    // erase nodes beginning
}





int main()
{
    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
