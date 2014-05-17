/**
 * @brief Program for evaluation of logfiles
 *
 * opens all files specified as parameters and creates source files for graphviz
 * containing information gathered from logfiles
 * @author Lukáš Němec
 * @version 1.0
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <istream>
#include <iterator>
#include <cstdlib>
#include <map>
#include <sstream>

#include "connection.h"
#include "node.h"

#define MAX_NODE_ID 50

//amount of messages to be skipped when creating animations,
//creates shorter begining sequence
#define SKIP 6000


//IDs of nodes present in network
int arraysID [] ={41, 44, 25, 10, 33, 46, 47, 48, 4, 19, 6,
                  28, 37, 17, 7, 15, 14, 43, 30, 35, 22, 42,
                  36, 40, 5, 31, 50, 32, 29};

//vector of IDs for easier acces
std::vector<int> IDs (arraysID, arraysID + sizeof(arraysID) / sizeof(int));

//map of nodes
std::map <int, std::string> nodeMap;


template <typename T>
std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

/**
 * @brief printBasic
 * creates basic .dot file structure in file, sets labels and positions to nodes
 * @param graph file to be writen into
 */
void printBasic(std::ofstream& graph){
    graph << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
    graph << "digraph M \{" << std::endl;
    graph << "ratio = 0.8" << std::endl;

    for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
        graph << *it << "[ label = " << *it << " pos = \"" << nodeMap[*it] << "\"]" << std::endl;
    }
}

/**
 * @brief main
 * @param argc
 * @param argv paths to logfiles
 * @return
 */
int main(int argc, char* argv[])
{

    //initialize
    std::map <int, connection*> connections;
    for (int i = 0; i <= MAX_NODE_ID; i++){
        connections[i] = new connection(i);
    }
    int senders[MAX_NODE_ID+1] = {0};
    int privacylevel = -1;
    std::string privacylevelString = "";
    unsigned long int magicReceived = 0;
    unsigned long int aliveReceived = 0;
    unsigned long int lastTime = 0;
    unsigned long int lines = 0;

    //create map of node positions, each ID is used as key in map, value is string containing position for graphviz
    nodeMap[41] = "0,0!";
    nodeMap[44] = "0,100!";
    nodeMap[25] = "0,200!";
    nodeMap[10] ="0,300!";
    nodeMap[33] = "70,70!";
    nodeMap[46] = "150,150!";
    nodeMap[47]= "150,250!";
    nodeMap[48] = "200,70!";
    nodeMap[4]  = "70,-70!";
    nodeMap[19] = "200,0!";
    nodeMap[6]= "300,0!";
    nodeMap[28] = "250,-200!";
    nodeMap[37] = "0,-100!";
    nodeMap[17]= "70,-200!";
    nodeMap[7]= "150,-250!";
    nodeMap[15]= "50,-300!";
    nodeMap[14]= "-30,-200!";
    nodeMap[43]= "-70,-300!";
    nodeMap[30]= "-200,-250!";
    nodeMap[35]= "-120,-180!";
    nodeMap[22]= "-70,-70!";
    nodeMap[42]= "-170,-130!";
    nodeMap[36]= "-250,-180!";
    nodeMap[40] = "-200,0!";
    nodeMap[5] = "-300,0!";
    nodeMap[31]= "-70,70!";
    nodeMap[50]= "-150,150!";
    nodeMap[32]= "-250,200!";
    nodeMap[29]= "-150,250!";

    //maps of IDs and coresponding nodes
    std::map<int, Node*> nodes;

    std::map<int, Node*> aliveNodes;

    std::map<int, Node*> idsNodes;

    //renew nodes in maps
    for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
        aliveNodes[*it] = (new Node(*it, nodeMap[*it] ));
        idsNodes[*it] = (new Node(*it, nodeMap[*it] ));
    }

    //for each file specified as input do
    if(argc >= 2){
        for(int i = 1; i < argc; i++){
            std::string filepath = argv[i];
            std::string line;
            std::ifstream myfile (filepath.c_str());


            if (myfile.is_open())
            {
                //process each line in file
                while ( myfile.good() )
                {
                    getline (myfile,line);

                    //check for #, which indicates data line
                    if(line[0] == '#'){
                        lines++;
                        //split line by spaces
                        std::stringstream ss(line);
                        std::istream_iterator<std::string> begin(ss);
                        std::istream_iterator<std::string> end;
                        std::vector<std::string> vstrings(begin, end);

                        //convert sender id from hex to int
                        unsigned int senderId;
                        std::stringstream ss2;
                        ss2 << std::hex << vstrings[7];
                        ss2 >> senderId;

                        senders[senderId]++;

                        //if(false) used so each else if branch can be commented or uncommented and run separately
                        if(false){

                        } else if(vstrings[8] == "12"){ // check for right length still alive or intruder
                            if(vstrings[24] == "64" && vstrings[12] == "00"){ //still alive message

                                //get all required data from message
                                std::stringstream ss3;
                                int counter;
                                ss3 << std::hex << vstrings[28];
                                ss3 >> counter;
                                std::stringstream ss4;
                                unsigned int realSender;
                                ss4 << std::hex << vstrings[26];
                                ss4 >> realSender;

                                if(senderId == realSender){
                                    //message originates from node
                                    connections.find(senderId)->second->addMessage(ORIGINAL, counter);
                                }else{
                                    //mesage is routed by node
                                    connections.find(senderId)->second->addMessage(ROUTED, counter);
                                }
                                std::stringstream ss5;
                                int destination;
                                ss5 << std::hex << vstrings[17];
                                ss5 >> destination;
                                //std::cout << "destiantion " << destination <<std::endl;
                                connections.find(senderId)->second->setDestination(destination);

                                if(lines > SKIP){ //skip some messages from front in order to make shorter animation
                                    aliveNodes[realSender]->sendAlive();

                                    //create numbered file, which will be ordered correctly for over 10000 files
                                    std::ofstream graphAnim;
                                    std::string filename = "alive";
                                    filename.append("/");
                                    if(aliveReceived < 10){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 100){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 1000){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 10000){
                                        filename.append("0");
                                    }
                                    filename.append(NumberToString(aliveReceived));
                                    filename.append(".dot");
                                    graphAnim.open (filename.c_str());
                                    graphAnim << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
                                    graphAnim << "digraph M \{" << std::endl;
                                    graphAnim << "ratio = 0.8" << std::endl;

                                    //write all nodes to file
                                    std::string line2;
                                    for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
                                        line2 = aliveNodes[*it]->printAliveDot();
                                        graphAnim << line2 << std::endl;
                                    }
                                    graphAnim << "}";
                                    graphAnim.close();
                                    aliveReceived++;
                                }

                            } else if (vstrings[24] == "65" && vstrings[12] == "00"){ //IDS message
                                if(lines > SKIP){
                                    unsigned int realSender;
                                    std::stringstream ss4;
                                    ss4 << std::hex << vstrings[26];
                                    ss4 >> realSender;

                                    std::stringstream ss3;
                                    int counter;
                                    ss3 << std::hex << vstrings[28];
                                    ss3 >> counter;

                                    connections.find(realSender)->second->addMessage(IDS, counter);

                                    aliveNodes[realSender]->sendIntrusion();

                                    std::ofstream graphAnim;
                                    std::string filename = "alive";
                                    filename.append("/");
                                    if(aliveReceived < 10){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 100){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 1000){
                                        filename.append("0");
                                    }
                                    if(aliveReceived < 10000){
                                        filename.append("0");
                                    }
                                    filename.append(NumberToString(aliveReceived));
                                    filename.append(".dot");
                                    graphAnim.open (filename.c_str());
                                    graphAnim << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
                                    graphAnim << "digraph M \{" << std::endl;
                                    graphAnim << "ratio = 0.8" << std::endl;
                                    std::string line2;
                                    for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
                                        line2 = aliveNodes[*it]->printAliveDot();
                                        graphAnim << line2 << std::endl;
                                    }
                                    graphAnim << "}";
                                    graphAnim.close();
                                    aliveReceived++;
                                }

                            }
                        } else if(vstrings[8] == "14"){ //magic packet

                            if( vstrings[13] != privacylevelString){ //magic packet for new privacy level e.g. first magic packet with this level

                                privacylevel++;
                                std::cout << "new privacylevel " << privacylevel << std::endl;
                                std::cout << line << std::endl;
                                privacylevelString = vstrings[13];
                                //renew nodes
                                for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
                                    nodes[*it] = (new Node(*it, nodeMap[*it] ));
                                }
                                magicReceived = 0;
                            }

                            //get timestamp from msg
                            std::string timeS = line.substr(6,18);
                            unsigned long int time = strtol(timeS.c_str(), NULL, 10);

                            if(time - lastTime > 1000) {
                                lastTime = 0;
                            }
                            if(lastTime == 0) {
                                lastTime = time - 1;
                            }
                            if(lastTime == time) {
                                lastTime--;
                            }

                            //generate images between last time and time, number of images is same as time passed between
                            for(unsigned long int uli = lastTime; uli < time || uli < 80; uli++){
                                magicReceived++;

                                //create file in folder acording to privacy level
                                std::ofstream graphAnim;
                                std::string filename = NumberToString(privacylevel);
                                filename.append("/");
                                if(magicReceived < 10){
                                    filename.append("0");
                                }
                                if(magicReceived < 100){
                                    filename.append("0");
                                }
                                if(magicReceived < 1000){
                                    filename.append("0");
                                }
                                filename.append(NumberToString(magicReceived));
                                filename.append(".dot");
                                graphAnim.open (filename.c_str());
                                graphAnim << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
                                graphAnim << "digraph M \{" << std::endl;
                                graphAnim << "ratio = 0.8" << std::endl;
                                std::string line2;
                                for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
                                    line2 = nodes[*it]->printDot();
                                    graphAnim << line2 << std::endl;
                                }
                                graphAnim << "}";
                                graphAnim.close();

                            }
                            nodes[senderId]->receiveMagic();
                            lastTime = time;

                        } else if(vstrings[8] == "0A"){ //IDS

                            unsigned int realSender;
                            std::stringstream ss3;
                            ss3 << std::hex << vstrings[11];
                            ss3 >> realSender;

                            unsigned int detectedDropper;
                            std::stringstream ss4;
                            ss4 << std::hex << vstrings[17];
                            ss4 >> detectedDropper;

                            if(senderId == realSender){
                                idsNodes[detectedDropper]->reportFrom(realSender);
                                idsNodes[realSender]->reports();
                            }

                        }
                        //return 0; // only if one line needed for debug
                    }

                }
                myfile.close();


            }
            else
            {
                std::cerr << "Unable to open file" << std::endl;
            }
        }

        //create IDS text report
        std::ofstream idsReport;
        idsReport.open("ids.txt");
        std::string idsLine;
        for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
            idsLine = idsNodes[*it]->getReports();
            idsReport << idsLine << std::endl;
        }
        idsReport.close();

        //create IDS graphs
        std::ofstream graph;
        graph.open("idsReported.dot");
        graph << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
        graph << "digraph M \{" << std::endl;

        graph << "ratio = 0.8" << std::endl;
        graph << "label=\"Nodes reported as droppers\";" << std::endl;

        for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
            idsLine = idsNodes[*it]->getDotReported();
            graph << idsLine << std::endl;
        }
        graph << "}" << std::endl;
        graph.close();


        graph.open("idsReporting.dot");

        graph << "#usage: dot -Kneato -n -Tsvg graph.dot -o graph.svg" << std::endl;
        graph << "digraph M \{" << std::endl;

        graph << "ratio = 0.8" << std::endl;
        graph << "label=\"Nodes reporting others as droppers\";" << std::endl;
        for(std::vector<int>::iterator it = IDs.begin(); it != IDs.end(); ++it){
            idsLine = idsNodes[*it]->getDotReporting();
            graph << idsLine << std::endl;
        }
        graph << "}" << std::endl;
        graph.close();

        //create still alive msgs graphs
        graph.open ("messages.dot");
        printBasic(graph);

        for(int i = 0; i <= 50; i++){
            if(senders[i] != 0){

                printf("%d node id, %d messages recived\n", i, senders[i]);
                connections.find(i)->second->printCurrentStats();
                graph << connections.find(i)->second->printDotMessagesSentRouted();
            }
        }
        graph << "}" << std::endl;
        graph.close();


        graph.open ("messagesSum.dot");
        printBasic(graph);

        for(int i = 0; i <= 50; i++){
            if(senders[i] != 0){

                printf("%d node id, %d messages recived\n", i, senders[i]);
                connections.find(i)->second->printCurrentStats();
                graph << connections.find(i)->second->printDotMessagesSum();
            }
        }

        graph << "}" << std::endl;
        graph.close();

        //create intrusion graph
        graph.open ("attackerSum.dot");
        printBasic(graph);
        for(int i = 0; i <= 50; i++){
            if(senders[i] != 0){

                printf("%d node id, %d messages recived\n", i, senders[i]);
                connections.find(i)->second->printCurrentStats();
                graph << connections.find(i)->second->printDotMessagesIDS();
            }
        }

        graph << "}" << std::endl;
        graph.close();

    } else {
        std::cout << "no file specified" << std::endl;
        std::cout << "usage:" << std::endl;
        std::cout << "specify selected logfiles as parameters" << std::endl;
        std::cout << "create folders for each privacy level in work folder (0, 1, 2, 3)" << std::endl;
    }
    return 0;
}

