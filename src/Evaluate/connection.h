#include <set>
#include <fstream>
#ifndef MESSAGE_H
#define MESSAGE_H

#define ROUTED 100
#define ORIGINAL 102
#define IDS 101

/**
 * @brief The connection class
 * represents connection between two nodes
 * @author Lukáš Němec
 */
class connection
{
public:
    connection(int from);

    /**
     * @brief getSource
     * @return source value
     */
    int getSource() const;

    /**
     * @brief setSource
     * @param value of source
     */
    void setSource(int value);

    /**
     * @brief getDestination
     * @return destination value
     */
    int getDestination() const;

    /**
     * @brief setDestination
     * @param value of destination
     */
    void setDestination(int value);

    /**
     * @brief addMessage
     * add message to certain set specified by type
     * @param type of message
     * @param counter of message, unique message indentifier
     * @return 0 if succes, -1 fail
     */
    int addMessage(int type, int counter);

    /**
     * @brief printCurrentStats
     * prints current statistics of saved messages
     */
    void printCurrentStats();

    /**
     * @brief printDotMessagesSentRouted
     * statistics of routed - send messages
     * @return graphviz line with stats
     */
    std::string printDotMessagesSentRouted();


    /**
     * @brief printDotMessagesSum
     * statistics of sum of send messages
     * @return graphviz line with stats
     */
    std::string printDotMessagesSum();


    /**
     * @brief printDotMessagesIDS
     * statistics of IDS messages
     * @return graphviz line with stats
     */
    std::string printDotMessagesIDS();


private:

    int source;
    int destination;

    std::set<int> stillAlive;
    std::set<int> attackerDetected;
    std::set<int> stillAliveToBS;
};

#endif // MESSAGE_H
