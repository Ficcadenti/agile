/*
 * main.cpp
 *
 * Copyright (C) 2016-2016 by Raffaele Ficcadenti
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// test_smooth
//
////////////////////////////////////////////////////////////////////////////////


//My Class
#include "reprocess_sdd.h"

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream> //header necessario per ifstream!


#define AGILES3_STORAGE "/home/raffo/agile/storage1/agile/agile3/"
#define LISTA_FILE_CORR "file_corr.csv"
#define UPDATE_DB_FILE  "update_db.sql"
#define DATE1 		"DATE"


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try 
    {
        while (!feof(pipe)) 
	{
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } 
    catch (...) 
    {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main (int   argc, char *argv[])
{

	reprocess_sdd  _processSTD;
	char* headas = getenv("HEADAS");
	std::string result;
	string s;
	int cont;
	
	
	cont=0;
	char agiles3_path[100];
	char nFile[1000];
	char date1[50];

	char cmd[3000];
	char update_cmd[3000];
	
	strcpy(agiles3_path,AGILES3_STORAGE);

	
	cout << "-----------------> nFile = " << nFile << endl;
	
	//mostra il valore della variabile HEADAS
	
	if(headas != NULL) 
	{
	    
	    printf("****************** Start Reprocess STD!!! *******************\n\n\n");
	    
	    
	    printf("Step 0: Verifica environment Heasoft.\n");
	    cout << "---> HEADAS = " << headas << endl;
	    
	    printf("\nStep 1: Prendi elenco file da correggere dal db-mysql.\n");
	    /* Lancio select su mysqldb */
	    {
	      sprintf(cmd,"mysql -u root -p'root' -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = 'FLG' AND datemin >= '2015-6-30' AND Filename='PKP048495_1_3901_000_1473040447.flg.gz' ORDER BY id\" agile3 -N | sed 's/\t/,/g' > %s",LISTA_FILE_CORR);
	      cout << "---> " << cmd << endl;
	      result = exec(cmd);
	    }
	    
	    /* Scorro l'elenco dei record nel file generato 'fits_destinazione.txt'  */
	    //printf("---> file da elaborare = [%s]\n",nFile);
	    
	    std::ifstream f;
	    std::ofstream out(UPDATE_DB_FILE);
    
	    f.open(LISTA_FILE_CORR); //nome del file da aprire, si può mettere anche il percorso (es C:\\file.txt)
	    
	    if(!f) 
	    {
		cout << "Il " << LISTA_FILE_CORR << "file non esiste!";
		return -1;
	    }
	    else
	    {

		while(std::getline(f, s)) //fino a quando c'è qualcosa da leggere ..
		{
		    std::size_t pos = s.find(","); 
		    std::string s1 = s.substr(pos+1);
		    std::string s_id = s.substr(0,pos);
		   

		    sprintf(nFile,"%s%s",agiles3_path,s1.c_str());
		    
		    
		    cout<< cont++ << "---> Elaboro il file("<< s_id << "): " << s <<endl;
		    printf("\nStep 1.2: Individua file drift.\n");
		    printf("\nStep 1.3: Applica correttore.\n");
		    printf("\nStep 1.4: Aggiorna link simbolici.\n");
		    printf("\nStep 1.5: Prendi key dal file fits.\n");
		    
		    sprintf(cmd,"fkeyprint %s+1 %s | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nFile,DATE1);
		    cout << "---> " << cmd << endl;
		    result = exec(cmd);
		    std::copy(result.begin(), result.end()-1, date1);
		
		    cout << "---> DATE1 = [" << date1 << "]" << endl;
		    
		    sprintf(update_cmd,"update PIPE_ArchivedFile set datemin = '%s' where id=%s;\n",date1,s_id.c_str());
		    
		    out << update_cmd;
		}
		
		out << "commit;";
		
		f.close(); //chiude il file di input
		out.close(); // chiude il file di output
	    }
	    
	    printf("Step 2: aggiorna db-mysql.\n");
	    
	    printf("\n\n\n");
	    printf("****************** Finish Reprocess STD!!! *******************");
	}
	else
	{
	    cout << "Ambiente Heasoft non settato!!!!\n";
	}
	
	return 0;
}