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
#include <string.h>
#include <cctype>
#include <limits>


using namespace std;

//global defintions


const unsigned int CAPACITY = 5000;



//structure for holding course info
struct Course {

    string courseNumber;   //the unique identifier for courses
    string courseName;
    string coursePrereq;   //prereqs also have an identifier and are separate courses within the file 

};

//helper function for comparing strings. tried to use strcasecmp but that is not compatible with windows
bool caseInsensitiveStringCompare(const string& str1, const string& str2) {

    return str1.size() == str2.size() &&

        equal(str1.begin(), str1.end(), str2.begin(),

            [](char c1, char c2) { return tolower(c1) == tolower(c2); });
}


class HashTable {

private:

    struct Node {

        Course course;
        unsigned int key;
        Node* next;

        Node() : key(UINT_MAX), next(nullptr) {}
        Node(Course aCourse) : Node() { course = aCourse; }
        Node(Course aCourse, unsigned int aKey) : Node(aCourse) { key = aKey; }
    };
    //vector to store nodes
    vector<Node> nodes;
    unsigned int tableSize;





    //hash value for a key. unsigned to disallow negative indices
    //parameter is a key to be hashed
    //implementing a newer improved hashing algorithm know as djb2. running into infinite loops for some inputs when searching.

    unsigned int hash(const string& courseNumber) {       //this function was not working at all. it gave every entry "0" as its key. attempting to fix it as of (3:58 6/22/24).

        string upperCourseNumber = courseNumber;
        transform(upperCourseNumber.begin(), upperCourseNumber.end(), upperCourseNumber.begin(), ::toupper);

        unsigned int hash = 5381; // Different starting value
        for (char ch : upperCourseNumber) {
            hash = ((hash << 5) + hash) + ch; // (hash * 33) + ch
        }
        //returns the caclulated hash value
        return hash % tableSize;
    }

public:

    HashTable() : tableSize(CAPACITY) {
        nodes.resize(tableSize);
    }

    HashTable(unsigned int size) : tableSize(size) {
        nodes.resize(tableSize);
    }

    ~HashTable() {
        nodes.clear();
    }

    






    //insert operation to insert a course 
    void Insert(Course course) {

        //create the key for a given course (by its unique identifier)
        //unsigned key = hash(atoi(course.courseNumber.c_str()));
        unsigned key = hash(course.courseNumber);

        //prints when a course is being inserted for testing purposes
        cout << "Inserting course: " << course.courseNumber << " at key: " << key << endl;

        //retrieves the node using the key we just created
        Node* newNode = new Node(course, key);

        //if no entry is found for the key
        if (nodes[key].key == UINT_MAX) {
            nodes[key] = *newNode;
        }
        else {
            //handle collision by appending to the end of the linked list
            Node* current = &nodes[key];
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = newNode;
        }
    }



    //lists all courses. this is where any tweaks will likely be made for output in a certain order. maybe a loop inserted to check a course, output it or not, hold it or something... not sure (3:35 pm 6/21/24)
    void PrintAll() {

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
    void Remove(string courseNumber) {
        //set key - to hash courseNumber
        unsigned int key = hash(courseNumber);

        cout << "Attempting to remove course: " << courseNumber << " at key: " << key << endl;

        Node* node = &(nodes[key]);   //bit wise AND "&"
        Node* prev = nullptr;

        //while node is not nullptr
        while (node != nullptr) {
            //comparision of node's courseNumber to the one passed
            if (caseInsensitiveStringCompare(node->course.courseNumber, courseNumber)) {
                if (prev == nullptr) {
                    //if it's the first node in the bucket
                    if (node->next != nullptr) {
                        nodes[key] = *(node->next);
                    }
                    else {
                        nodes[key] = Node();
                    }
                    cout << "Removed top node for " << courseNumber << endl;
                }
                else {
                    //if it's not the first node
                    prev->next = node->next;
                    cout << "Removed node in chain for " << courseNumber << endl;
                }
                delete node;
                return;
            }
            prev = node;
            node = node->next;
        }

        cout << "Course " << courseNumber << " not found for removal." << endl;
    }

    //the original function is not working as intended. using .compare returns a zero when the strings are equal but it is treating any non zero result as a match... modifying it as of (3:26 6/22/2024).
    // final version as of (5:22 6/22/2024)
    //search operation
    Course Search(string courseNumber) {
        // Convert input courseNumber to uppercase
        transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

        //creates a key for the given courseNumber
        unsigned int key = hash(courseNumber);

        //prints info for testing 
        cout << "Searching for course: " << courseNumber << " at key: " << key << endl;

        Node* node = &(nodes[key]);
        int iterations = 0;
        const int MAX_ITERATIONS = 10; // Small number for testing

        //while node is not nullptr
        while (node != nullptr && iterations < MAX_ITERATIONS) {
            //if entry is found for the key
            if (node->course.courseNumber == courseNumber) {
                cout << "Course found!" << endl;
                //return node course
                return node->course;
            }

            //node equals next node
            node = node->next;
            iterations++;
        }

        cout << "Course not found or search terminated after " << MAX_ITERATIONS << " iterations." << endl;
        return Course(); // Return empty course if not found
    }



    //prints the courses alphanumerically
    friend void printSortedCourseList(const HashTable& courseTable) {
        vector<Course> allCourses;

        // Collect all courses from the hash table
        for (const auto& node : courseTable.nodes) {
            if (node.key != UINT_MAX) {
                allCourses.push_back(node.course);
                Node* current = node.next;
                while (current != nullptr) {
                    allCourses.push_back(current->course);
                    current = current->next;
                }
            }
        }

        // Sort the courses
        sort(allCourses.begin(), allCourses.end(),
            [](const Course& a, const Course& b) {
                return a.courseNumber < b.courseNumber;
            });

        // Print the sorted list with prerequisites
        cout << "Course List (sorted alphanumerically):\n";
        for (const auto& course : allCourses) {
            cout << course.courseNumber << ": " << course.courseName;
            if (!course.coursePrereq.empty() && course.coursePrereq != "None") {
                cout << " | Prerequisites: " << course.coursePrereq;
            }
            else {
                cout << " | Prerequisites: None";
            }
            cout << endl;
        }
    }
};










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

    // Check if the file is empty
    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Error: The file '" << fileName << "' is empty." << std::endl;
        file.close();
        return;
    }

    //line variable
    std::string line;

    // removed this line of code "std::getline(file, line);" //

    int lineCount = 0;  // Add a counter for loaded courses

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

            //transforms courseNumber to uppercase
            transform(course.courseNumber.begin(), course.courseNumber.end(), course.courseNumber.begin(), ::toupper);

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
            lineCount++;  // Increment the counter
        }
        else {
            cout << "Warning: Skipping invalid line in CSV: " << line << endl;
        }
    }
    file.close();
    cout << "File loading complete. " << lineCount << " courses loaded." << endl;
}

//display course information
void displayCourse(Course course) {
    //outputs course info
    cout << course.courseNumber << " | " << course.courseName << " | " << course.coursePrereq << endl;
    return;
}






int main()
{
    //variable declaration for main()
    HashTable courseTable;
    int choice = 0;
    bool dataLoaded = false;

    //this is where you should be able to alter the path to your pc's path when checking if this program works. you may need to hard code it before it works like the example above. 
    string csvPath = "CS 300 ABCU_Advising_Program_Input.csv";

    while (choice != 9) {
        cout << "\nMenu:\n";
        cout << "1. Load data structure\n";
        cout << "2. Print course list\n";
        cout << "3. Print course information\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice)) {
            cin.clear(); // clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard invalid input
            cout << "Invalid input. Please enter a number.\n";   //this error sometimes shows itself while using the search function
            continue; // restart the loop
        }

        

        switch (choice) {
        case 1:
            if (!dataLoaded) {
                loadCourses(csvPath, &courseTable);
                dataLoaded = true;
            }
            else {
                cout << "Data has already been loaded.\n";
            }
            break;
        case 2:
            if (dataLoaded) {
                printSortedCourseList(courseTable);
            }
            else {
                cout << "Please load the data first (Option 1).\n";
            }
            break;
        case 3:
            if (dataLoaded) {
                string courseNumber;
                cout << "Enter the course number: ";
                cin >> courseNumber;
                // Input validation
                if (courseNumber.length() > 10 || courseNumber.length() < 4) {
                    cout << "Invalid course number. Please enter a course number between 4 and 10 characters.\n";
                }
                else {
                    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);
                    Course course = courseTable.Search(courseNumber);
                    if (!course.courseNumber.empty()) {
                        displayCourse(course);
                    }
                    else {
                        cout << "Course not found.\n";
                    }
                }
            }
            else {
                cout << "Please load the data first (Option 1).\n";
            }
            break;
        case 9:
            cout << "Thank you for using the program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;










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
