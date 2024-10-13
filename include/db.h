#ifndef DB_H
#define DB_H

#include <iostream>
#include <variant>
#include <pqxx/pqxx>

using namespace std;

/* To avoid insert usernames with the same name this function makes a query to check it out */
bool userExists(string& username, pqxx::connection& conn);

/* This function makes a query to register the username in the db */
void registerNewUser(string& username, string& password, pqxx::connection& conn);

/* When the user wants to login this function will check out if the password is correct */
bool matchPassword(pqxx::connection& conn, string& password, string& username);

/* To insert amount, the user_id will be used to save the amount to the corresponding user */
string getUserId(pqxx::connection& conn, string& username);

/* This funcion is in charge to register the amount to the corresponding user */
void insertAmount(float& amount, pqxx::connection& conn, string& username);

/* Function that gets user's current money */
float getMoney(pqxx::connection& conn, string& user_id);

/* When the cost of an expense is modificated, the user's current money must be updated */
void updateAmount(pqxx::connection& conn, string& user_id, float& amount);

/* After ask for expenses, this function is gonna register all the expenses that user entered */
void saveExpenses(pqxx::connection& conn, vector<string>& expenses, vector<float>& costs, vector<string>& dates, string& user_id);

/* This functions returns all expenses and their information that user has registered */
pqxx::result getExpenses(pqxx::connection &conn, string& username);

/* This function return all expenses names tahta user has registered */
pqxx::result getNameExpenses(pqxx::connection &conn, string& user_id);

/* According to what the user has chosen, this function will update an expense data */
void updateExpense(pqxx::connection& conn, string& nameExp, string columnName, string& user_id, variant<string, float> data);

/* This funcion get the expense amount to be able to update it */
float getAmount(pqxx::connection& conn, string& user_id, string& nameExp);

/* This functions delete the expense user has selected */
void deleteExpense(pqxx::connection& conn, string& nameExp, string& userId);

#endif
