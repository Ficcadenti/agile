/*
 * main.cpp
 *
 * Copyright (C) 2016-2016 by Raffaele Ficcadenti
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// reprocess_std
//
////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream> 
#include <unistd.h>


#define AGILES3_STORAGE "/home/raffo/agile/storage1/agile/agile3/"
#define LISTA_FILE_CORR "file_corr.csv"
#define UPDATE_DB_FILE  "update_db.sql"
#define DATE1 		"DATE"

using namespace std;

bool exists_file (const string& name);
string exec(const char* cmd);

int main (int   argc, char *argv[])
{
	char* 	headas = getenv("HEADAS");
	string 	result;
	string 	s;
	char 	agiles3_path[100];
	char 	nFile[1000];
	char 	date1[50];
	char 	cmd[3000];
	char 	update_cmd[3000];
	int	cont=0;
	
	strcpy(agiles3_path,AGILES3_STORAGE);

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
	      cout << "	Command: " << cmd << endl;
	      result = exec(cmd);
	    }
	    
	    /* Scorro l'elenco dei record nel file generato 'fits_destinazione.txt'  */
	    //printf("---> file da elaborare = [%s]\n",nFile);
	    
	    ifstream f(LISTA_FILE_CORR); //nome del file da aprire, si può mettere anche il percorso (es C:\\file.txt)
	    ofstream out(UPDATE_DB_FILE);
	    
	    if(!f) 
	    {
		printf("ERROR: Il file '%s' non esiste!\n",LISTA_FILE_CORR);
		return -1;
	    }
	    else
	    {

		while(getline(f, s)) //fino a quando c'è qualcosa da leggere ..
		{
		    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		    size_t pos = s.find(","); 
		    string s1 = s.substr(pos+1);
		    string s_id = s.substr(0,pos);
		   
		    sprintf(nFile,"%s%s",agiles3_path,s1.c_str());
		     
		    printf("	Elaboro il file N°%d con id=%s: %s",++cont,s_id.c_str(),nFile);
		    printf("\n	Step 1.2: Individua file drift.\n");
		    printf("\n	Step 1.3: Applica correttore.\n");
		    printf("\n	Step 1.4: Aggiorna link simbolici.\n");
		   
		    if(exists_file(nFile))
		    {
			/* se tutto ok dopo il correttore */
			/*if( unlink(nFile) )
			{
			    printf("Warning: Unable to remove '%s' link.\n", nFile);
			}
			else
			{
			    printf("\nStep 1.5: unlink file '%s'.\n",nFile);
			}*/
			printf("\n	Step 1.5: unlink file '%s'.\n",nFile);
			printf("\n	Step 1.6: Prendi key dal file fits.\n");
			sprintf(cmd,"fkeyprint %s+1 %s | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nFile,DATE1);
			printf("		Command: %s\n",cmd);
			result = exec(cmd);
			copy(result.begin(), result.end()-1, date1);
		    
			printf("		DATE1 = [%s]\n",date1);
			
			sprintf(update_cmd,"update PIPE_ArchivedFile set datemin = '%s' where id=%s;\n",date1,s_id.c_str());
			
			out << update_cmd;
		    }
		    else
		    {
			printf("	WARNING: Il file '%s' non esiste !!!!!!!\n",nFile);
		    }
		    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
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
	    printf("ERROR: Ambiente Heasoft non settato!!!!\n");
	}
	
	return 0;
}


bool exists_file (const string& name) {
    ifstream f(name.c_str());
    return f.good();
}

string exec(const char* cmd) {
    char buffer[128];
    string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw runtime_error("popen() failed!");
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
