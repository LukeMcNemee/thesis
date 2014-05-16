#ifndef LOGFILE_H
#define LOGFILE_H
#include <string>
#include <list>
#include <fstream>

/**
 * @brief The logFile class
 * class for management of single logfile
 * @author Lukáš Němec
 */
class logFile
{
public:
    /**
     * @brief logFile
     * @param filename path to logfile
     */
    logFile(std::string filename);

    ~logFile();

    /**
     * @brief readLine
     * reads one line from file and saves it to msgs list
     * @return
     */
    int readLine();

    /**
     * @brief readTop
     * returns mesagge at top of msgs list
     * @return if msgs list is empty, then 0, else top message
     */
    std::string readTop();

    /**
     * @brief removeMsg
     * if msg is in msgs list, then removes message from msgs list
     * @param msg message to be removed
     */
    void removeMsg(std::string msg);

    /**
     * @brief topTime
     * extracts time stamp from top message
     * @return top timestamp
     */
    unsigned long int topTime();

    /**
     * @brief empty
     * checks if msgs list is empty or contains just empty lines
     * @return true if msgs list is empty or contains empty lines else false
     */
    bool empty();
private:

    std::string filename;
    std::ifstream file;
    std::list<std::string> msgs;
};

/**
 * @brief removeTime
 * function for removal of timestamp from message
 * @param msg message to be processed
 * @return msg without timestamp
 */
std::string removeTime(std::string msg);

#endif // LOGFILE_H
