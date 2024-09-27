#include <windows.h> // Header from API Windows, it provide us with the Sleep() function
#include <iostream>
/*pqxx is a library recovered from this Github repository: https://github.com/jtv/libpqxx.git */
#include <pqxx/pqxx>  // This library allows us to establish a conection with postgreSQL
#include <variant>
/* #include <string> */
#include <cmath>

/* Including functions definitions (Headerfiles) */
#include "db.h"

using namespace std;

bool userExists(string& username, pqxx::connection& conn){
    pqxx::work w(conn); // Starting transaction 

    pqxx::result res{w.exec("SELECT user_id FROM users WHERE username = " + w.quote(username))}; // Storing the result of query
    /* .quote is used to many diferents things like: escape automatically special characters to use it in the SQL query, this prevents 
    SQL injections.
    To values of type string, this function surrounds the value with sibgle quotes and escapes internal simple quotes, so that also
    this function takes care of the best conversion of other data types so the value is included in the query.
    */

    w.commit();

    return !res.empty();

}

void registerNewUser(string& username, string& password, pqxx::connection& conn){
    try {
        pqxx::work w(conn);
        const string query = "INSERT INTO users(username, password) VALUES(" + w.quote(username) + ", " + w.quote(password) + ")";

        w.exec(query); // Execute the query 

        w.commit(); // Confirm the transaction to aply changes

        cout<<"User created successfully"<<endl;
    } 
    catch (const pqxx::sql_error &e) { // sql_error provide us information about the SQL errors
        cerr<<"SQL Error: "<<e.what()<<endl; // .what give us information about the error
        cerr<<"Query was: "<<e.query()<<endl; // .query tell us which was the query that produced the error
    }
    catch (exception const &e){ // Catching any exception 
        cerr<<"Error: "<<e.what()<<endl;
    }
}

bool matchPassword(pqxx::connection& conn, string& password, string& username){
    pqxx::work w(conn);
    const string query = "SELECT user_id FROM users WHERE password = " + w.quote(password) + "AND username = " + w.quote(username);
    pqxx::result res{w.exec(query)};

    w.commit();

    return !res.empty(); // if the result is empety return false (password did not match), else return true (password match)

}

string getUserId(pqxx::connection& conn, string& username){
    pqxx::work w(conn);
    const string query = "SELECT user_id FROM users WHERE username = " + w.quote(username);

    pqxx::result res{w.exec(query)}; // "res" stores the tuples and column "user_id" of the table "users" 

    w.commit(); //End transaction to not cause problems with the function "insertAmount"

    return (res[0]["user_id"]).as<string>(); // returning as string user_id
}

void insertAmount(float& amount, pqxx::connection& conn, string& username){
    try {
        string user_id = getUserId(conn, username); // Calling getUserId to be able to make the query

        pqxx::work w(conn);

        string query = "INSERT INTO money(money_id, amount) VALUES(" + w.quote(user_id) + ", " + w.quote(amount) + ")";
    
        w.exec(query);
        w.commit();

        
    }
    catch (pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what()<<endl;
        cerr<<"Query was: "<<e.query();
    }
    catch (exception const &e){
        cerr<<"Error: "<<e.what();
    }
}

float getMoney(pqxx::connection& conn, string& user_id){
    pqxx::work w(conn);
    string query = "SELECT amount FROM money WHERE money_id = " + w.quote(user_id);

    pqxx::result res{w.exec(query)};

    w.commit();

    if (res.empty()){
        return 0; // If user doesn't have money
    }else{
        return res[0]["amount"].as<float>(); // returning result from column amount
    }

}

void updateAmount(pqxx::connection& conn, string& user_id, float& amount){
    try {
        pqxx::work w(conn);
        string query = "UPDATE money SET amount = " + w.quote(amount) + " WHERE money_id = " + w.quote(user_id);

        w.exec(query); // Updating user's balance
        w.commit();
    }
    catch(pqxx::sql_error &e){
        cerr<<"SQL Error: "<<e.what()<<endl;
        cerr<<"Query: "<<e.query();
    }
    catch(exception const &e){
        cerr<<"Error: "<<e.what();
    }
}

void saveExpenses(pqxx::connection& conn, vector<string>& expenses, vector<float>& costs, vector<string>& dates, string& user_id){
    try {
        pqxx::work w(conn);
        int size = expenses.size();
        string query;
        float roundedCost;
        stringstream ss;
        
        // For loop to register each expense in the database
        for(int i = 0; i < size; i++){
            // Rounding each cost to two decimals places
            roundedCost = roundf((costs[i]*100)/100);
            ss<<fixed<<setprecision(2)<<roundedCost;
            
            query = "INSERT INTO expenses(name, date, cost, user_id) VALUES(" + w.quote(expenses[i]) + ", " + w.quote(dates[i]) + ", " + w.quote(costs[i]) + ", " + w.quote(user_id) +")";
            w.exec(query);
        }
        w.commit();
        cout<<"Succesfull expenses saving";
    } 
    catch (pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what();
        cerr<<"Query: "<<e.query();
    }
    catch (exception const &e) {
        cerr<<"Error: "<<e.what();
    }
}

pqxx::result getExpenses(pqxx::connection &conn, string& username){

    try{
        string user_id = getUserId(conn, username);
        pqxx::work w(conn);

        string query = "SELECT name, date, cost FROM expenses WHERE user_id = " + w.quote(user_id) + "ORDER BY expense_id";

         pqxx::result res{w.exec(query)};
        w.commit(); 

        return res;
    }

    catch (pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what();
        cerr<<"Query: "<<e.query();
    }
    catch (exception const &e) {
        cerr<<"Error: "<<e.what();
    }

}

pqxx::result getNameExpenses(pqxx::connection &conn, string& user_id){

    try{
        pqxx::work w(conn);
        string query = "SELECT name FROM expenses WHERE user_id = " + w.quote(user_id);

        pqxx::result res{w.exec(query)};

        w.commit();

        return res;
    }
    catch(pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what();
        cerr<<"Query: "<<e.query();
    }
    catch(exception const &e){
        cerr<<"Error: "<<e.what();
    }
}

void updateExpense(pqxx::connection& conn, string& nameExp, string columnName, string& user_id, variant<string, float> data){
    try{
        pqxx::work w(conn);
        string query = "UPDATE expenses SET " + columnName + " = " + w.quote(data) + " WHERE name = " + w.quote(nameExp) + " AND user_id = " + w.quote(user_id);

        w.exec(query);
        w.commit();

        cout<<"Update made succesfully"<<endl;
        Sleep(400);
        system("cls");
    }
    catch(pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what();
        cerr<<"Query: "<<e.query();
    }
    catch(exception const &e){
        cerr<<"Error: "<<e.what();
    }
}

float getAmount(pqxx::connection& conn, string& user_id, string& nameExp){
    try{
        pqxx::work w(conn);
        string query = "SELECT cost FROM expenses WHERE user_id = " + w.quote(user_id) + " AND name = " + w.quote(nameExp);

        pqxx::result res{w.exec(query)};

        return res[0]["cost"].as<float>();
    }
    catch(pqxx::sql_error &e){
        cerr<<"SQL error: "<<e.what();
        cerr<<"Query: "<<e.query();
    }
    catch(exception const &e){
        cerr<<"Error: "<<e.what();
    }
}


