#ifndef UTILS_H
#define UTILS_H

#include <pqxx/pqxx>
#include <iostream>
#include <vector>

using namespace std;

/* This funtion is used to prevent the suer from entering an empty username */
void getUsername(string& username);

/* This function gets the password without displaying it on the screen */
void getPassword(string &password);

/* This function validates the amount provided. It avoids input of chars or strings */
float validateNumber();

/* This function validates the answer of user (yes/no) */
bool validateAnswer(string message);

/* This function saves the amount of money provided by the user */
void saveMoney(pqxx::connection& conn, string& username, string& user_id);

/* With this funtion the user will be able to see his/her money */
void checkMoney(pqxx::connection& conn, string& username, string& user_id);

/* To avoid enter an incorret format date in the database, this function is in charge to get the correct format */
string getDate();

/* Function to convert the query of name expenses to string */
vector<string>queryToString(pqxx::result& res);

/* In case that user enters a new expense or updates a name of an expense and sets a name that was registered in the database, this function is gonna rename it with a numeric indicator */
string renameRepeatExp(string expenseName, vector<string> &aux_exp);

/* This function facilitates to get expenses*/
void requestExpenses(pqxx::connection& conn, string& username, string& user_id);    

#endif