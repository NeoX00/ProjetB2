#ifndef REQUETTEBDD_H
#define REQUETTEBDD_H

#include "Conf.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include "mysql_connection.h"
#include "mysql_driver.h"
#include "../Files/Fichier.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


using namespace std;

namespace utils
{

    class RequetteBDD
    {
        public:
            RequetteBDD(string query="");
            virtual ~RequetteBDD();
            sql::ResultSet* executeSQL(string query);
            vector<Files::Fichier> search(vector<string> words,int debut=0,int nombre=20);
        protected:
        private:
            string like(string word);
            sql::mysql::MySQL_Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
    };
}
#endif // REQUETTEBDD_H