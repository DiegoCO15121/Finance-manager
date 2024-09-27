#include <iostream>
/*pqxx is a library recovered from this Github repository: https://github.com/jtv/libpqxx.git */
#include <pqxx/pqxx>  // This library allows us to establish a conection with postgreSQL

/* Including functions definitions (Headerfiles) */
#include "interfaces.h"

using namespace std;

int main(){
    // Setting connection with the database
    const string conn_info = "host=localhost port=YourDefaultPort user=yourUser password=*** dbname=finances";

    try {
        pqxx::connection conn(conn_info);

        // Calling the interface to login or register
        startInterface(conn);
        
    }
    catch(exception const &e){ // Catching any exception 
    // cerr is similar to cout, the diference is that cerr is used to display errors and it does not store in the buffer
        cerr<<"Error: "<<e.what(); 
    }

    return 0;
}