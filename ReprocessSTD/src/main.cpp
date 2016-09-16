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
#include <algorithm>


#define MYSQL_USER		"root"
#define MYSQL_PASSWORD		"asdctoor"
#define MYSQL_DB		"agile3_test"
#define MYSQL_HOST		"mysql"
#define QUERY_DATE_DA		"2015-7-1"
#define QUERY_DATE_A		"2016-7-27"
#define QUERY_TYPE		"FLG"

#define ORIGIN_BASE_PATH	"/tmp/"
#define DEST_BASE_PATH		"/tmp/"

#define ORIGIN_DRIFT_BASE_PATH	"/storage1/"
#define CLONE_ORIGIN_BASE_PATH	"/storage1/"
#define CLONE_BASE_PATH		"/tmp/"

#define AGILES2_STORAGE 	"agile/agile2/"
#define AGILES3_STORAGE 	"agile/agile3/"
#define LISTA_FILE_CORR 	"file_corr.csv"
#define UPDATE_DB_FILE  	"update_db.sql"
#define SELECT_DB_FILE  	"select_db.sql"
#define UPDATE_REPORT_FILE	"new_time.csv"
#define DATE 			"DATE"
#define DATE_END 		"DATE-END"
#define TSTART 			"TSTART"
#define TSTOP	 		"TSTOP"

#define EXEC_DRIFT		"/home/adc/ADC/correction/bin/cor_drift"

//#define COLLAUDO		1

using namespace std;

bool exists_file (const string& name);
string exec(const char* cmd);
string trim(string& str);
void clonaSorgenti();

int main (int   argc, char *argv[])
{
	char* 	headas=NULL;
	string 	result;
	string 	s;
	char 	nSorgentePath[1000];
	char 	nDestinazionePath[1000];
	char 	nSorgenteFile[1000];
	char 	nCorFile[1000];
	char 	nCorFileTemp[1000];
	char 	nDriftFile[1000];
	char 	time_min[50];
	char 	time_max[50];
	char 	cmd[3000];
	char 	update_cmd[3000];
	char 	select_cmd[3000];
	
	int	cont		= 0;
	int	contUpdate	= 0;
	int	resultRename	= 0;
	int	resultUnlink	= 0;
	int	resultCorr	= 0;
	bool	enableClone	= false;
	bool	updateDB	= false;
	
	memset( nSorgentePath, '\0', sizeof(nSorgentePath) );
	memset( nDestinazionePath, '\0', sizeof(nDestinazionePath) );
	memset( nSorgenteFile, '\0', sizeof(nSorgenteFile) );
	memset( nCorFile, '\0', sizeof(nCorFile) );
	memset( nCorFileTemp, '\0', sizeof(nCorFileTemp) );
	memset( nDriftFile, '\0', sizeof(nDriftFile) );
	memset( cmd, '\0', sizeof(cmd) );
	memset( update_cmd, '\0', sizeof(update_cmd) );
	memset( select_cmd, '\0', sizeof(select_cmd) );

	/* controllo parametri da linea di comando */
	if(argc>1)
	{
	    switch(argc)
	    {
		 case 2: 
		 {
		    string 	param=argv[1];
		    transform(param.begin(), param.end(),param.begin(), ::toupper);
		    
		    if(param.compare("-CLONE")==0)
		    {
			
			cout << "Abilito clone dei dati cone..." << endl;
			printf("Sorgente:     '%s'\n",ORIGIN_BASE_PATH);
			printf("Destinazione: '%s'\n",DEST_BASE_PATH);
			
			enableClone=true;
		    }
		    else if(param.compare("-UPDATEDB")==0)
		    {
			
			cout << "Abilito scrittura date su DB..." << endl;
			updateDB=true;
		    }else if(param.compare("-SHOW")==0)
		    {
			
			cout << "Parametri di default:" << endl;
		
			cout << "----- AGILE -------------------------------------------------------" << endl;
			printf("Path base:         '%s'\n",ORIGIN_DRIFT_BASE_PATH);
			printf("Path agile2:       '%s'\n",AGILES2_STORAGE);
			printf("Path agile3:       '%s'\n",AGILES3_STORAGE);
			
			cout << "----- CLONE -------------------------------------------------------" << endl;
			printf("Path origine:      '%s'\n",CLONE_ORIGIN_BASE_PATH);
			printf("Path destinazione: '%s'\n",CLONE_BASE_PATH);
			
			cout << "----- CORRETTORE --------------------------------------------------" << endl;
			printf("Eseguibile:        '%s'\n",EXEC_DRIFT);
			printf("Sorgente:          '%s'\n",ORIGIN_BASE_PATH);
			printf("Destinazione:      '%s'\n",DEST_BASE_PATH);
			
			cout << "----- MYSQL -------------------------------------------------------" << endl;
			printf("MySql-user:        '%s'\n",MYSQL_USER);
			printf("MySql-psw:         '%s'\n",MYSQL_PASSWORD);
			printf("MySql-db:          '%s'\n",MYSQL_DB);
			printf("MySql-host:        '%s'\n",MYSQL_HOST);
			printf("Query-date >=:     '%s'\n",QUERY_DATE_DA);
			printf("Query-date <=:     '%s'\n",QUERY_DATE_A);
			printf("Query-type:        '%s'\n",QUERY_TYPE);
			
			cout << "----- FILE --------------------------------------------------------" << endl;
			printf("Lista file da correggere:               '%s'\n",LISTA_FILE_CORR);
			printf("Lista update da applicate:              '%s'\n",UPDATE_DB_FILE);
			printf("Lista select per calcolare nuovi tempi: '%s'\n",SELECT_DB_FILE);
			printf("Report con nuovi tempi:                 '%s'\n",UPDATE_REPORT_FILE);

			exit(0);
		    }
		 }break;
		 
		 default: {
		     cout << "Non definito" << endl;
		}
	    }
	}
	
	sprintf(nSorgentePath,"%s/%s",ORIGIN_BASE_PATH,AGILES3_STORAGE);
	sprintf(nDestinazionePath,"%s/%s",DEST_BASE_PATH,AGILES3_STORAGE);

	//prende l'eventuale valore della variabile HEADAS
	headas = getenv("HEADAS");
	
	if(headas != NULL) /* Se l'ambiente Heasoft è settato procedi con la correzione */
	{
	    if(enableClone==true)
	    {
	      printf("****************** Start Clone STD!!! *******************\n\n\n");
	      clonaSorgenti();
	      printf("****************** Finish Clone STD!!! *******************\n\n\n");
	    }
	    else
	    {  
	      printf("****************** Start Reprocess STD!!! *******************\n\n\n");
	      printf("Step 0: Verifica environment Heasoft.\n");
	      cout << "---> HEADAS = " << headas << endl;
	      /*1 Luglio 2015 al 27 Luglio 2016*/
	      printf("\nStep 1: Prendi elenco file da correggere dal db-mysql.\n");
	      /* Lancio select su mysqldb */
	      //sprintf(cmd,"mysql -u %s -p'%s' -h %s -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = '%s' AND datemin >= '%s' AND Filename='PKP048495_1_3901_000_1473040447.flg.gz' ORDER BY id\" %s -N | sed 's/\t/,/g' > %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,QUERY_TYPE,QUERY_DATE,MYSQL_DB,LISTA_FILE_CORR);
	      sprintf(cmd,"mysql -u %s -p'%s' -h %s -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = '%s' AND datemin >= '%s' AND datemax <= '%s' ORDER BY id\" %s -N | sed 's/\t/,/g' > %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,QUERY_TYPE,QUERY_DATE_DA,QUERY_DATE_A,MYSQL_DB,LISTA_FILE_CORR);
	      cout << "	Command: " << cmd << endl;
	      
	      result = exec(cmd);
	      
	      ifstream f(LISTA_FILE_CORR); //nome del file da aprire, si può mettere anche il percorso (es C:\\file.txt)
	      ofstream out(UPDATE_DB_FILE);
	      ofstream out_select(SELECT_DB_FILE);
	      
	      
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
		      
		      /* Nome file sorgente */
		      size_t pos = s.find(","); 
		      string s1 = s.substr(pos+1);
		      string s_id = s.substr(0,pos);
		      sprintf(nSorgenteFile,"%s%s",nSorgentePath,s1.c_str());
		      
		      cont++; 
		      printf("	Step 1.1: Elaboro il file N°%d con id=%s: '%s'",cont,s_id.c_str(),nSorgenteFile);
		      
		      
		      /* Nome file destinazione */
		      sprintf(nCorFile,"%s%s",nDestinazionePath,s1.c_str());
		      
		      /* Nome file destinazione temporaneo */
		      sprintf(nCorFileTemp,"%s%s_temp",nDestinazionePath,s1.c_str());
		      
		      /* Nome file drift */
		      pos = s1.find("PKP");
		      string s_id_drift = s1.substr(pos+3,6);
		      printf("\n	Step 1.1: Individua file DRIFT con id '%s','%s/%s/LV1corr/%s/VC1/DRIFT-PKP%s_1_33XY_000.lv1.cor'.\n",s_id_drift.c_str(),ORIGIN_DRIFT_BASE_PATH,AGILES2_STORAGE,s_id_drift.c_str(),s_id_drift.c_str());
		      sprintf(nDriftFile,"%s/%s/LV1corr/%s/VC1/DRIFT-PKP%s_1_33XY_000.lv1.cor",ORIGIN_DRIFT_BASE_PATH,AGILES2_STORAGE,s_id_drift.c_str(),s_id_drift.c_str());
		      
		      if(exists_file(nDriftFile))
		      {
			if(exists_file(nSorgenteFile))
			{
			    printf("\n	Step 1.2: Applica correttore.\n");
			    
			    #ifdef COLLAUDO
				sprintf (cmd, "cp %s %s",nSorgenteFile,nCorFileTemp); 
				printf("		Command: %s\n",cmd);
			    #else
				sprintf (cmd, "%s %s %s %s",EXEC_DRIFT,nSorgenteFile,nDriftFile,nCorFileTemp);
				printf("		Command: %s\n",cmd);
			    #endif	
			    
			    resultCorr = system(cmd);
			    
			    if( resultCorr == 0)
			    {  
				if(exists_file(nCorFileTemp)) /* se tutto ok dopo il correttore */
				{

				      printf("\n	Step 1.4: elimina link simbolici.\n");
				      printf("		Command: unlink file '%s'.\n",nSorgenteFile);
				      resultUnlink = unlink(nSorgenteFile);
			      
				      if( resultUnlink == 0) /* se tutto ok dopo unlink */
				      {
					  printf("\n	Step 1.5: rename corr file.\n");
					  printf("		Command: rename '%s' to '%s'.\n",nCorFileTemp,nCorFile);
					  
					  resultRename = rename( nCorFileTemp , nCorFile );
					  
					  if ( resultRename == 0 ) /* se tutto ok dopo rename */
					  {
					      memset( time_min, '\0', sizeof(time_min) );
					      memset( time_max, '\0', sizeof(time_max) );
	
					      printf("\n	Step 1.6: Prendi key dal file fits.\n");
					  
					      /*
					      // get datemin from key DATE
					      sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nSorgenteFile,DATE);
					      printf("		Command: %s\n",cmd);
					      result = exec(cmd);
					      copy(result.begin(), result.end()-1, date_min);
					      printf("		datemin = [%s]\n",date_min);
					      
					      // get datemax from key DATE-END 
					      sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nSorgenteFile,DATE_END);
					      printf("		Command: %s\n",cmd);
					      result = exec(cmd);
					      copy(result.begin(), result.end()-1, date_max);
					      printf("		datemax = [%s]\n",date_max);
					      */
					      
					      // get timemin from key TSTART
					      sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"=\" | cut -f1 -d \"/\"",nCorFile,TSTART);
					      printf("		Command: %s\n",cmd);
					      result = exec(cmd);
					      result=trim(result);
					      copy(result.begin(), result.end()-2, time_min);
					      printf("		timemin = [%s]\n",time_min);
					      
					      // get timemax from key TSTOP 
					      sprintf(cmd,"fkeyprint %s+0 %s exact=yes | grep = | cut -f2 -d \"=\" | cut -f1 -d \"/\"",nCorFile,TSTOP);
					      printf("		Command: %s\n",cmd);
					      result = exec(cmd);
					      result=trim(result);
					      copy(result.begin(), result.end()-2, time_max);
					      printf("		timemax = [%s]\n",time_max);
					      
					      sprintf(update_cmd,"UPDATE PIPE_ArchivedFile b SET b.datemin = DATE_ADD(b.datemin, INTERVAL (SELECT delta FROM (SELECT (%s-timemin) as delta FROM PIPE_ArchivedFile WHERE id=%s) AS sub_delta) SECOND),b.datemax = DATE_ADD(b.datemax, INTERVAL (SELECT delta_b FROM (SELECT (%s-timemax) as delta_b FROM PIPE_ArchivedFile WHERE id=%s) AS sub_delta_b ) SECOND),timemin=%s,timemax=%s WHERE b.id=%s;\n",time_min,s_id.c_str(),time_max,s_id.c_str(),time_min,time_max,s_id.c_str());
					      sprintf(select_cmd,"SELECT id, Filename, datemin, datemax, timemin, timemax, (%s-timemin) as delta_timemin, (%s-timemax) as delta_timemax,(timemin+(%s-timemin)) as new_timemin,(timemax+(%s-timemax)) as new_timemax,DATE_ADD(datemin, INTERVAL (%s-timemin) SECOND)  as new_datemin, DATE_ADD(datemax, INTERVAL (%s-timemax) SECOND)  as new_datemax FROM PIPE_ArchivedFile where id=%s;\n",time_min,time_max,time_min,time_max,time_min,time_max,s_id.c_str());
					      
					      out << update_cmd;
					      out_select << select_cmd;
					      
					      contUpdate++;
					  }
					  else
					  {
					      printf("	WARNING: rename fallito !!!!!!!\n");
					  }
				      }
				      else
				      {
					  /*sprintf(cmd,"rename %s %s",nCorFileTemp,nCorFile);
					  cout << cmd << endl;
					  resultCorr = system(cmd);*/
					  resultRename = rename( nCorFileTemp , nCorFile );
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
			    printf("	WARNING: Il file N°%d con id=%s: '%s' non esiste !!!!!!!\n",cont,s_id.c_str(),nSorgenteFile);
			}
		      }
		      else
		      {
			  printf("	WARNING: drift file '%s' non presente !!!!!!!\n",nDriftFile);
		      }
		      printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		  }
		  
		  if(contUpdate>0)
		  {
		      out << "commit;";
		  }
		  
		  f.close(); //chiude il file di input
		  out.close(); // chiude il file di output
		  out_select.close();
	      }
	      
	      if(contUpdate>0)
	      {
		  printf("Step 2: Crea report update.\n");  
		  sprintf(cmd,"mysql -u %s -p'%s' -h %s %s -N < %s > %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,MYSQL_DB,SELECT_DB_FILE,UPDATE_REPORT_FILE); 
		  printf("	Command: %s\n",cmd);
		  result = exec(cmd);
		  
		  if(updateDB==true)
		  {
			  printf("Step 3: Aggiorna mysqldb.\n");
			  sprintf(cmd,"mysql -u %s -p'%s' -h %s %s < %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,MYSQL_DB,UPDATE_DB_FILE);
			  printf("	Command: %s\n",cmd);
			  result = exec(cmd);
		  }
	      }
	      else
	      {
		  printf("Step 2: non ci sono aggionamenti da fare db-mysql.\n");
	      }
	      
	      printf("\n\n\n");
	      printf("****************** Finish Reprocess STD!!! *******************\n");
	    }
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

void clonaSorgenti()
{
	string 	result;
	string 	s;
	char 	cmd[3000];
	char 	nSorgentePath[1000];
	char 	nDestinazionePath[1000];
	char 	nSorgenteFile[1000];
	char 	nDestinazioneFile[1000];
	int	iResult;
	int	cont=0;
	
	sprintf(nSorgentePath,"%s/%s",CLONE_ORIGIN_BASE_PATH,AGILES3_STORAGE);
	sprintf(nDestinazionePath,"%s/%s",CLONE_BASE_PATH,AGILES3_STORAGE);
	
	printf("\nStep 0: Clona dati.\n");
	/* Lancio select su mysqldb */
	//sprintf(cmd,"mysql -u %s -p'%s' -h %s -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = '%s' AND datemin >= '%s' AND Filename='PKP048495_1_3901_000_1473040447.flg.gz' ORDER BY id\" %s -N | sed 's/\t/,/g' > %s",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,QUERY_TYPE,QUERY_DATE,MYSQL_DB,LISTA_FILE_CORR);
	sprintf(cmd,"mysql -u %s -p'%s' -h %s -e \"SELECT id,CONCAT(path,'/',Filename) as fName from PIPE_ArchivedFile WHERE Type = '%s' AND datemin >= '%s' AND datemax <= '%s' ORDER BY id\" %s -N | sed 's/\t/,/g' > %s ",MYSQL_USER,MYSQL_PASSWORD,MYSQL_HOST,QUERY_TYPE,QUERY_DATE_DA,QUERY_DATE_A,MYSQL_DB,LISTA_FILE_CORR);
	cout << "	Command: " << cmd << endl;
	result = exec(cmd);
	
	ifstream f(LISTA_FILE_CORR);
	    
	if(!f) 
	{
	    printf("ERROR: Il file csv '%s' non è stato creato correttamente!\n",LISTA_FILE_CORR);
	}
	else
	{
	    /* Scorro l'elenco dei record nel file csv generato dal'interrogazione al mysqldb: LISTA_FILE_CORR  */
	    while(getline(f, s)) //fino a quando c'è qualcosa da leggere ..
	    {
		    /* Nome file sorgente */
		    size_t pos = s.find(","); 
		    string s1 = s.substr(pos+1);
		    string s_id = s.substr(0,pos);
		    

		    cont++;
		    sprintf(nSorgenteFile,"%s%s",nSorgentePath,s1.c_str());
		    
		    if(exists_file(nSorgenteFile))
		    {
			/* Nome file destinazione */
			sprintf(nDestinazioneFile,"%s%s",nDestinazionePath,s1.c_str());
			
			sprintf(cmd,"install -D %s %s",nSorgenteFile,nDestinazioneFile);
			
			printf("%d: %s\n",cont,cmd);
			result = system(cmd);
			iResult = unlink(nDestinazioneFile);
			printf("unlink file '%s'.\n",nDestinazioneFile);
			    
			if( iResult == 0) /* se tutto ok dopo unlink */
			{
			    sprintf(cmd,"ls -lrta %s | tail -n 1 | cut -f11 -d \" \"",nSorgenteFile);
			    cout << cmd << endl;
			    result = exec(cmd);
			    
			    result=trim(result);
			    
			    copy(result.begin(), result.end()-2, nSorgenteFile);
			    sprintf(cmd,"ln -s %s %s",nSorgenteFile,nDestinazioneFile);
			    cout << cmd << endl;
			    result = exec(cmd);
		      }
		    }
		    else
		    {
		      printf("%d: WARNING: File '%s' inesistente !!!\n",cont,nSorgenteFile);
		    }
	    }
	}	  
}

