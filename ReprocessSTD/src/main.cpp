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
#include <memory>


#define MYSQL_USER		"root"
#define MYSQL_PASSWORD		"root"
#define MYSQL_DB		"agile3"
#define MYSQL_HOST		"localhost"
#define QUERY_DATE		"2015-6-30"
#define QUERY_TYPE		"FLG"
#define AGILES3_STORAGE 	"/home/raffo/agile/storage1/agile/agile3/"
#define LISTA_FILE_CORR 	"file_corr.csv"
#define UPDATE_DB_FILE  	"update_db.sql"
#define DATE 			"DATE"
#define DATE_END 		"DATE-END"
#define TSTART 			"TSTART"
#define TSTOP	 		"TSTOP"

using namespace std;

bool exists_file (const string& name);
string exec(const char* cmd);
string trim(string& str);


int main (int   argc, char *argv[])
{
	char* 	headas=NULL;
	string 	result;
	string 	s;
	char 	agiles3_path[100];
	char 	nFile[1000];
	char 	corr_File[1000];
	//char 	date_min[50]; /* unused */
	//char 	date_max[50]; /* unused */
	char 	time_min[50];
	char 	time_max[50];
	char 	cmd[3000];
	char 	update_cmd[3000];
	int	cont=0;
	int	contUpdate=0;
	int	resultRename;
	int	resultUnlink;
	int	resultCorr;
	
	strcpy(agiles3_path,AGILES3_STORAGE);

	//prende l'eventuale valore della variabile HEADAS
	headas = getenv("HEADAS");
	
	if(headas != NULL) /* Se l'ambiente Heasoft è settato procedi con la correzione */
	{
	    printf("****************** Start Reprocess STD!!! *******************\n\n\n");
	    
	    printf("Step 0: Verifica environment Heasoft.\n");
	    cout << "---> HEADAS = " << headas << endl;
	    
	    printf("\nStep 1: Prendi elenco file da correggere dal db-mysql.\n");
	    /* Lancio select su mysqldb */
	    sprintf(cmd,"mysql -u %s -p'%s' -h %s -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = '%s' AND datemin >= '%s' ORDER BY id\" %s -N | sed 's/\t/,/g' > %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,QUERY_TYPE,QUERY_DATE,MYSQL_DB,LISTA_FILE_CORR);
	    cout << "	Command: " << cmd << endl;
	    result = exec(cmd);
	    
	    
	    ifstream f(LISTA_FILE_CORR); //nome del file da aprire, si può mettere anche il percorso (es C:\\file.txt)
	    ofstream out(UPDATE_DB_FILE);
	    
	    
	    if(!f) 
	    {
		printf("ERROR: Il file csv '%s' non è stato creato correttamente!\n",LISTA_FILE_CORR);
		return -1;
	    }
	    else
	    {
		/* Scorro l'elenco dei record nel file csv generato dal'interrogazione al mysqldb: LISTA_FILE_CORR  */
		while(getline(f, s)) //fino a quando c'è qualcosa da leggere ..
		{
		    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		    size_t pos = s.find(","); 
		    string s1 = s.substr(pos+1);
		    string s_id = s.substr(0,pos);
		   
		    sprintf(nFile,"%s%s",agiles3_path,s1.c_str());
		    cont++; 
		    if(exists_file(nFile))
		    {
			printf("	Elaboro il file N°%d con id=%s: '%s'",cont,s_id.c_str(),nFile);
			printf("\n	Step 1.2: Individua file drift.\n");
			printf("\n	Step 1.3: Applica correttore.\n");
			
			/* per collaudo unlink */
			sprintf(corr_File,"%s_temp",nFile);
			sprintf (cmd, "cp %s %s",nFile,corr_File);
			printf("		cor_drift %s <DRIFT_FILE> %s\n",nFile,corr_File);
			resultCorr = system(cmd);
			
			if( resultCorr == 0)
			{  
			    if(exists_file(corr_File)) /* se tutto ok dopo il correttore */
			    {
				  printf("\n	Step 1.4: elimina link simbolici.\n");
				  printf("		unlink file '%s'.\n",nFile);
				  
				  resultUnlink = unlink(nFile);
				  
				  if( resultUnlink == 0) /* se tutto ok dopo unlink */
				  {
				      printf("\n	Step 1.5: rename corr file.\n");
				      printf("		rename '%s' to '%s'.\n",corr_File,nFile);
				      
				      resultRename= rename( corr_File , nFile );
				      
				      if ( resultRename == 0 ) /* se tutto ok dopo rename */
				      {
					  printf(" File successfully renamed" );
					  printf("\n	Step 1.6: Prendi key dal file fits.\n");
				      
					  /*
					  // get datemin from key DATE
					  sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nFile,DATE);
					  printf("		Command: %s\n",cmd);
					  result = exec(cmd);
					  copy(result.begin(), result.end()-1, date_min);
					  printf("		datemin = [%s]\n",date_min);
					  
					  // get datemax from key DATE-END 
					  sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nFile,DATE_END);
					  printf("		Command: %s\n",cmd);
					  result = exec(cmd);
					  copy(result.begin(), result.end()-1, date_max);
					  printf("		datemax = [%s]\n",date_max);
					  */
					  
					  // get timemin from key TSTART
					  sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"=\" | cut -f1 -d \"/\"",nFile,TSTART);
					  printf("		Command: %s\n",cmd);
					  result = exec(cmd);
					  result=trim(result);
					  copy(result.begin(), result.end()-2, time_min);
					  printf("		timemin = [%s]\n",time_min);
					  
					  // get timemax from key TSTOP 
					  sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"=\" | cut -f1 -d \"/\"",nFile,TSTOP);
					  printf("		Command: %s\n",cmd);
					  result = exec(cmd);
					  result=trim(result);
					  copy(result.begin(), result.end()-2, time_max);
					  printf("		timemax = [%s]\n",time_max);
					  
					  sprintf(update_cmd,"UPDATE PIPE_ArchivedFile b SET b.datemin = DATE_ADD(b.datemin, INTERVAL (SELECT delta FROM (SELECT (%s-timemin) as delta FROM PIPE_ArchivedFile WHERE id=%s) AS sub_delta) SECOND),b.datemax = DATE_ADD(b.datemax, INTERVAL (SELECT delta_b FROM (SELECT (%s-timemax) as delta_b FROM PIPE_ArchivedFile WHERE id=%s) AS sub_delta_b ) SECOND),timemin=%s,timemax=%s WHERE b.id=%s;\n",time_min,s_id.c_str(),time_max,s_id.c_str(),time_min,time_max,s_id.c_str());
					  
					  //sprintf(update_cmd,"update PIPE_ArchivedFile set datemin = '%s',datemax = '%s',timemin = %s,timemax = %s where id=%s;\n",date_min,date_max,time_min,time_max,s_id.c_str());
					  
					  out << update_cmd;
					  contUpdate++;
				      }
				      else
				      {
					  printf("	WARNING: rename fallito !!!!!!!\n");
				      }
				  }
				  else
				  {
				      printf("	WARNING: unlink fallito !!!!!!!\n");
				  }
			    }
			    else
			    {
				  printf("	WARNING: corr_file non presente !!!!!!!\n");
			    }
			}
			else
			{
			      printf("	WARNING: correttore fallito !!!!!!!\n");
			}
		    }
		    else
		    {
			printf("	WARNING: Il file N°%d con id=%s: '%s' non esiste !!!!!!!\n",cont,s_id.c_str(),nFile);
		    }
		    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		}
		
		if(contUpdate>0)
		{
		    out << "commit;";
		}
		
		f.close(); //chiude il file di input
		out.close(); // chiude il file di output
	    }
	    
	    if(contUpdate>0)
	    {
		printf("Step 2: aggiorna mysqldb.\n");
		sprintf(cmd,"mysql -u %s -p'%s' -h %s %s < %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,MYSQL_DB,UPDATE_DB_FILE);
		printf("	Command: %s\n",cmd);
		result = exec(cmd);
		/* mysql -u root -p'root' agile3 < update.sql */
	    }
	    else
	    {
		printf("Step 2: non ci sono aggionamenti da fare db-mysql.\n");
	    }
	    
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

string trim(string& str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

