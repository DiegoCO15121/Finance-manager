#include <windows.h> // Header from API Windows, it provide us with the Sleep() function
#include <iostream>
/*pqxx is a library recovered from this Github repository: https://github.com/jtv/libpqxx.git */
#include <pqxx/pqxx>  // This library allows us to establish a conection with postgreSQL 
#include <regex> // This library is used to use regular expresions
#include <variant>

/* Including functions definitions (Headerfiles) */
#include "interfaces.h" 
#include "utils.h"
#include "db.h"

using namespace std;

void startInterface(pqxx::connection& conn){
    int ans;
    bool flag = true;

     do{
        system("cls");
        cin.clear();

        cout<<"\t***** Welcome to your finances manager *****"<<endl;
        cout<<"1.Login"<<endl;
        cout<<"2.Register"<<endl;
        cout<<"3.Exit"<<endl;
        cout<<"Choose an option: ";cin>>ans;

        switch(ans){
            case 1: loginInterface(conn);
                break; // Calling the login interface
            case 2: registerInterface(conn); 
                break; // Calling the register interface
            case 3: flag = false; 
                break; // Exit
            default: cout<<"That option is not into menu"<<endl;
                Sleep(400);  
                break; // Controlling user input
        }
    
    }while(flag);
}

void loginInterface(pqxx::connection& conn){
    system("cls");

    string username, password;
    bool flag = false;

    cout<<"\tLogin"<<endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    do {
        cout<<"Username: ";
        getUsername(username);

        if(!userExists(username, conn)){ // Checking if the user exist in the database
            cout<<"Username does not match"<<endl;
            Sleep(400);
            system("cls");
            cout<<"\tLogin"<<endl;
        }else{
            flag = true;
        }

    }while(!flag);

    /* This loop avoids entring an empty password and a wrong password */
    while(!matchPassword(conn, password, username)){
        password.clear();

        cout<<"Password: ";
        getPassword(password); // Calling a function to get password by the security way

        if(!matchPassword(conn, password, username)) cout<<"Wrong password"<<endl;
    };

    /* If the password and username are correct the interface to select options offered by the program will be called */
    menuInterface(username, password, conn);
}

void registerInterface(pqxx::connection& conn){
    system("cls");
    
    cout<<"\tRegister"<<endl;

    string username, password, confirm_password;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    do{
        cout<<"New username: ";
        getUsername(username);

        if(username.size() > 45){ 
            cout<<"Please enter a username with less than 45 characters"<<endl;
            cin.clear();
            Sleep(500);
            system("cls");
            cout<<"\tRegister"<<endl;
        }
    }while(username.size() > 45); // This loop prevents to enter a username longer than 45 characters

    do { // This loop avoids entering an empty password or password longer than 15 characters
        password.clear();

        cout<<"Your password: ";
        getPassword(password); // Calling a function to get password by the security way

        if(password.size() > 15){ 
            Sleep(350);
            cout<<"Please enter a password with less than 15 characters"<<endl;
            cin.clear();
        }
    } while(password.size() > 15);

    /* Same way to confirm password */
    while(confirm_password != password){
        confirm_password.clear();

        cout<<"Confirm your password: ";
        getPassword(confirm_password); 

        if((confirm_password != password)) cout<<"Password entered does not match"<<endl; // Comparing both strings
        cin.clear();
    };

    if(userExists(username, conn)){ // Checking if the user already exists
        cout<<"That user is already exists"<<endl;
        Sleep(500);
        return;
        
    } else {
        bool flag = validateAnswer("Are you sure about creating this new user?(y/n): "); 

        if(flag){ // If the entry is positive, the user's information will be registered in the db
            float amount = 0.0f;
            registerNewUser(username, password, conn);
            insertAmount(amount, conn, username);  
            Sleep(400);
            cin.clear();
            loginInterface(conn); // The user is sent to the login interface
            return;
        }else{ // Else, the user is sent to the start interface
            cin.clear();
            return;
        }
    }
}

void showExpenses(pqxx::connection& conn, string& username){
    system("cls");

    array<int,3> wCol = {45, 12, 10}; // This array help to control the spaces to generate the table
    pqxx::result res = getExpenses(conn, username);

    if(!res.empty()){
        int columns = res.columns(); // Getting number of columns
        cout<<"+---------------------------------------------+------------+----------+"<<endl;
        cout<<"|";
        for(int i = 0; i < columns; i++){
            /* setfill is used to fill the spaces not used.
            setw is in charge to control the spaces with wCol
            left is the aling
            */
            cout<<setfill(' ')<<setw(wCol[i])<<left<<res.column_name(i)<<"|";
        }
        cout<<endl;
        cout<<"+---------------------------------------------+------------+----------+"<<endl;

        for(auto row: res){ // auto allows the compiler to automatically deduce the type of a variable from its initializer.
            cout<<"|";
                
            for(int j = 0; j < columns; ++j){
                cout<<setfill(' ')<<setw(wCol[j])<<left<<row[j].c_str()<<"|";
            }
            cout<<endl;
        }
        cout<<"+---------------------------------------------+------------+----------+"<<endl;

    }

    system("pause");
} 

void editExpenses(pqxx::connection& conn, string& username, string& user_id){
    system("cls");
    bool flag = false;
    string expense;
    pqxx::result res = getNameExpenses(conn, user_id);
    // list_exp_ux strores the query in format string
    vector<string>list_exp_aux = queryToString(res);

    if(res.empty()){
        cout<<"You do not have any expense";
        Sleep(500);
        return ; 
    }

    cin.ignore(numeric_limits<streamsize>::max(),'\n');

    do{
        cout<<"Enter the name of the expense that you want to modify: ";
        getline(cin,expense);

        if(expense.empty()){
            cin.clear();
            cout<<"Enter the name"<<endl;
        }else{
            // Looking for expense that user entered
            for(auto row: res){
                if(row[0].c_str() == expense){
                    flag = true;
                    break;
                }
            }
            if(!flag) cout<<"Expense not founded"<<endl;
        }
    }while(!flag);

    modifyExpenseInterface(conn, expense, user_id, list_exp_aux);
}

void modifyExpenseInterface(pqxx::connection& conn, string& nameExp, string& user_id, vector<string> &list_exp_aux){
    int ans;
    string newData;
    float newDataFloat, balance, oldAmount, difference, newBalance;
    bool flag = true;

    do{
        cout<<"'"<<nameExp<<"'"<<endl;
        cout<<"\tWhat do you want to modify?"<<endl;
        cout <<"1.Name"<<endl;
        cout <<"2.Cost"<<endl;
        cout <<"3.Date"<<endl;
        cout<<"4.Delete expense"<<endl;
        cout<<"5.Exit"<<endl;
        cin>>ans;

        switch(ans){
            case 1: 
            cout<<"Enter a new name: ";
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            getline(cin, newData);

            // rename new name if it is into the database
            newData = renameRepeatExp(newData, list_exp_aux);

            updateExpense(conn, nameExp, "name", user_id, newData); 
            flag = false;
            break;

            case 2: 
            newDataFloat = validateNumber(); // Getting new amount

            oldAmount = getAmount(conn, user_id, nameExp); // Getting old amount

            updateExpense(conn, nameExp, "cost", user_id, newDataFloat);

            balance = getMoney(conn, user_id); // Getting current balance

            /* difference stores the value of the diference between old amount and new amount to update the money */
            difference = oldAmount - newDataFloat;
            newBalance = balance + difference; // Getting new amount of user's money

            updateAmount(conn, user_id, newBalance); // Updating user's balance

            flag = false;
            break;

            case 3: cin.ignore();
            cout<<"Enter a new date: ";
            newData = getDate();
            updateExpense(conn, nameExp, "date", user_id, newData);
            flag = false;
            break;

            case 4: 
            oldAmount = getAmount(conn,user_id, nameExp); // Getting amount of the expense
            balance = getMoney(conn, user_id); // Getting user's balance

            newBalance = balance + oldAmount;

            deleteExpense(conn, nameExp, user_id); // Deleting expense
            updateAmount(conn, user_id, newBalance); // Updating user's balance
            flag = false;
            break;

            case 5: flag = false; break;

            default: cout<<"Please choose an option"<<endl;
            cin.ignore();
            break;
        }
    }while(flag);

    return;
}



void menuInterface(string& username, string& password, pqxx::connection& conn){
    int ans;
    bool flag = true;
    string user_id = getUserId(conn, username);

    do{
        system("cls");

        cout<<"1.Save or add money"<<endl;
        cout<<"2.Save expences"<<endl;
        cout<<"3.Check available money "<<endl;
        cout<<"4.Check expenses"<<endl;
        cout<<"5.Edit expenses"<<endl;
        cout<<"6.Exit"<<endl;
        cout<<"Choose an option: ";cin>>ans;

        switch(ans){
            case 1: saveMoney(conn, username, user_id); break;
            case 2: requestExpenses(conn, username, user_id); break;
            case 3: checkMoney(conn, username, user_id); break;
            case 4: showExpenses(conn, username); break;
            case 5: editExpenses(conn, username, user_id); break;
            case 6: flag = false; break;
            default: cout<<"That option is not in the menu"<<endl; 
                Sleep(500);
                break;
        }

    }while(flag);
}