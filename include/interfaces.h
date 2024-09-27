#ifndef INTERFACES_H
#define INTERFACES_H

#include <iostream>
#include <pqxx/pqxx>

using namespace std;

// Interface that appears when starting the program 
void startInterface(pqxx::connection& conn);

// Interface to login
void loginInterface(pqxx::connection& conn);

//Interface to register
void registerInterface(pqxx::connection& conn);

//Interface to select options offered by the program. Appears after login
void menuInterface(string& username, string& password, pqxx::connection& conn);

/* This function shows a table with all information about user's expenses */
void showExpenses(pqxx::connection& conn, string& username);

/* Funtion in charge to get the name of expense to modify */
void editExpenses(pqxx::connection& conn, string& username, string& user_id);

/* This interface allows to user to choose what they want to modify */
void modifyExpenseInterface(pqxx::connection& conn, string& nameExp, string& user_id, vector<string> &list_exp_aux);

#endif