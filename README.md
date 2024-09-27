# Finance Manager - C++ with PostgreSQL
*This project is a finance manager developed in C++ that uses PostgreSQL as the database management system. It allows users to manage income and expenses efficiently.*

## Features
- Add income to update your balance.
- Record expenses with details such as date and cost.
- View your current balance.
- Modify any recorded expense in case of errors.

## System Requirements
- **PostgreSQL** installed.
- **Operating System:** Windows 10/11 *(The finance manager uses a Windows-specific API)*.
- **Development Environment:** MinGW32 and any code editor.

## Installation

1. Clone this repository:

    ```cmd
    git clone https://github.com/DiegoCO15121/Finance-manager.git
    ```

2. Open the "src" folder and edit the `main.cpp` file to configure your PostgreSQL connection:

    ```cpp
    // Replace "localhost", "port", "user", "password" & "dbname" with your PostgreSQL connection information.
    const string conn_info = "host=localhost port=YourDefaultPort user=yourUser password=*** dbname=finances";
    ```

3. Open a terminal in the folder where you cloned the repository and run the following commands:

    ```cmd
    # Create the database
    psql -U yourUser -d postgres -f sql/finances.sql
    
    # Compile the program
    mingw32-make
    
    # Change directory to the bin folder
    cd bin
    
    # Run the executable
    Finances.exe
    ```

4. (Optional) To clean up the generated files, run:

    ```cmd
    mingw32-make clean
    ```

