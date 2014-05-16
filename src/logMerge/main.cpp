/**
 * @brief Program for merging log files
 *
 * opens all files specified as parameters and merges them into one file
 * which is mergeLogs.txt, and this file is created in work directory
 *
 * @author Lukáš Němec
 * @version 1.0
 */

#include <iostream>
#include <string>
#include <fstream>
#include <list>
#include "logfile.h"
#include <iterator>

/**
 * @brief main
 * @param argc
 * @param argv should contain paths to all logfiles that are required to merge
 * @return
 */
int main(int argc, char* argv[])
{
    //create list of opened logfiles
    std::list<logFile*> logFiles;
    if(argc >= 2){
        for(int i = 1; i < argc; i++){
            std::string filepath = argv[i];
            logFiles.push_back(new logFile(filepath));
        }
    }

    //read first 10 lines from each logfile
    std::cout << logFiles.size() << std::endl;
    for(std::list<logFile*>::iterator i = logFiles.begin(); i != logFiles.end(); ++ i){
        for(int j = 0; j < 10; j++){
            (*(*i)).readLine();
        }
    }

    bool empty = true;
    for(std::list<logFile*>::iterator i = logFiles.begin(); i != logFiles.end(); ++ i){
        if((*(*i)).empty() != true){
            empty = false;
        }
    }
    //create output file
    std::ofstream output;
    output.open ("mergeLogs.txt");

    //while not all logs empty do
    while( !empty){

        //find lowest time
        std::list<logFile*>::iterator headTime = logFiles.begin();
        for(std::list<logFile*>::iterator i = logFiles.begin(); i != logFiles.end(); ++ i){
            if((*(*i)).topTime() < (*(*headTime)).topTime()){
                headTime = i;
            }
        }

        //detele selected message from all logfiles and write it into output file
        std::string msg = (*(*headTime)).readTop();
        for(std::list<logFile*>::iterator i = logFiles.begin(); i != logFiles.end(); ++ i){
            (*(*i)).removeMsg(msg);
        }
        output << msg << std::endl;


        //recalculate if logs are empty
        empty = true;
        for(std::list<logFile*>::iterator i = logFiles.begin(); i != logFiles.end(); ++ i){
            if((*(*i)).empty() != true){
                empty = false;
            }
        }
    }
    output.close();
    return 0;
}

