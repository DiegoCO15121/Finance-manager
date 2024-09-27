#include <windows.h> // Header from API Windows, it provide us with the Sleep() function
#include <iostream>
/*pqxx is a library recovered from this Github repository: https://github.com/jtv/libpqxx.git */
#include <pqxx/pqxx>  // This library allows us to establish a conection with postgreSQL 
#include <regex> // This library is used to use regular expresions
#include <iomanip> // With this library we can round off the decimal numbers of the amount of money
#include <conio.h> // This library proviede us _getch()
//#include <vector>

/* Including functions definitions (Headerfiles) */
#include "utils.h"
#include "interfaces.h"
#include "db.h"

using namespace std;

void getUsername(string& username){
    bool flag  = true;

    while(flag){
        getline(cin, username, '\n');
        
        if(username.empty()){
            cout<<"Please enter a username: ";
        }else{ //If the string is not empty, the function exits the loop
            flag = false;
        }
    }
}

void getPassword(string& password){
    char aux;

    do {
        while((aux = _getch()) != '\r'){ // _getch() captures a single input character without displaying it & the loop ends when the user press enter key
            if(aux == '\b'){ // If the user press backspace key 
                if(!password.empty()){ //& the password is not empty
                   password.pop_back(); // The last character is removed
                   cout<<"\b \b";  // Clean "*" of the output terminal
               }    
            }else{
                password.push_back(aux); // Addding character to password
                cout<<"*";
            }
        }

        if(password.empty()){ 
            cin.clear();
            cout<<endl<<"Please enter password: ";
            }
    }while(password.empty());

    cout<<endl;

}

float validateNumber(){
    float x;
    char aux;
    string input;

    while(true){
        cout<<"Enter an amount: ";
        while((aux = getch()) != '\r'){ //Reading character by character
            if(aux == '\b'){ 
             if(!input.empty()){
                    input.pop_back();
                    cout<<"\b \b";
                }
            }else{
                cout<<aux; //printing each character
                input.push_back(aux);
            }
        }
        cout<<endl;

        if(!input.empty()){ // Check if the input is not empty
            stringstream ss(input); //stringstream allows us to convert various types of data in string and viceversa

            if (ss >> x){ // If convertion from string to float does not fail, the loop ends
                break; 
            }
        }else{
            cin.clear(); // If the input is empty, we clear cin
        }

        input.clear(); // If the convertion did not be succesful, the input clears and the loop ask for the amount again
    }

    return x; // If alll are fine, our function will return a float
}

bool validateAnswer(string message){
    char ans;
    regex yesNo("[YyNn]"); // This regex is used to avoid a incorrect entry
    regex yes("[Yy]"); // This one is used to return true or false
        
    do{
        cout<<message;
        cin>>ans;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Cleaning up the buffer

        if(!regex_match(string(1, ans), yesNo)) cout<<"Type Yy/Nn"<<endl;
    }while(!regex_match(string(1, ans), yesNo));

    return regex_match(string(1, ans), yes); // If "ans" match with yes, return true, else return false
}

void saveMoney(pqxx::connection& conn, string& username, string& user_id){
    system("cls");

    float amount;
    float current_money = getMoney(conn, user_id); // Getting user's current money

    do {
        amount = validateNumber(); // Validating the amount

        if(amount > 99999999.99) cout<<"that's a lot of money :0";
    }while(amount > 99999999.99);



    bool flag = validateAnswer("Are you sure to enter this amount?(y/n): ");
    
    
    if(flag){ // If user is sure, the function in charge to update the amount of money in the db will be called
        amount += current_money;
        updateAmount(conn, user_id, amount);
        cout<<"Money saved successfully"<<endl;
    }

    Sleep(400);
    return; // returning to menuInterface
}

void checkMoney(pqxx::connection& conn, string& username, string& user_id){
    system("cls");

    float money = getMoney(conn, user_id); // Gettin money

    cout<<"Available money: $"<<money<<endl;

    system("pause");
    return;
}

string getDate(){
    regex dateFormat("^\\d{4}-\\d{2}-\\d{2}$"); // This regex set the format that the database accept
    int day, month, year;
    char aux, aux2; // This variables is used to store the "-"
    string date;
    bool flag = false, result_regex;

    /* cin.ignore(numeric_limits<streamsize>::max(), '\n'); */
    cin.clear();

    do {
        do {
            cout<<"Enter the date when you did this expense (yyyy-mm-dd): ";
            getline(cin, date);
            result_regex = regex_match(date, dateFormat); // This regex function return true if the date match with regex

            if(!result_regex){ 
                cout<<"Please enter this format -> (yyyy-mm-dd)"<<endl;
                cin.clear();
            }
        }while(!result_regex);

        stringstream ss(date); // Using stringstream to separate date string

        ss>>year>>aux>>month>>aux2>>day;

        if(month < 1 || month > 12 || year < 1 || day < 1){ // Checking if the month, day or year goes out of range
            flag = false;
            cout<<"Year or month does not exists"<<endl;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }else{
            
            if (((year % 100 != 0 && year % 4 == 0) || (year % 400 == 0)) && (month == 2)){ // Checking if the year is leap and if month is February
                
                if(day <= 29){ //Febrary has 29 days in leap years
                     flag = true;
                }else{
                    cout<<"Invalid day"<<endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }   
                 
            }else{
                int daysOfYear[12] = {31,28,31,30,31,30,31,31,30,31,31,31};
                if (day <= daysOfYear[month-1]){ // Checking if the days match with the month
                    flag = true;
                }else{
                    cout<<"Invalid day"<<endl;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }

    }while(!flag);

    return date;
}

vector<string> queryToString(pqxx::result& res){ 
    vector<string>listExp; 

    for(auto row: res){ // auto allows the compiler to automatically deduce the type of a variable from its initializer.
        listExp.push_back(row[0].as<string>()); // Conerting each name to string
    }

    return listExp;
}

string renameRepeatExp(string expenseName, vector<string> &aux_exp){
    int cont=1; // it is gonna be our nemeric indicator to rename the expense
    string newName = expenseName;

    /* Within the conditional, it goes through the list of expenses until to find the expense name entered by user */
    if(find(aux_exp.begin(), aux_exp.end(), expenseName) != aux_exp.end()){ // If the name expense was found
        do{
            stringstream ss; 
            ss << expenseName << "(" << cont << ")"; 
            newName = ss.str();
            cont ++;
            /* Reaname will happen if the actual rename also is in the database */
        }while(find(aux_exp.begin(), aux_exp.end(), newName) != aux_exp.end());
    }
    
    return newName;
}

void requestExpenses(pqxx::connection& conn, string& username, string& user_id){
    // This tree first vectors is gonna save the data of each expense to register in the database
    vector<string> expenses; 
    vector<float> costs;
    vector<string> dates;

    vector<string> aux_exp;
    bool flag = true; // flag to control while loop
    string expense;
    string date;
    float cost, total=0.0f;
    float current_money = getMoney(conn, user_id);
    pqxx::result res = getNameExpenses(conn, user_id);

    cin.ignore(numeric_limits<streamsize>::max(),'\n');

    do {
        do {
            // Getting name of the expense
            cout<<"Enter name or description of the expense: "; 
            getline(cin, expense);
             

            if(expense.size() > 45){
                cout<<"Try with the name or description less than 45 characters"<<endl;
                expense.clear();
                cin.clear();
            }else if(expense.empty()) cin.clear();

        }while(expense.size() > 45 || expense.empty());

        // Getting it cost
        cost = validateNumber();

        // Getting it date
        date = getDate();

        // Saving the data of each expense
        expenses.push_back(expense);
        costs.push_back(cost);
        dates.push_back(date);

        flag = validateAnswer("Do you want to add another expense?(y/n): ");
        cout<<endl;

    }while(flag);

    for(int i = 0;i < static_cast<int>(costs.size()); ++i){
        total += costs[i]; // Getting the total amount of money spent
    }
    
    if(total <= current_money){ // The money spent must be less than the current money
        current_money -= total; // Updating value current amount

        aux_exp = queryToString(res); // Converting list of expenses to string

        // Renaming each expense before adding it to the database
        for(size_t i=0; i < expenses.size(); i++){
            expenses[i] = renameRepeatExp(expenses[i], aux_exp);
            aux_exp = expenses;
        }
    
        updateAmount(conn, user_id, current_money); // Updating current amount in the database
        saveExpenses(conn, expenses, costs, dates, user_id); // Saving expenses
        Sleep(500);

    }else{ // If user doesn't have enough money
        cout<<"You do not have enough money"<<endl;
        Sleep(500);
        return;
    }

    return;
}