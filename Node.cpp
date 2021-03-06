/**
  * @developers Zachary L. Johnson, Rony Calderon, and Logan Long
  *
  * @details Self repairing Mesh Network
  *
  * @version 1.00
  *          Original development (December 6, 2017)
  *
*/

#ifndef NODE_CPP
#define NODE_CPP

#include "Node.h"

// CONSTRUCTOR/DESTRUCTOR

Node::Node()
{

    activeNode = false;
    posX = -1;
    posY = -1;
    passBackCount = 1;
}

Node::~Node()
{


}

void Node::activateNode(int x, int y)
{

    activeNode = true;
    posX = x;
    posY = y;

}

bool Node::isActive()
{

    return activeNode;

}

void Node::addNeighbor(Node* p_Neighbor)
{
    int distanceX = p_Neighbor->posX - posX;
    int distanceY = p_Neighbor->posY - posY;

    if(distanceX < 0)
    {
      distanceX *= -1;
    }

    if(distanceY < 0)
    {
      distanceY *= -1;
    }

    // Set up new Neighbor and fill data
    Neighbor* newNeighbor = new Neighbor();
    newNeighbor->node = p_Neighbor;
    newNeighbor->weightBetween = distanceX + distanceY;

    neighborStructs.push_back(newNeighbor);

}

int Node::getNeighborCount()
{

    return neighborStructs.size();

}

void Node::startPackageDelivery(Node* destNode)
{

    // Package is received
    std::cout << "Received package at postion (" << posX << ", " << posY << ")" << std::endl;

    //  Build a package with both start node, destination node,
    Package* newPackage = new Package();
    newPackage->startNode = this;
    newPackage->destNode = destNode;

    // packageHandler
    packageHandler(newPackage);

}

void Node::packageHandler(Package* p_Package)
{

    srand(time(NULL));

    // Run dijkstra to find the correct path for the package
    std::vector<Dijkstra*> v;
    std::vector<Node*> visited;
    dijkstraHandler(v, 0, visited, p_Package->destNode);

    // Send to destination
    if(pathToDestination[pathToDestination.size()-1] == p_Package->destNode)
    {

      // TEST with broken path
      if(rand()%5 != 0)
      {
        pathToDestination[pathToDestination.size()-(rand()%(pathToDestination.size()-1)+1)]->setActive(false);
      }
      std::cout << "Begin delivery of package to destination" << std::endl << std::endl;
      sendPackageToDestination(p_Package, pathToDestination, this, p_Package->destNode);
    }else
    { // ERROR TODO
      std::cout << "Trouble reaching destination and cannot deliver package" << std::endl;
    }

}

void Node::sendPackageToDestination(Package* p_Package, std::vector<Node*> visited, Node* startNode, Node* destNode)
{
  //std::cout << "Enter SendPackageToDestination" << std::endl;
  std::vector<Node*> holdNode;
  if(this == destNode)
  {

    std::cout << "Delivery Success" << std::endl;

  }else
  {

    // If the next node is still active pass along the correct path else you'll need to run MST
    if(visited[1]->isActive())
    {
      // can't pop top so I'll just make a new one
      for(unsigned int x = 1; x < visited.size(); x++)
      {

        holdNode.push_back(visited[x]);

      }
      visited[1]->sendPackageToDestination(p_Package, holdNode, startNode, destNode);

    }else // Else correct the pathToDestination with MST and begin sending again
    {

      // Using MST Prim's version
      std::cout << "No response from next path node (" << visited[1]->posX << ", " << visited[1]->posY << ")" "... repair the path using MST" << std::endl;
      std::vector<MST*> v;
      MSTHandler(v, this, destNode);

    }
  }

}

bool Node::checkContainerMST(std::vector<MST*> container, Node* pNode)
{

  for(unsigned int x = 0; x < container.size(); x++)
  {
    if(container[x]->toNode == pNode)
    {
        return true;
    }
  }

  return false;

}

void Node::MSTHandler(std::vector<MST*> pathOptions, Node* startNode, Node* destNode)
{



  MST* holdNode = NULL;
  int holdWeight = 999;

  if(pathOptions.size() == 0)
  {

    // add the starting path which is this
    holdNode = new MST();
    holdNode->toNode = this;
    holdNode->fromNode = this;
    holdNode->visited = true;
    holdNode->weight = 0;
    pathOptions.push_back(holdNode);

  }

  for(unsigned int x = 0; x < neighborStructs.size(); x++)
  {

    if(neighborStructs[x]->node == destNode)
    {
      if(neighborStructs[x]->node->isActive())
      {
        std::cout << "Found corrected path and finish delivery to (" << destNode->posX << ", " << destNode->posY << ")" << std::endl;
        /*pathToDestination.clear();

        int value = x;
        while(neighborStructs[x] != startNode)
        {
          pathToDestination.push_back(neighborStructs[x]->node);
        }
        */
        return;
      }else
      {

        std::cout << "Seems like the destination node is no longer active... can't deliver to that node" << std::endl;
        return;

      }
    }

    // for each neighbor check if they are in the container // && !wasVisited(visited, neighborStructs[x]->node)
    if(!checkContainerMST(pathOptions, neighborStructs[x]->node) && neighborStructs[x]->node->isActive())
    {
      //std::cout << "added(" << neighborStructs[x]->node->posX << ", " << neighborStructs[x]->node->posY << ")" << std::endl;
      // Add this path to our pathOptions
      holdNode = new MST();
      holdNode->toNode = neighborStructs[x]->node;
      holdNode->fromNode = this;
      holdNode->visited = false;
      holdNode->weight = neighborStructs[x]->weightBetween;

      pathOptions.push_back(holdNode);
    }

  }

  // Check all paths that have not been traveled to yet and decide who is the best
  for(unsigned int x = 0; x < pathOptions.size(); x++)
  {
    if(pathOptions[x]->weight < holdWeight && !pathOptions[x]->visited)
    {
      //std::cout << "picking path(" << pathOptions[x]->toNode->posX << ", " << pathOptions[x]->toNode->posY << ")" << std::endl;
      holdNode = pathOptions[x];
      holdWeight = pathOptions[x]->weight;

    }

  }

  if(allVisited(pathOptions))
  {

    std::cout << "MST has checked all paths and failed to find any other way to the destination node" << std::endl;
    return;

  }

  std::cout << "MST has selected a new path to check out which is (" << holdNode->toNode->posX << ", " << holdNode->toNode->posY << ")" << std::endl;
  // go to the node that has been selected
  holdNode->visited = true;
  holdNode->toNode->MSTHandler(pathOptions, startNode, destNode);

}

bool Node::allVisited(std::vector<MST*> container)
{

  for(unsigned int x = 0; x < container.size(); x++)
  {

    if(container[x]->visited == false)
    {

      return false;

    }

  }

  return true;

}

void Node::setActive(bool p)
{

  activeNode = p;

}

bool Node::checkContainer(std::vector<Dijkstra*> pContainer, Node* pNode)
{

  for(unsigned int x = 0; x < pContainer.size(); x++)
  {

    if(pContainer[x]->toNode == pNode)
    {

      return true;

    }

  }

  return false;

}

bool Node::wasVisited(std::vector<Node*> pContainer, Node* pNode)
{
  for(unsigned int x = 0; x < pContainer.size(); x++)
  {

    if(pContainer[x] == pNode)
    {
      return true;
    }
  }

  return false;

}

void Node::dijkstraHandler(std::vector<Dijkstra*> container, int currentWeight, std::vector<Node*> visited, Node* destNode)
{

  std::cout << "Adding new information to Dijkstra graph at (" << posX << ", " << posY << ")" << std::endl;
  bool check = true;
  for(unsigned int x = 0; x < visited.size(); x++)
  {
    if(visited[x] == this)
    {
      check = false;
    }
  }
  if(check)
  {
    visited.push_back(this);
  }
  Dijkstra* newDijkstra = NULL;
  // Possible updated routes
  std::vector<Dijkstra*> holdPossibleRoutes;

  // Check this nodes neighbors to see if there is the destination
  for(unsigned int x = 0; x < neighborStructs.size(); x++)
  {

    if(neighborStructs[x]->node == destNode)
    {

      visited.push_back(neighborStructs[x]->node);
      std::cout << "Return calculated Dijkstra information back to package start" << std::endl;
      sendBackToStart(visited, visited.size()-1);
      return;


    }

  }

  // Create new dijkstra structs based on the neighbors
  for(unsigned int x = 0; x < neighborStructs.size(); x++)
  {

    // for each neighbor check if they are in the container
    if(!checkContainer(container, neighborStructs[x]->node) && !wasVisited(visited, neighborStructs[x]->node))
    {
      // if it isn't in the container you should add it
      newDijkstra = new Dijkstra();
      newDijkstra->toNode = neighborStructs[x]->node;
      newDijkstra->fromNode = this;
      newDijkstra->weight = neighborStructs[x]->weightBetween;

      container.push_back(newDijkstra);

      // since I just added this node it is a holdPossibleRoutes
      holdPossibleRoutes.push_back(newDijkstra);

      //std::cout << "Pushed on new node" << std::endl;

    }

  }

  // Update container to have corrected weights for each neighbor
  for(unsigned int x = 0; x < neighborStructs.size(); x++)
  {
    // Each neighbor is now for sure in the container of dijkstra
    // for each of the neighbors go through the container and change info
    for(unsigned int y = 0; y < container.size(); y ++)
    {
        // for neighbor x is this his data y?
        if(neighborStructs[x]->node == container[y]->toNode)
        {
          // if the current weight + weight between is less than that nodes weight then replace
          if(neighborStructs[x]->weightBetween + currentWeight < container[y]->weight && !wasVisited(visited, neighborStructs[x]->node))
          {
            container[y]->weight = neighborStructs[x]->weightBetween + currentWeight;
            std::cout << "Replaced a weight" << std::endl;

            // since it was modified it is also a possible route
            holdPossibleRoutes.push_back(container[y]);

          }
        }
    }
  }



  // based on the possible routes pick the correct one
  int lowestWeight = 999;
  Node* passToNode = NULL;
  for(unsigned int x = 0; x < holdPossibleRoutes.size(); x++)
  {

    if(holdPossibleRoutes[x]->weight < lowestWeight)
    {
        lowestWeight = holdPossibleRoutes[x]->weight;
        passToNode = holdPossibleRoutes[x]->toNode;
    }

  }

  if(visited[visited.size()-1] == destNode)
  {

    // we are complete and need to head back with all the new info
    std::cout << "Return calculated Dijkstra information back to package start" << std::endl;
    sendBackToStart(visited, visited.size()-1);
    return;

  }else
  {
    if(passToNode != NULL)
    {
      passBackCount = 2;
      passToNode->dijkstraHandler(container, lowestWeight, visited, destNode);
    }else
    {
      // is there a neighbor who hasn't been visited then?
      for(unsigned int x = 0; x < neighborStructs.size(); x++)
      {

        if(!wasVisited(visited, neighborStructs[x]->node))
        {
          // try this path atleast
          passToNode = neighborStructs[x]->node;
          passBackCount = 2;
          passToNode->dijkstraHandler(container, lowestWeight, visited, destNode);
          return;

        }

      }

        passBackCount++;
        visited[visited.size()-passBackCount]->dijkstraHandler(container, lowestWeight, visited, destNode);
      }
      // else we need to backtrack to choose a different path
      // Check the container for another from to
      //for(unsigned int x = 0; x < container.size(); x++)
      //for(unsigned int x = visited.size()-2; x > -1; x--)
      //{

          // find a from that doesn't go to this
          //if(container[x]->fromNode == visited[visited.size()-2] && container[x]->toNode != this)
          //{

            // send to new found node
//            visited.pop_back();
//            container[x]->toNode->dijkstraHandler(container, currentWeight, visited, destNode);

          //}
      //}


  }

}

void Node::sendBackToStart(std::vector<Node*> visited, int position)
{
  //std::cout << "Send collected information back to package node" << std::endl;
  if(this != visited[0])
  {
    visited[position-1]->sendBackToStart(visited, position-1);
  }else
  {
    for(unsigned int x = 0; x < visited.size(); x++)
    {
      pathToDestination.push_back(visited[x]);
    }
  }
}

std::string Node::printNodeScreen()
{

    if(activeNode)
    {

        return "N";

    }

    //return std::string();
    return " ";

}

// TODO: Modify the output to specify the node being printed and its neighbors
std::string Node::printNodeFile()
{

    if(activeNode)
    {

        return "N";

    }

    //return std::string();
    return " ";

}

#endif
