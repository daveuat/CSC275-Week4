#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

// Using the standard namespace to make things easier.
using namespace std;

mutex mtx; // mutex is used

// Class and program is named LCARS, Library Computer Access and Retrieval System (LCARS for short), after the computer from Star Trek The Next Generation
class LCARS {
public:
    LCARS(const string& filename) : filename(filename) {
        createFile(); // Creates the file for the object is constructed
    }

    // function called 'store' for storing data to the file
    void store(const string& content, const string& rank) {
        unique_lock<mutex> lock(data_mutex); // Lock the mutex to protect the file access
        ofstream file(filename, ios::app); // this creates an ofstream object and appends data in the file

        if (file.is_open()) { // check to see if the file is open
            // The first two lines are for simulating a delay in storing data for multithreading.
            cout << "Storing data to ship's memory banks, please stand by " << rank << ". " << endl;
            std::this_thread::sleep_for(std::chrono::seconds(3)); // pause for 3 seconds during the simulated multithread

            file << content << endl; // write the user inputted data to the file
            file.close(); // closes file
            cout << "Thank you " << rank << ", data is now stored in my memory banks." << endl; // print confirmation message to user
        }
        lock.unlock(); // Unlock the mutex
    }

    // Function for finding stored data based off a keyword or word
    void find(const string& keyword, const string& rank) {
        lock_guard<mutex> lock(mtx);

        ifstream file(filename); // Create an ifstream object to read the file
        string line; // Declare a string variable to store each line read from the file
        bool found = false;

        if (file.is_open()) { // Check if the file is open
            while (getline(file, line)) { // Read each line from the file
                if (line.find(keyword) != string::npos) { // Check if the keyword is found in the current line
                    cout << rank << ", I have retrieved the following data: " << line << endl; // Print what was found to the screen
                    found = true;
                    break;
                }
            }
            file.close(); // Close the file
        }
        // Old display
        // if (!found) {
        //    cout << rank << ", I am unable to locate the requested information. " << keyword << ", does not exist in my memory banks." << endl;
        // }

        // New display to user and makes it all uppercase to look nicer in the response back.
        if (!found) {
            string keyword_upper = keyword;
            for (auto& c : keyword_upper) {
                c = toupper(c);
            }
            cout << rank << ", I am unable to locate the requested information. " << keyword_upper << ", does not exist in my memory banks." << endl;
        }
    }

    // function to find all data stored by the user
    void findAll(const string& rank) {
        lock_guard<mutex> lock(mtx);

        ifstream file(filename); // Create a needed ifstream object for reading the file
        string line; // This declares a string variable to store each line that is read from the file

        cout << rank << ", ship memory banks contain the following data: " << endl; // This prints the a message for the found data
        if (file.is_open()) { // This checks if the file is open or not
            while (getline(file, line)) { // This will read each line from the data file
                cout << line << endl;
            }
            file.close(); // Closes the file when done.
        }
    }

    void clear_LCARS(const string& rank) {
        lock_guard<mutex> lock(mtx);

        ofstream file(filename, ios::trunc);
        if (file.is_open()) {
            file.close();
            cout << "All data that was stored has been erased from my memory banks. " << endl;
        }
    }

    void displayLastThreeLines(const string& rank, const string& filename) {
        while (true) {
            data_mutex.lock(); // lock the mutex while the program reads the file
            ifstream file(filename); // open the file
            vector<string> last_three_lines; // this makes a vector to hold the last 3 entries in LCARS

            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    if (last_three_lines.size() < 3) { // adds current line to the vector
                        last_three_lines.push_back(line);
                    }
                    else {
                        last_three_lines.erase(last_three_lines.begin()); // Removes oldest and just keeps the last 3
                        last_three_lines.push_back(line);
                    }
                }
                file.close(); // closes out the file
            }

            system("cls"); // Clears screen
            // Prints welcome message to the screen, used https://patorjk.com/ ASCII Art Generator
            cout << R"(
  _      _____          _____   _____ 
 | |    / ____|   /\   |  __ \ / ____|
 | |   | |       /  \  | |__) | (___  
 | |   | |      / /\ \ |  _  / \___ \ 
 | |___| |____ / ____ \| | \ \ ____) |
 |______\_____/_/    \_\_|  \_\_____/ 
)" << '\n';
            cout << "-----------------------------------------------------" << endl; // Prints divider to screen

            // Print the last three lines
            cout << "The most recent data is:" << endl;
            for (const auto& line : last_three_lines) {
                cout << line << endl;
            }

            cout << "-----------------------------------------------------" << endl; // Prints divider to screen
            cout << rank << ", specify a command when ready. (store/find/find_all/clear/exit): "; // Print the prompt to user

            data_mutex.unlock(); // unlocks the mutex

            this_thread::sleep_for(chrono::seconds(10)); // pause/sleep for 10 seconds before refresh
        }
    }

private:
    string filename; // Declaring string variable to store the filename for all data
    mutex data_mutex;

    // A function to create the file for stored data
    void createFile() {
        ifstream file_test(filename);
        if (!file_test.good()) { // Needed to add this because it kept overwriting the file. This checks to see if it already exists.
            ofstream file(filename);
            file.close(); // Now closes the file
        }
    }
};

int main() {
    LCARS lcars("lcars_data.txt"); // Data file name for stored data based off the star trek computer LCARS for short

    string input;
    string rank;
    string keyword;

    cout << "Welcome to ";
    // Prints welcome message to the screen, used https://patorjk.com/ ASCII Art Generator
    cout << R"(
  _      _____          _____   _____ 
 | |    / ____|   /\   |  __ \ / ____|
 | |   | |       /  \  | |__) | (___  
 | |   | |      / /\ \ |  _  / \___ \ 
 | |___| |____ / ____ \| | \ \ ____) |
 |______\_____/_/    \_\_|  \_\_____/ 
)" << '\n';
    cout << "Please verify if you are Commander or Higher rank of the Enterprise crew and login. " << '\n';
    cout << "Please enter your rank (Captain or Commander): ";
    getline(cin, rank);

    if (rank != "Captain" && rank != "Commander") {
        cout << "That rank is invalid. Contacting the Chief of Security. Please remain here. " << endl;
        return 0;
    }

    thread display_thread(&LCARS::displayLastThreeLines, &lcars, rank, "lcars_data.txt");
    display_thread.detach(); // Detach the thread so it runs in the background

    bool running = true;
    while (running) { // main loop
        cout << rank << ", specify a command when ready. (store/find/find_all/clear/exit): "; // prompt for user with commands
        getline(cin, input);

        if (input == "store") {
            cout << "Please enter the data you want to store: ";
            getline(cin, input);
            lcars.store(input, rank);
        }
        else if (input == "find") { // find a single item by keyword
            cout << "Please enter the keyword you want to find: ";
            getline(cin, keyword);
            lcars.find(keyword, rank);
        }
        else if (input == "find_all") { // if user types find_all it will display all stored input
            lcars.findAll(rank);
        }
        else if (input == "clear") { // clears stored data
            string confirmation;
            cout << "Do you confirm the purging of my memory banks? This action is irreversible. " << rank << ", please respond with 'affirmative' to continue: "; // prompt for confirmation of clearing of data
            getline(cin, confirmation);
            if (confirmation == "affirmative") // confirmation of clear/wipe
            {
                lcars.clear_LCARS(rank); // calls clear_LCARS and clears data
            }
            else // this will be displayed if the input does not match the above
            {
                cout << rank << ", clear operation has been cancelled. " << endl;
            }
        }
        else if (input == "exit") { // check to see if user wants to exit
            running = false; // sets running to false and exits main loop
        }
        else { // if the user enters the wrong information it will display the following to the screen
            cout << "That is an invalid command " << rank << ". Please try again. " << endl;
        }
    }

    return 0;
}