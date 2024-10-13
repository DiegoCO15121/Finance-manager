CREATE DATABASE finances;

\c finances

CREATE TABLE users(
    user_id SERIAL PRIMARY KEY,
    username VARCHAR(45) NOT NULL,
    password VARCHAR(15) NOT NULL
);

CREATE TABLE money(
    money_id INT PRIMARY KEY,
    amount DECIMAL(10,2) NOT NULL,
    /* user_id INT NOT NULL, */
    FOREIGN KEY(money_id) REFERENCES users(user_id)
);

CREATE TABLE expenses(
    expense_id SERIAL PRIMARY KEY,
    name VARCHAR(60) NOT NULL,
    date DATE,
    cost DECIMAL(10,2) NOT NULL,
    user_id INT NOT NULL,
    FOREIGN KEY(user_id) REFERENCES users(user_id)
);