// QuickSSH by Tony S
// A easy OpenSSH server selector

// Defines File Name and also Path
#define FILENAME "quickssh.list"
// Defines wheter the program logs what it's doing
#define LOG false
// Defines wheter the screen clearing method is System Based or Escape Codes based
#define USE_ANSI_ESCAPE_CODES false
// Defines if ANSI Escape Codes are allowed for coloring
#define USE_AEC_COLORING true

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const char* reset =
#if USE_AEC_COLORING
"\x1B[0m";
#else
"";
#endif

const char* error =
#if USE_AEC_COLORING
"\x1B[38;5;196m\x1B[4m";
#else
"";
#endif

const char* warning =
#if USE_AEC_COLORING
"\x1B[38;5;220m";
#else
"";
#endif

const char* ok =
#if USE_AEC_COLORING
"\x1B[38;5;46m";
#else
"";
#endif

const char* title =
#if USE_AEC_COLORING
"\x1B[38;5;196m\x1b[48;5;233m\x1B[1m";
#else
"";
#endif


size_t count_char(fstream &file, char char_to_count) {
    file.clear();
    file.seekg(0);
    return count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), char_to_count) + 1;
}

size_t count_char(string input, char char_to_count) {
    return count(input.begin(), input.end(), char_to_count);
}

void exists_or_make(const char* file) {
    ifstream checkfile(file);
    if (checkfile.fail()) {
        ofstream dummyfile(file);
        dummyfile.close();
#if LOG
        cout << warning << "List File not found. Created a new List File\n" << reset;
#endif // LOG
    }
    checkfile.close();
}

void new_entry(fstream &file, string alias, string ip, string username) {
    string line;
    file.seekg(0, ios::end);
    file << alias << '\n' << ip << '\n' << username << '\n';
    file.flush();
    file.seekg(0);
}

string get_line(fstream &file, size_t line_number, size_t line_count) {
    file.clear();
    file.seekg(0);
    string line;
    for (size_t i = 0; i < line_number + 1 && i < line_count; i++)
        getline(file, line);
    return line;
}

void print_list(fstream &file, size_t lines) {
    for (size_t i = lines; i > 1; i -= 3) {
        cout << "Server Name: " << get_line(file, i - 3, lines) << "\nUsername: " << get_line(file, i - 1, lines) << "\nIp: " << get_line(file, i - 2, lines) << "\n\n";
    }
}

void print_options(fstream& file, size_t lines) {
    for (size_t i = lines; i > 1; i -= 3) {
        cout << ' ' << i / 3 - 1 << " - " << get_line(file, i - 3, lines) << '\n';
    }
}

void clear_screen() {
#if USE_ANSI_ESCAPE_CODES
    cout << "\x1B[1;1H\x1B[2J";
#else //USE_ANSI_ESCAPE_CODES

#ifdef _WIN32
    system("cls");
#elif unix 
    system("clear");
#endif

#endif //USE_ANSI_ESCAPE_CODES
}

void add_entry(fstream &file) {
    clear_screen();
    string sname = "", ip = "", uname = "";
    cout << "Adding a Server to the List File.\nType in Server Name: ";
    cin >> sname;
    cout << "Type in IP Address: ";
    cin >> ip;
    cout << "Type in Username: ";
    cin >> uname;
    cout << "\n\nServer Name: " << sname << "\nIP Address: " << ip << "\nUsername: " << uname << "\n\nCorrect?    [Y/N]";

    new_entry(file, sname, ip, uname);
    cout << ok << "\nSaved.\n" << reset;
    clear_screen();
    return;
}


int main() {

#if LOG
    cout << "Checking if OpenSSH is installed...\n";
#endif //LOG
    if (system("ssh") == 1) {
        clear_screen();
        cout << error << "OpenSSH not installed, install it to use QuickSSH\nQuitting...\n" << reset;
        return 0;
    }

    while (true) {
#if LOG
        cout << "Loading List File " << FILENAME << "...\nChecking if file ends with a New Line...\n";
#endif // LOG
        exists_or_make(FILENAME);
        fstream listfile(FILENAME, ios::in | ios::out | ios::app);
        size_t line_count = count_char(listfile, '\n') - 1;

        listfile.seekg(-1, ios_base::end);
        char expected_new_line;
        listfile.get(expected_new_line);
        if (expected_new_line != '\n') {
#if LOG
            cout << error << "List File does not end with New Line, adding \\n character.\n" << reset;
#endif //LOG
            listfile << '\n';
            listfile.flush();
        }
        else {
#if LOG
            cout << ok << "List File ends with New Line.\n" << reset;
#endif // LOG
        }
        listfile.seekg(0);

#if LOG
        cout << "Parsing List File...\n" << ok << "Found " << count_char(listfile, '\n') << " lines.\nFound " << count_char(listfile, '\n') / 3 << " servers.\n" << reset <<"Checking for correct count of entries...\n";
#endif // LOG

        if (line_count % 3 != 0) {
#if LOG
            cout << warning << "Count error, subtracting to acceptable value." << reset;
#endif // LOG
            line_count++;

            listfile.seekg(ios::end);
            string newlist = "";
            listfile.seekg(0, std::ios::end);
            newlist.reserve(listfile.tellg());
            listfile.seekg(0, std::ios::beg);
            newlist.assign((std::istreambuf_iterator<char>(listfile)), std::istreambuf_iterator<char>());
            listfile.close();
            newlist.append("unknown\n");
            fstream listfile(FILENAME, ios::in | ios::out | ios::trunc);
            listfile << newlist;
            listfile.seekg(0);

            continue;
        }
        else {
#if LOG
            cout << ok << "Correct number of entries." << reset;
#endif // LOG
        }

        size_t selector = 0;
        clear_screen();
        cout << title << "QuickSSH Server Selector\n" << reset <<"\nSelect the SSH Server:\n\n";

        print_options(listfile, line_count);

        cout << "\n-1 - Quit\n-2 - New SSH Server\n-3 - Delete SSH Server\n-4 - Print Server List File\n-5 - Credits\n\n\nChoose an option: ";

        if (!(cin >> selector))
        {
            cin.clear();
            cin.ignore();
            cout << error << "\nWrong Input. Type a valid option: " << reset;
        }

        if (selector == -1) {
            clear_screen();
            return 0;
        }

        if (selector == -2) {
            add_entry(listfile);
        }

        if (selector == -3) {
            clear_screen();
            cout << "Choose which server to delete or type in -1 to go back\n\n";
            print_options(listfile, line_count);
            cout << "\n\nServer to " << error << "Delete" << reset << ": ";

            if (!(cin >> selector)) {
                cin.clear();
                cin.ignore();
                cout << error <<"\nWrong Input. Type a valid option: " << reset;
            }

            if (selector == -1) {
                clear_screen();
                continue;
            }

            if (selector >= 0 && selector < line_count / 3) {
                if (line_count % 3 == 0) {
                    string newlist;
                    listfile.seekg(0, std::ios::end);
                    newlist.reserve(listfile.tellg());
                    listfile.seekg(0, std::ios::beg);

                    newlist.assign((std::istreambuf_iterator<char>(listfile)), std::istreambuf_iterator<char>());

                    string lookup_line = get_line(listfile, selector * 3, line_count) + '\n'
                        + get_line(listfile, selector * 3 + 1, line_count) + '\n'
                        + get_line(listfile, selector * 3 + 2, line_count) + '\n';
                    
                    newlist.replace(newlist.find(lookup_line), lookup_line.length(), "");
                    listfile.close();
                    fstream listfile(FILENAME, ios::in | ios::out | ios::trunc);
                    listfile << newlist;
                    cout << ok << "\nDeleted.\n" << reset;
                    clear_screen();
                    continue;
                }
                else
                    cout << error <<"Not enough lines to delete\n" << reset;
            }
            else {
                clear_screen();
                continue;
            }
        }


        if (selector == -4) {
            clear_screen();
            cout << "Servers saved in the List File:\n\n";
            print_list(listfile, line_count);
            cout << "\n\n\nPress Enter to go back.";
            getchar();
            getchar();
            clear_screen();
            continue;
        }

        if (selector == -5) {
            clear_screen();
            cout << "Credits:\n\n" << ok << "Made for fun by" << title << "\nTony S" << reset << "\n\n\nPress Enter to go back.";
            getchar();
            getchar();
            clear_screen();
            continue;
        }

        if (selector >= 0 && selector < line_count / 3) {
            clear_screen();
            cout << "Connecting to " << ok << get_line(listfile, selector * 3, line_count) << reset << " at " << error << get_line(listfile, selector * 3 + 1, line_count) << reset << " with username " << warning << get_line(listfile, selector * 3 + 2, line_count) << reset << "...\n";
            string ssh_line("ssh -l");
            string command = ssh_line + get_line(listfile, selector * 3 + 2, line_count) + ' ' + get_line(listfile, selector * 3 + 1, line_count);
            system(command.c_str());
            cout << ok << "SSH Connection ended. Press Enter to quit.\n" << reset;
            getchar();
            getchar();
            clear_screen();
            break;
        }
    }
}
