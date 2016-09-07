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



#define DATE1 "DATE"


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
	
	char nome_file[500];
	char path_file[500];
	char nFile[1000];
	char date1[50];

	char cmd[3000];
	
	
	
	strcpy(nome_file,"PKP048495_1_3901_000_1473040447.flg");
	strcpy(path_file,"/home/raffo/agile/agile-test/");
	strcpy(nFile,path_file);
	strcat(nFile,nome_file);
	
	//mostra il valore della variabile HEADAS
	
	if(headas != NULL) 
	{
	    
	    printf("****************** Start Reprocess STD!!! *******************\n\n\n");
	    
	    cout << "HEADAS = " << headas << endl;
	    
	    printf("Step 1: Prendi elenco file da correggere dal db-mysql.\n");
	    printf("---> file da elaborare = [%s]\n",nFile);
	    
	    
	    printf("Step 2: Applica correttore.\n");
	    
	    
	    printf("Step 3: Aggiorna link simbolici.\n");
	    
	    
	    printf("Step 4: Aggiorna date su db-mysql.\n");
	    
	    sprintf(cmd,"fkeyprint %s+1 %s | grep = | cut -f2 -d \"'\" | cut -f1 -d \"/\"",nFile,DATE1);
	    cout << "---> " << cmd << endl;
	    
	    result = exec(cmd);
	    std::copy(result.begin(), result.end(), date1);
	    
	    cout << "---> DATE = [" << DATE1 << "] = " << date1 << endl;
	    
	    
	    printf("\n\n\n");
	    printf("****************** Finish Reprocess STD!!! *******************");
	}
	else
	{
	    cout << "Ambiente Heasoft non settato!!!!\n";
	}
	
	return 0;
}