#include "logfile.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstdlib>
#include <climits>

logFile::logFile(std::string filename)
{
    file.open(filename.c_str(), std::ifstream::in);
    std::cout << filename << "  " << file.is_open() << std::endl;

}

logFile::~logFile(){
    file.close();
}

bool logFile::empty(){
    if(msgs.front() == ""){
        return true;
    }
    return msgs.empty();
}

int logFile::readLine(){
    std::string line = " ";
    if (file.is_open() && file.good()){
        while(line[0] != '#' && file.good()){
            getline (file,line);
        }
        msgs.push_back(line);
        return 0;
    }
    return -1;
}

std::string removeTime(std::string msg){
    if(msg.size() < 40){
        return "";
    }
    return msg.substr(39, msg.size());
}

std::string logFile::readTop(){
    if(msgs.empty()){
        return "";
    }
    return msgs.front();
}

void logFile::removeMsg(std::string msg){
    if(msg == ""){
        return;
    }
    //check if message is present, compares messages without considering time,
    //which may differ in logfiles
    std::string payload = removeTime(msg);
    std::list<std::string>::iterator i;
    for( i = msgs.begin(); i != msgs.end(); ++ i){
        if(removeTime(*i) == payload){
            break;
        }
    }
    if(i!= msgs.end()){
        msgs.erase(i);
    }
    readLine();
}

unsigned long int logFile::topTime(){
    //return max value if msgs empty
    if(msgs.empty() || msgs.front() == ""){
        return ULONG_MAX;
    }
    std::string timeS = msgs.front().substr(6,18);
    unsigned long int time = strtol(timeS.c_str(), NULL, 10);
    return time;
}
