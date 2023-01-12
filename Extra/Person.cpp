#include <string>
#include <iostream>
using namespace std;

class Person
{
public:
    string mColor;
    int mAge;
    string mGender;

    Person(int age, string color, string gender)
    {
        cout << "Called constructor with 3 parameters" << endl;
        mAge = age;
        mGender = gender;
        mColor = color;
    }

    Person(int age)
    {
        cout << "Called constructor with 1 parameters" << endl;
        mColor = "White";
        mGender = "male";
    }

    ~Person()
    {
        cout << "Finishing Person" << endl;
    }
};

class Game
{
    Person person1;
    Person person2;

public:
    // Constructor
    // Object life cycle
    // Initilizer list
    Game() : person1(10, "male", "asd"), person2(10)
    {
        cout << "Creating game object" << endl;
        cout << "Person1 age is: " << person1.mAge << endl;
    }

    ~Game()
    {
        cout << "Finishign game" << endl;
    }
};

int main()
{
    Game g;
}

// Constructor & Desctructor
// Initialization list
// Object life cycle
// When does the desctructor get called?
// Extra:
// What is a pointer?