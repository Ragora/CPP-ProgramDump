/**
 *  @file main.cpp
 *  @brief Driver implementation to test the HashTable class.
 *  @author Robert MacGregor
 */

#include <string>
#include <iostream>

#include "HashTable.h"

using namespace std;

struct Student
{
    string name; // Key
    string field;
    int quarter;

    Student(const string &name, const string &field, const int &quarter) : name(name), field(field),
    quarter(quarter)
    {

    }

    friend ostream& operator <<(ostream &stream, const Student &student)
    {
        stream << student.name << "\t" << student.field << "\t" << "Q" << student.quarter;
        return stream;
    }
};

struct Vehicle
{
    string vmake;
    string model;
    string license; // Key

    Vehicle(const string &vmake, const string &model, const string &license) : vmake(vmake), model(model), license(license)
    {

    }

    friend ostream& operator <<(ostream &stream, const Vehicle &vehicle)
    {
        stream << vehicle.vmake << "\t" << vehicle.model << "\t" << vehicle.license;
        return stream;
    }
};

struct Dictionary
{
    string word; // Key
    string definition;

    Dictionary(const string &word, const string &definition) : word(word), definition(definition)
    {

    }

    friend ostream& operator <<(ostream &stream, const Dictionary &dictionary)
    {
        stream << dictionary.word << " means " << dictionary.definition;
        return stream;
    }
};

//! Helper type used to populate the word list initially.
typedef struct
{
    string word;
    string definition;
} WordPairs;

/**
 *  @brief Main entry point of the program.
 *  @param argc The number of arguments that can be
 *  found in argv.
 *  @param argv The space-delineated parameter list passed
 *  in the operating system.
 */
int main(int argc, char *argv[])
{
    HashTable<Dictionary> table;

    WordPairs initialWords[] =
    {
        { "Phone", "Make Calls" },
        { "Computer", "Do Computations" },
        { "Programming", "Write Code" },
        { "Test", "Make sure it works!"},
        { "Dive", "Leap in head first!"},
        { "Slap", " ... with a wet fish!"},
        { "Decompile", "Machine Code -> Human Readable Code"},
        { "Compile", "Human Readable Code -> Machine Code"},
        { "Processor", "Does mystical black magic things."},
        { "Register", "Temporily stores a value."},
        { "RAM", "Temporarily stores values"},
        { "Word", "Verbal utterance with meaning"},
        { "Key", "Keyboard button!"},
        { "Light", "Luminous"},
        { "Luma", "Light"},
    };

    // Add the words
    for (size_t iteration = 0; iteration < sizeof(initialWords) / sizeof(WordPairs); iteration++)
        table.add(initialWords[iteration].word,
                  new Dictionary(initialWords[iteration].word, initialWords[iteration].definition));

    cout << table << endl;

    // Query the user for their choice
    int userChoice = -1;
    while (userChoice != 0)
    {
        cout << "What do you want to do?" << endl;
        cout << "0.) Quit" << endl;
        cout << "1.) Add a Word" << endl;
        cout << "2.) Find a Word" << endl;
        cout << "Choice: ";

        cin >> userChoice;

        switch (userChoice)
        {
            case 1: // Add a Word
            {
                char word[256];
                char definition[256];

                cout << "Type a word: ";
                cin.ignore(256, '\n'); // Ensure that anything left in cin doesn't get dumped into word
                cin.getline(word, sizeof(word) / sizeof(char));

                cout << "Type a definition: ";
                cin.getline(definition, sizeof(definition) / sizeof(char));

                // Add the Lookup
                table.add(word, new Dictionary(word, definition));
                break;
            }

            case 2: // Find a word
            {
                string word;

                cout << "Type a word: ";
                cin >> word;

                Dictionary *result = table.find(word);

                if (!result)
                    cout << "No such word: '" << word << "'!" << endl;
                else
                    cout << word << " means " << result->definition << endl;

                break;
            }

            // Exit. The while loop terminates if userOption == 0, so we just use this to prevent the
            // Printing of "Unknown option: 0"
            case 0:
                break;

            default:
                cout << "Unknown option: " << userChoice << endl;
        }
    }

    return 0;
}
