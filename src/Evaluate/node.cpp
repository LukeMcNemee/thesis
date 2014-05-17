#include "node.h"
#include <sstream>
#include <iostream>

Node::Node(int id, std::string dotString): nodeId(id), dotString(dotString)
{
    magicReceived = false;
    aliveCountdown = 0;
    intrusionCountdown = 0;
    reportedSum = 0;
    reportsSum = 0;
    for(int i = 0; i < 51; i++){
        reportedFrom[i] = 0;
    }
}

void Node::receiveMagic(){
    magicReceived = true;
}

std::string Node::printDot(){
    std::stringstream line;
    line << nodeId << " " << "[ label = " << nodeId << " pos = \"" << dotString << "\" ";
    if(magicReceived){
        line << "color=\"green\" style=filled";
    }
    line << "]" << std::endl;

    return line.str();
}

std::string Node::printAliveDot(){
    std::stringstream line;
    line << nodeId << " " << "[ label = " << nodeId << " pos = \"" << dotString << "\" ";
    if(aliveCountdown > 0 && intrusionCountdown > 0){
        line << "color=\"orange\" style=filled";
        aliveCountdown--;
        intrusionCountdown--;
    } else if(aliveCountdown > 0){
        line << "color=\"green\" style=filled";
        aliveCountdown--;
    } else if(intrusionCountdown > 0){
        line << "color=\"red\" style=filled";
        intrusionCountdown--;
    }
    line << "]" << std::endl;
    return line.str();
}

std::string Node::getDotReported(){
    std::stringstream line;
    line << nodeId << " " << "[ label = \"id-" << nodeId << "\\n"<<reportedSum << "x\" pos = \"" << dotString << "\" ";
    line << "]" << std::endl;
    return line.str();
}

std::string Node::getDotReporting(){
    std::stringstream line;
    line << nodeId << " " << "[ label = \" id-" << nodeId << "\\n"<<reportsSum << "x\" pos = \"" << dotString << "\" ";
    line << "]" << std::endl;
    return line.str();
}

void Node::sendAlive(){
    //std::cout << "alive received" << std::endl;
    aliveCountdown = 5;
}

void Node::sendIntrusion(){
    //std::cout << "intrusion received" << std::endl;
    intrusionCountdown = 15;
}

void Node::reportFrom(int nodeId){
    reportedFrom[nodeId]++;
    reportedSum++;
}

std::string Node::getReports(){
    std::stringstream line;
    line << nodeId << " reported from: ";
    for(int i = 0; i < 51; i++){
        if(reportedFrom[i] > 0){
            line << i << " - " << reportedFrom[i] << "x, ";
        }
    }
    return line.str();
}

void Node::reports(){
    reportsSum++;
}
