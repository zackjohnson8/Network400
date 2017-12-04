#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>

class Node
{

struct Package{

    Node* startNode;
    Node* destNode;
    std::vector<Node*> vistedNodes;

};

struct Neighbor{

    Node* node;
    int weightBetween;

};

struct Dijkstra
{

    Node* toNode;
    Node* fromNode;
    int weight;

};

struct MST
{

  Node* toNode;
  bool visited;

};

public:

    // constructor/destructor
    Node();
    ~Node();
    std::string printNodeFile();
    std::string printNodeScreen();
    void activateNode(int, int);
    bool isActive();
    void addNeighbor(Node*);
    int getNeighborCount();
    void startPackageDelivery(Node*);

private:

    void packageHandler(Package*);
    void dijkstraHandler(std::vector<Dijkstra*>, int, std::vector<Node*>, Node*);
    bool checkContainer(std::vector<Dijkstra*>, Node*);
    bool wasVisited(std::vector<Node*>, Node*);
    void sendBackToStart(std::vector<Node*>, int);
    void sendPackageToDestination(Package*, std::vector<Node*>, Node*, Node*);
    void MSTHandler(std::vector<MST*>);
    bool checkContainerNode(std::vector<Node*>, Node*);

    bool activeNode;
    int posX;
    int posY;
    std::vector<Neighbor*> neighborStructs;
    std::vector<Node*> pathToDestination;

};


#endif
