#include "RequetteBDD.h"
#include <regex>

using namespace utils;
using namespace std;







RequetteBDD::RequetteBDD(string query)
{

    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect(Conf::getConf("adresseMysql"), Conf::getConf("userMysql"), Conf::getConf("mdpMysql"));//connection au serveur

    con->setSchema( Conf::getConf("BDDname")); //selection de la bdd
    stmt = con->createStatement();

    stmt->execute("SET NAMES 'utf8';");


    if(query!="")
    {
        executeSQL(query);
    }
}

vector<Files::Fichier> RequetteBDD::search(vector<string> words,int debut,int nombre)
{

    if(words.size()>2)
        if(words.at(words.size()-2).find("d=")==0
                && words.at(words.size()-1).find("nb=")==0)
        {
            debut = stoi(words.at( words.size()-2).substr(2));
            nombre = stoi(words.at(words.size()-1).substr(3));

            words.erase(words.end());
            words.erase(words.end());//on supprime les 2 derniers mots
        }

    ostringstream query("");
    query <<"SELECT * FROM files WHERE ";
    for(unsigned i = 0; i < words.size(); i++)
    {
        if(i>0)
            query<<"OR ";
        query<< "url "<<like(words[i])<<" OR motImportant "<<like(words[i])<< " OR txt "<<like(words[i])<<" ";
    }

    if(debut < nombre)
        query << "LIMIT " << debut <<","<< nombre;
    //query << " ORDER BY  DESC LIMIT 1 " ;//
    sql::ResultSet  *result;

    result = executeSQL(query.str());

    vector<Files::Fichier> resultTab;

    while (result->next())
    {

        resultTab.push_back( Files::Fichier( result->getString("titre"),
                                             result->getString("url"),
                                             result->getString("motImportant"),
                                             result->getInt("type"),
                                             utils::str::calculNote( words,result->getString("txt"),
                                                     result->getString("motImportant"))
                                           )
                           );

    }

    result->close();

    delete result;
    return resultTab;
}



string RequetteBDD::oldestCrawl() //crawl le plus ancien
{
    ostringstream query("");
    query<<"SELECT url FROM files WHERE lastcrawl < '";
    query <<(time(NULL)-(31*24*3600))<<"' ORDER BY RAND() LIMIT 1";

    sql::ResultSet  *result;

    result = executeSQL(query.str());


    if(result->next())
    {
        string urlToReturn(result->getString("url"));
        delete result;
        return urlToReturn;
    }

    result->close();

    delete result;
    return "";
}



void RequetteBDD::add(Files::Fichier file)
{
    RequetteBDD reqVerif;
    if(reqVerif.verifUrl(file.getURL().getUri()))
    {
        try
        {
            string query = "INSERT INTO files(titre,url,type,txt,motImportant,lastcrawl) VALUES (?,?,?,?,?,?) ";

            sql::PreparedStatement  *prep_stmt;

            prep_stmt = con->prepareStatement(query);

            prep_stmt->setString(1,file.getNom()); //titre
            prep_stmt->setString(2,file.getURL().getUri()); //url
            prep_stmt->setInt(3,file.getTypeInt()); //type
            istringstream stream(file.getTextFull());
            prep_stmt->setBlob(4,&stream); //txt
            prep_stmt->setString(5,Algo::generateMotImportant(file)); //mot important
            prep_stmt->setInt(6,time(NULL)); //timstamp

            prep_stmt->execute();

            delete prep_stmt;
        }
        catch(...) {}
    }
    else
        update(file,file.getURL().getUri(),0);
}


void RequetteBDD::add(vector<string> urls)
{

    string query = "INSERT INTO files(url,lastcrawl) VALUES (?,?) ";
    sql::PreparedStatement  *prep_stmt;
    prep_stmt = con->prepareStatement(query);


    for(uint i=0; i<urls.size(); i++)
    {
        RequetteBDD reqVerif;
        if(reqVerif.verifUrl(urls[i]))
        {
            try
            {
                prep_stmt->setString(1,urls[i]); //url
                prep_stmt->setInt(2,0); //timstamp
                prep_stmt->executeUpdate();
            }
            catch(...) {}
        }
    }

    delete prep_stmt;
}

bool RequetteBDD::verifUrl(string url)
{

    ostringstream query("");
    //on verifie si l'url n'est pas deja dans la
    query <<"SELECT url FROM files WHERE url='"<<url<<"' LIMIT 1";

    try
    {

        sql::ResultSet  *result;

        result = executeSQL(query.str());

        if(result->next())
        {
            delete result;
            return false;

        }
        else
            return true;
        delete result;
    }
    catch(...) {}
    return false;
}

void RequetteBDD::update(Files::Fichier file,string url,int temps)
{
    if(!file.isEmpty())
    {
        int tps=0;
        string urlAsave="";
        string query = "UPDATE files SET titre=?,url=?,type=?,txt=?,motImportant=?,lastcrawl=? WHERE url=? ";

        if(url!=file.getURL().getUri() && verifUrl(file.getURL().getUri()))
        {
            urlAsave = file.getURL().getUri();
            if(temps==0)
                tps =time(NULL); //timstamp
            else
                tps = temps;
        }
        else
        {
            urlAsave = url;
            tps = (time(NULL)+3600*24*365);
        }

        sql::PreparedStatement  *prep_stmt;

        prep_stmt = con->prepareStatement(query);
        int i=1;
        prep_stmt->setString(i++,file.getNom()); //titre
        prep_stmt->setString(i++,urlAsave); //url
        prep_stmt->setInt(i++,file.getTypeInt()); //type
        prep_stmt->setString(i++,file.getTextFull()); //txt
        prep_stmt->setString(i++,Algo::generateMotImportant(file)); //mot important


        prep_stmt->setInt(i++,tps);//temps imposé

        prep_stmt->setString(i++,url); //where url

        prep_stmt->execute();
        delete prep_stmt;

    }

}


string RequetteBDD::like(string word)
{
    utils::str::supprimerTousLesCharacteres(word,' ');
    return "LIKE '%"+word+"%'";
}


sql::ResultSet* RequetteBDD::executeSQL(string query)
{
    return stmt->executeQuery(query);
}


RequetteBDD::~RequetteBDD()
{
    delete stmt;
    delete con;
}

