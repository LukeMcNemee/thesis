#include "connection.h"
#include <string>
#include <sstream>
#include <iostream>

connection::connection(int i)
{
    source = i;
}

int connection::getSource() const
{
    return source;
}

void connection::setSource(int value)
{
    source = value;
}

int connection::getDestination() const
{
    return destination;
}

void connection::setDestination(int value)
{
    destination = value;
}

int connection::addMessage(int type, int counter){
    //std::cout << "pokus" << stillAlive.size() <<std::endl;
    if(type == ORIGINAL){

        stillAlive.insert(counter);
    } else if(type == ROUTED){
        stillAliveToBS.insert(counter);
    } else if(type == IDS) {
        attackerDetected.insert(counter);
    }
    return 0;
}

void connection::printCurrentStats(){

    std::cout << "source "<< source << " sent "<< stillAlive.size() << " messages to " << destination <<std::endl;
}

std::string connection::printDotMessagesSentRouted(){
    std::stringstream line;
    if(stillAlive.size() == 0){
        return "";
    }
    line << source << " -> " << destination << " [label=\""<< stillAlive.size()
           << " - " << stillAliveToBS.size() << "\"];" << std::endl;
    return line.str();
}

std::string connection::printDotMessagesSum(){
    std::stringstream line;
    if(stillAlive.size() == 0){
        return "";
    }
    line << source << " -> " << destination << " [label=\""<< stillAlive.size() +
            stillAliveToBS.size() << "\"];" << std::endl;
    return line.str();
}

std::string connection::printDotMessagesIDS(){
    std::stringstream line;
    if(stillAlive.size() == 0){
        return "";
    }
    line << source << " -> " << destination << " [label=\""<< attackerDetected.size() << "\"];" << std::endl;
    return line.str();

}







