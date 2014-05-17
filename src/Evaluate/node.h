#ifndef NODE_H
#define NODE_H
#include <string>

/**
 * @brief The Node class
 * this class represents single node
 * @author Lukáš Němec
 */
class Node
{
public:
    Node(int id, std::string dotString);

    /**
     * @brief receiveMagic
     * this node received magic packet
     */
    void receiveMagic();

    /**
     * @brief printDot
     * print line with correct graphviz format for animation of magic packet
     * @return line with dot format
     */
    std::string printDot();

    /**
     * @brief printAliveDot
     * return dot line with correct format for graphviz for still alive animation
     * @return line with dot format
     */
    std::string printAliveDot();

    /**
     * @brief sendAlive
     * this node sent still alive message
     */
    void sendAlive();

    /**
     * @brief sendIntrusion
     * this node sent message of attacker intrusion
     */
    void sendIntrusion();

    /**
     * @brief reportFrom
     * this node is reported from another node IDS
     * @param nodeId reporting node
     */
    void reportFrom(int nodeId);

    /**
     * @brief reports
     * node sends IDS report about another node
     */
    void reports();

    /**
     * @brief getReports
     * statistics of IDS reports for this node in text format
     * @return line with stats
     */
    std::string getReports();

    /**
     * @brief getDotReported
     * get dot format with number of IDS reports for node
     * @return
     */
    std::string getDotReported();

    /**
     * @brief getDotReporting
     * get dot format with IDS reports outgoing from this node
     * @return line with stats
     */
    std::string getDotReporting();

private:
    int nodeId;
    int reportedFrom[51];
    int reportsSum;
    int reportedSum;
    std::string dotString;
    bool magicReceived;
    unsigned int aliveCountdown;
    unsigned int intrusionCountdown;
};

#endif // NODE_H
