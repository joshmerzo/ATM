#include <iostream>
#include <string>
#include <iomanip>      // Input/output manipulators, used for formatting the console output.
#include <sstream>      // String stream, used for converting between strings and other data types.
#include <conio.h>      // Console input/output functions, used for getting keyboard input without waiting for Enter.
#include <fstream>
#include <random>       // Random number generation, used for generating random PINs.
#include <set>          // Set container, used to manage unique values in the program.
#include <windows.h>    // Windows-specific functions, used for console manipulation (e.g., clearing the screen).
#include <chrono>       // Time-related functionality, used for generating seeds for random number generation.

using namespace std;

class ATMList{
private:
    struct ATM {
        string accountNumber;
        string username;
        string birthday;
        string contactNumber;
        int PIN;
        int balance;
};
    //Global Variables
    const int MAX_USERS = 1001;
    int users = 0;
    ATM userList[100];
    const int XOR_KEY = 0x5A5A5A5A;

    //Function to encrypt the PIN code
    int encryptPin(int pin) {
            return pin ^ XOR_KEY;
        }
    //Function to decrypt the PIN code
    int decryptPin(int encryptedPin) {
            return encryptedPin ^ XOR_KEY;
        }

    //Function that generates a random PIN-Code
    int generateRandomPIN() {
    unsigned seed = static_cast<unsigned>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::mt19937 gen(seed);  // Seed with the current time
    std::uniform_int_distribution<int> distribution(1000, 9999); // Range of PINs (4-digit)

    return distribution(gen);
}

public:
    //Function that generates a unique account number
    string generateAccountNumber(const string& lastAccountNumber) {
        stringstream ss;
        int lastUserCount = stoi(lastAccountNumber);
        ss << setw(5) << setfill('0') << lastUserCount + 1;
        return ss.str();
    }

    //Function that gets the last account number
    string getLastAccountNumber() {
        ifstream inputFile("pin.code.txt");
        string lastAccountNumber;

        if (inputFile.is_open()) {
            while (!inputFile.eof()) {
                ATM user;
                inputFile >> user.accountNumber >> user.username >> user.birthday >> user.contactNumber >> user.PIN >> user.balance;
                if (!inputFile.eof()) {
                    lastAccountNumber = user.accountNumber;  // Update the last account number
                }
            }
            inputFile.close();
        }
        return lastAccountNumber.empty() ? "0" : lastAccountNumber;
    }

    //Function to validate the user's contact number
    bool isValidContactNumber(const string &contactNumber) {
        // Check if the contact number is in either of the two formats
        // 1. (+63) followed by 10 digits
        // 2. 11 digits starting with 09 or +639
        if (contactNumber.length() == 15 && contactNumber.substr(0, 5) == "(+63)") {
            for (int i = 5; i < 15; i++) {
                if (!isdigit(contactNumber[i]))
                    return false;
            }
            return true;
        } else if (contactNumber.length() == 11 && (contactNumber.substr(0, 2) == "09" || contactNumber.substr(0, 4) == "+639")) {
            for (int i = 2; i < 11; i++) {
                if (!isdigit(contactNumber[i]))
                    return false;
            }
            return true;
        }

        return false;
    }

    //Function to enroll a new account
    void enrollUser(ATMList& atm) {
        int month, day, year;
        ATM newUser;

        string lastAccountNumber = atm.getLastAccountNumber();
        int lastAccountNumberInt = stoi(lastAccountNumber);
        newUser.accountNumber = atm.generateAccountNumber(to_string(lastAccountNumberInt));
        system("cls");
        cout << "\x1b[36m\n\t~CREATE NEW ACCOUNT~\x1b[0m" << endl;
        cout << "\n\tEnter Account Name: ";
        getline(cin, newUser.username);

        //Validates user's birthday
        do {
            cout << "\tEnter Birthday (MM DD YY): ";
            cin >> month >> day >> year;
            if (month < 1 || month > 12 || day < 1 || day > 31 || year > 2005) {
                cout << "\t\x1b[31mInvalid birthday. Please enter a valid date (MM DD YY).\n \x1b[0m";
            }
        } while (month < 1 || month > 12 || day < 1 || day > 31 || year > 2005);

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Add this line to clear the newline character.

        newUser.birthday = to_string(month) + "-" + to_string(day) + "-" + to_string(year);

        cout << "\tEnter Contact Number: ";
        getline(cin, newUser.contactNumber);

        while (!isValidContactNumber(newUser.contactNumber)) {
            cout << "\x1b[31m\tInvalid contact number format\x1b[0m. Please enter a valid Philippine phone number format: ";
            getline(cin, newUser.contactNumber);
        }

        cout << "\tEnter Initial Deposit (Minimum of 5000Php): ";
        cin >> newUser.balance;

        //Initial deposit cannot be less than 5000
        while (newUser.balance < 5000) {
            cout << "\x1b[31m\tInvalid deposit amount\x1b[0m. Please enter an amount of 5000Php or more: ";
            cin >> newUser.balance;
        }

        newUser.PIN = generateRandomPIN();

        if (users < MAX_USERS) {
            userList[users] = newUser;
            users++;
            cout << "\n\tUser successfully enrolled!\n";
            cout << "\n\tYour account number is \x1b[32m" << newUser.accountNumber << "\x1b[0m" << endl;
            cout << "\n\tYour PIN code is: \x1b[32m" << newUser.PIN << "\x1b[0m\n" << endl;
        } else {
            cout << "\n\tUser limit reached. Enrollment failed.\n";
        }
    }

    //Function to display user information
    void displayUserInfo(string accountNum) {

        // Search for the account in the user list
        int index = -1;
        for (int i = 0; i < users; i++) {
            if (userList[i].accountNumber == accountNum) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            cout << "\x1b[36m\n\tAccount Information\x1b[0m" << endl;
            cout << "\n\tAccount Number: " << userList[index].accountNumber << endl;
            cout << "\tAccount Name: " << userList[index].username << endl;
            cout << "\tBirthday: " << userList[index].birthday << endl;
            cout << "\tContact Number: " << userList[index].contactNumber << endl;
            cout << "\tBalance: " << userList[index].balance << "Php" << endl;
            cout << "\n\t";
            system ("pause");
        } else {
            cout << "\x1b[31m\n\tAccount not found. Please check your account number.\x1b[0m" << endl;
            cout << "\n\t";
            system ("pause");
        }
    }

    // Function to withdraw money
    void withdraw(string accountNum, int pin) {
    int index = -1;
    for (int i = 0; i < users; i++) {
        if (userList[i].accountNumber == accountNum && userList[i].PIN == pin) {
            index = i;
            break;
        }
    }
    if (index != -1) {
        system("cls");
        cout << "\x1b[36m\n\tWITHDRAW MONEY\x1b[0m" << endl;
        while (true) {
            cout << "\n\tYour current balance is \x1b[32m" << userList[index].balance << "Php \x1b[0m" << endl;
            int amount;
            cout << "\n\tEnter the amount to withdraw (or enter 0 to return to the main menu): ";
            cin >> amount;
            if (amount == 0) {
                // User chose to return to the main menu
                break;
            } else if (amount < 0) {
                cout << "\x1b[31m\n\tInvalid withdrawal amount. Please enter a valid amount.\x1b[0m" << endl;
            } else if (amount > userList[index].balance) {
                cout << "\x1b[31m\n\tInsufficient balance. Withdrawal failed.\x1b[0m" << endl;
            } else {
                userList[index].balance -= amount;
                cout << "\n\tWithdrawal successful. Your new balance is \x1b[32m" << userList[index].balance << "Php\x1b[0m" << endl;
                break; // Exit the loop when a successful withdrawal is made
            }
        }
    } else {
        cout << "\x1b[31m\n\tAccount not found or incorrect PIN. Please check your account number and PIN.\x1b[0m" << endl;
    }
    saveUserData(); // Save the updated user data
}


    // Function to deposit money
    void deposit(string accountNum, int pin) {
        // Search for the account in the user list
        int index = -1;
        for (int i = 0; i < users; i++) {
            if (userList[i].accountNumber == accountNum && userList[i].PIN == pin) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            system ("cls");

            int amount;

            cout << "\x1b[36m\n\tDEPOSIT MONEY\x1b[0m" << endl;
            cout << "\n\tYour current balance is \x1b[32m" << userList[index].balance << "Php\x1b[0m\n" << endl;
            cout << "\n\tEnter the amount to deposit: ";
            cin >> amount;

            if (amount > 0) {
                userList[index].balance += amount;
                cout << "\n\tDeposit successful. New balance: \x1b[32m" << userList[index].balance << "Php\n\x1b[0m" << endl;
            } else {
                cout << "\n\tInvalid amount or incorrect PIN. Deposit failed." << endl;
            }
        } else {
            cout << "\x1b[31m\n\tAccount not found or incorrect PIN. Please check your account number and PIN.\x1b[0m" << endl;
        }

        saveUserData(); // Save the updated user data
    }



    //Function for balance inquiry
    void balanceInquiry(string accountNum, int pin) {
        int index = -1;
        for (int i = 0; i < users; i++) {
            if (userList[i].accountNumber == accountNum && userList[i].PIN == pin) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            system ("cls");
            cout << "\x1b[36m\n\tBALANCE INQUIRY\x1b[0m" << endl;
            cout << "\n\tAccount Number: " << userList[index].accountNumber << endl;
            cout << "\tAccount Name: " << userList[index].username << endl;
            cout << "\tBalance: " << userList[index].balance << "Php" << endl;
            cout << "\n\t";
            system ("pause");
        } else {
            cout << "Account not found or incorrect PIN. Please check your account number and PIN." << endl;
            cout << "\n\t";
            system ("pause");
        }
    }


    // Function to perform fund transfer
    void fundTransfer(string senderAccountNum) {
    string receiverAccountNum;
    double amount;

    int senderIndex = -1;
    for (int i = 0; i < users; i++) {
        if (userList[i].accountNumber == senderAccountNum) {
            senderIndex = i;
            break;
        }
    }

    if (senderIndex == -1) {
        cout << "\x1b[31m\n\tSender account not found. Please check your account number.\x1b[0m" << endl;
        cout << "\n\t";
        system("pause");
        return;
    }

    system("cls");
    cout << "\x1b[36m\n\tFUND TRANSFER\x1b[0m" << endl;

    while (true) {
        cout << "\n\tEnter the recipient's account number (or enter 0 to return to the main menu): ";
        cin >> receiverAccountNum;

        if (receiverAccountNum == "0") {
            // User chose to return to the main menu
            break;
        }

        int receiverIndex = -1;
        for (int i = 0; i < users; i++) {
            if (userList[i].accountNumber == receiverAccountNum) {
                receiverIndex = i;
                break;
            }
        }

        if (receiverIndex == -1) {
            cout << "\x1b[31m\n\tRecipient account not found. Please check the account number.\x1b[0m" << endl;
        } else {
            cout << "\n\tYour current balance is \x1b[32m" << userList[senderIndex].balance << "Php\x1b[0m\n" << endl;

            while (true) {
                cout << "\n\tEnter the amount to transfer (or enter 0 to return to the main menu): ";
                cin >> amount;

                if (amount == 0) {
                    // User chose to return to the main menu
                    return;
                }

                if (amount > 0 && amount <= userList[senderIndex].balance) {
                    userList[senderIndex].balance -= amount;
                    userList[receiverIndex].balance += amount;
                    cout << "\n\tTransfer successful. " << amount << "Php transferred to account " << receiverAccountNum << "\n\n";

                    // Save the updated user data after the fund transfer
                    saveUserData();
                    return;
                } else {
                    cout << "\x1b[31m\n\tInvalid amount or insufficient balance for the transfer. Please enter a valid amount.\x1b[0m" << endl;
                        }
                }
            }
        }
    }
    // Function to change the PIN code
    void changePin(string accountNum) {
    int newPin;
    int pin;
    int index = -1;

    while (true) {
        cout << "\n\tEnter your current PIN code (or enter 0 to return to the main menu): ";
        cin >> pin;
        if (pin == 0) {
            // User chose to return to the main menu
            return;
        }

            for (int i = 0; i < users; i++) {
                if (userList[i].accountNumber == accountNum && userList[i].PIN == pin) {
                    index = i;
                    break;
                }
            }

            if (index == -1) {
                cout << "\x1b[31m\n\tAccount not found or incorrect PIN. Please check your account number and PIN.\x1b[0m" << endl;
                getch();
            } else {
                break; // Exit the loop if the correct PIN is provided
            }
        }

        while (true) {
            cout << "\n\tEnter your new PIN code: ";
            cin >> newPin;

            if (newPin >= 1000 && newPin <= 9999) {
                userList[index].PIN = newPin;
                cout << "\n\tPIN code successfully changed.\n" << endl;

                saveUserData(); // Save the updated user data after changing the PIN
                break; // Exit the loop once a valid PIN is provided
            } else {
                cout << "\x1b[31m\n\tInvalid PIN code. Please enter a 4-digit number.\x1b[0m" << endl;
            }
        }
    }


    //Function to display transaction menu
    void accountMenu(string accountNum) {
        int choice;
        int pin;

        cout << "\tEnter your PIN code: ";
        cin >> pin;

        // Search for the account in the user list
        int index = -1;
        for (int i = 0; i < users; i++) {
            if (userList[i].accountNumber == accountNum && userList[i].PIN == pin) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            while (true) {
                system ("cls");
                cout << "\x1b[36m\n\tLogin successful!\n\x1b[0m";
                cout << "\n\t1. Balance Inquiry" << endl;
                cout << "\t2. Withdraw" << endl;
                cout << "\t3. Deposit" << endl;
                cout << "\t4. Fund Transfer (for enrolled accounts only)" << endl;
                cout << "\t5. Change PIN Code" << endl;
                cout << "\t6. Display Account Information" << endl;
                cout << "\t7. Exit" << endl;
                cout << "\n\tEnter your choice: ";
                cin >> choice;

                switch (choice) {
                    case 1:
                        balanceInquiry(accountNum, pin);
                        break;
                    case 2:
                        withdraw(accountNum, pin);
                        break;
                    case 3:
                        deposit(accountNum, pin);
                        break;
                    case 4:
                        fundTransfer(accountNum);
                        break;
                    case 5:
                        changePin(accountNum);
                        break;
                    case 6:
                        displayUserInfo(accountNum);
                        break;
                    case 7:
                        cout << "\n\tExiting the program." << endl;
                        cout << "\n\t";
                        system ("pause");
                        exit(0); // Use exit() to exit the program
                        break;
                    default:
                        cout << "\n\tInvalid choice. Please try again." << endl;
                        cout << "\n\t";
                        system ("pause");
                        break;
                }
            }
        } else {
            cout << "\n\tAccount not found or incorrect PIN. Please check your account number and PIN." << endl;
            cout << "\n\t";
            system ("pause");
        }
    }



    //Function to save the user data
    void saveUserData() {
            return; // Exit the function if the USB drive is not present

        ofstream outputFile("pin.code.txt");
        if (outputFile.is_open()) {
            for (int i = 0; i < users; i++) {
                outputFile << userList[i].accountNumber << " "
                    << userList[i].username << " "
                    << userList[i].birthday << " "
                    << userList[i].contactNumber << " "
                    << encryptPin(userList[i].PIN) << " " // Encrypt the PIN before saving
                    << userList[i].balance << "\n";
            }
            outputFile.close();
            cout << "\tUser data saved successfully." << endl;
            getch();

        }
        else {
            cout << "\n\tError opening E:/pin.code.txt for writing." << endl;
            cout << "\n\tPress 1 to exit." << endl;
            int choice;
            cin >> choice;
            if (choice == 1) {
                cout << "\n\tExiting the ATM application." << endl;
                exit(0);
            }
        }
    }

    //Function to load the user data
    void loadUserData() {
        ifstream inputFile("pin.code.txt");
        if (inputFile.is_open()) {
            users = 0; // Initialize users count
            while (inputFile >> userList[users].accountNumber) {
                inputFile >> userList[users].username
                         >> userList[users].birthday
                         >> userList[users].contactNumber
                         >> userList[users].PIN; // PIN is read as is from the file
                // Decrypt the PIN after reading from the file
                userList[users].PIN = decryptPin(userList[users].PIN);
                inputFile >> userList[users].balance;
                users++; // Increment the user count for each user read
            }
            inputFile.close();
        } else {
            cout << "\n\tNo file found. Creating a new pin.code.txt file." << endl;
            inputFile.close(); // Close the file stream

            ofstream newFile("pin.code.txt");
            if (newFile.is_open()) {
                newFile.close();
            } else {
                cout << "\n\tError creating a new pin.code.txt file." << endl;
                cout << "\n\tPress 1 to exit." << endl;
                int choice;
                cin >> choice;
                if (choice == 1) {
                    cout << "\n\tExiting the ATM application." << endl;
                    exit(0);
                }
            }
        }
    }

};

int main() {
    ATMList atm;
    int choice;
    string accountNum;

    // Load user data from file
    atm.loadUserData();

    // Get the last account number
    string lastAccountNumber = atm.getLastAccountNumber();

    // Generate a new account number
    string newAccountNumber = atm.generateAccountNumber(lastAccountNumber);

    while (true) {
        system("cls");
        cout << "\x1b[36m\n\tATM MACHINE\x1b[0m" << endl;
        cout << "\n\t1. Enroll User" << endl;
        cout << "\t2. Login" << endl;
        cout << "\t3. Exit" << endl;
        cout << "\n\tEnter your choice: ";
        cin >> choice;

        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                atm.enrollUser(atm);
                atm.saveUserData();
                break;
            case 2:
                cout << "\n\tEnter your account number: ";
                cin >> accountNum;
                atm.accountMenu(accountNum);
                break;
            case 3:
                cout << "\n\tExiting the program." << endl;
                return 0;
            default:
                cout << "\n\tInvalid choice. Please try again." << endl;
                break;
        }
    }
}
