// HashTable.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <climits>
#include <string>
#include <time.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>


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

//hash value for a key. unsigned to disallow negative indices
//parameter is a key to be hashed
unsigned int HashTable::hash(int key) {

    //returns the caclulated hash value
    return key % tableSize;
}


//insert operation to insert a course 
void HashTable::Insert(Course course) {

    //create the key for a given course (by its unique identifier)
    unsigned key = hash(atoi(course.courseNumber.c_str()));

    //retrieves the node using the key we just created
    Node* oldNode = &(nodes.at(key));

    //if no entry is found for the key
    if (oldNode == nullptr) {

        //assign this node to the position of the key
        Node* newNode = new Node(course, key);
        nodes.insert(nodes.begin() + key, (*newNode));

       
    } //else if node is not used
    else {
        if (oldNode->key == UINT_MAX) {
            
            //assign the oldNode to UINT_MAX, set to key, set oldNode to course, then set oldNode next to null ptr
            oldNode->key = key;

            oldNode->course = course;

            oldNode->next = nullptr;
            
        }

    }
}
//lists all courses. this is where any tweaks will likely be made for output in a certain order. maybe a loop inserted to check a course, output it or not, hold it or something... not sure (3:35 pm 6/21/24)
void HashTable::PrintAll() {
    
    //for loop to iterate through the nodes from the beginning to the end. 
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {

        //if key is not equal to UNIT_MAX
        if (it->key != UINT_MAX) {

            //output course info... number, name, prereqs, etc.
            cout << "Key: " << it->key << ": " << it->course.courseNumber << " | " << it->course.courseName << " | " << it->course.coursePrereq << endl;

            //next node is equal to next iteration
            Node* node = it->next;

            //while node is not null ptr
            while (node != nullptr) {

                //output course info 
                cout << "Key: " << it->key << ": " << it->course.courseNumber << " | " << it->course.courseName << " | " << it->course.coursePrereq << endl;

                //node equal to next node
                node = node->next;
            }
        }
    }
}


//remove operation for the hash table 
void HashTable::Remove(string courseNumber) {

    //set key - to hash atoi courseNumber c_string. learned atoi turns strings into integers. pretty cool stuff
    unsigned int key = hash(atoi(courseNumber.c_str()));

    Node* node = &(nodes.at(key));   //bit wise AND "&"

    //if node is not equal to UINT_MAX
    if (node->key != UINT_MAX) {

        //if the nodes courseNumber matches the courseNumber passed to the method, the .compare returns a 0. basic logic that is easy to follow. Have used .compare elsewhere in my projects
        if (node->course.courseNumber.compare(courseNumber) == 0) {

            cout << "remove top nodes for " << courseNumber << endl;

            //if next node is nullptr ("empty")
            if (node->next == nullptr) {

                node->key = UINT_MAX; //if the ptr is null, resets the key to default

                return;
            }
            else {
                //sets the keys nodes to the next one
                nodes.at(key) = *(node->next);
                return;
            }

        } //part of the operation that actually removes the specified element
        else {
            //new node for current
            Node* cur = node->next;         //c++ pointers are nice and easy compared to java. javas points sit inbetween the next and previous and forces some weird logic in methods like this. I think it is better due to c++ being a lower level
            
            //new node for previous
            Node* pre = node;

            //while current is not nullptr
            while (cur != nullptr) {

                //comparision of iterators nodes courseNumber to the one passed
                if (cur->course.courseNumber.compare(courseNumber)) {

                    //previous nodes next set to current nodes next
                    pre->next = cur->next;

                    //REMOVE
                    delete cur;

                    cur = nullptr;

                    return;
                }

                pre = cur;

                cur = cur->next;


                
            }

        }

    }

}
//search operation
Course HashTable::Search(string courseNumber) {

    //course object
    Course course;

    //creates a key for the given courseNumber
    unsigned int key = hash(atoi(courseNumber.c_str()));

    Node* node = &(nodes.at(key));

    //if entry is found for the key
    if (node != nullptr && node->key != UINT_MAX && node->course.courseNumber.compare(courseNumber) == 0) {

        //return node course
        return node->course;

    }

    //if no entry is found for the key
    if (node == nullptr || node->key == UINT_MAX) {

        //return the course
        return course;
    }

    //while node is not nullptr
    while (node != nullptr) {

        //if current nodes courseNumber matches the passed courseNumber, return it
        if (node->key != UINT_MAX && node->course.courseNumber.compare(courseNumber)) {

            //return it
            return node->course;

        }
        //node equals next node
        node = node->next;

    }

    return course;
    
}






//load the courses into the container which is our hashtable
void loadCourses(string csvPath, HashTable* hashTable) {
    //simplistic parsing method i researched with no delimiters as of 6/21/2024 at 7:16

    //the file which has the csvPath which is defined in main().
    std::ifstream file;

    const std::string fileName = csvPath;

    file.open(fileName);

    

    //checks for errors regarding opening the file. might encase this operations actually work within a try/catch
    if (!file.is_open()) {
        

        int errorNumber = errno;

        char errorMessage[80];

        strerror_s(errorMessage, sizeof(errorMessage), errorNumber);

        std::cerr << "Error opening file '" << fileName << "': " << errorMessage << std::endl;

        

        return;
    }

    //line variable
    std::string line;

    std::getline(file, line);



    while (std::getline(file, line)) {

        std::stringstream lineStream(line);

        std::string cell;

        std::vector<std::string> parsedRow;

        while (std::getline(lineStream, cell, ',')) {

            parsedRow.push_back(cell);

        }

        // Ensure we have at least 2 columns (course number and name)
        if (parsedRow.size() >= 2) {
            Course course;
            course.courseNumber = parsedRow[0];
            course.courseName = parsedRow[1];

            // Check if there's a prerequisite
            if (parsedRow.size() > 2) {
                course.coursePrereq = parsedRow[2];
            }
            else {
                course.coursePrereq = "None";
            }

            // Insert the course into the hash table
            hashTable->Insert(course);

            cout << "Loaded: " << course.courseNumber << " | " << course.courseName << " | " << course.coursePrereq << endl;
        }
        else {
            cout << "Warning: Skipping invalid line in CSV: " << line << endl;
        }
    }
    file.close();
    cout << "File loading complete." << endl;

}
    


//display course information
void displayCourse(Course course) {

    //outputs course info
    cout << course.courseNumber << " | " << course.courseName << " | " << course.coursePrereq << endl;

    return;
}






int main()
{
    std::cout << "Hello World!\n";

    HashTable courseTable;

    string csvPath = "ABCU_Advising_Program_Input.csv";



    loadCourses(csvPath, &courseTable);



    

    
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
