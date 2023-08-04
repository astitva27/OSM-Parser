#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <climits>
using namespace std;
#include "code/rapidxml_utils.hpp"
using namespace rapidxml;
typedef struct node
{ // Struct to store the nodes in osm file
    string id;
    double latitude, longitude;
    string name;
} node;
typedef struct way
{ // Struct to store the nodes in osm file

    string id;

    vector<pair<string, node>> path;
} way;

inline bool check_substring(string text, string substring)
{
    transform(text.begin(), text.end(), text.begin(), ::tolower); // converts to lower case
    transform(substring.begin(), substring.end(), substring.begin(), ::tolower);

    return (text.find(substring) != string::npos);
}
bool inline compare(pair<double, node> p1, pair<double, node> p2) { return p1.first < p2.first; }
void inline print_path(map<string,string> parent,string id){
    if(id=="") return;
    print_path(parent,parent[id]);
    cout<<id<<" -> ";
}
void print_matching_name(vector<node> &nodeList);
void dijkstra(map<string, map<string, double>> &adjacencyList, string source_id, string destination_id);
void find_k_nearest_neighbour(vector<node> &nodeList);

int findNode(vector<node> &nodeList, string id)
{
    for (int i = 0; i < nodeList.size(); i++)
    {
        if (nodeList[i].id == id)
        {

            return i;
        }
    }
    return -1;
}

static double haversine(double lat1, double lon1,
                        double lat2, double lon2)
{

    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) *
                  M_PI / 180.0;

    // convert to radians
    lat1 = (lat1)*M_PI / 180.0;
    lat2 = (lat2)*M_PI / 180.0;

    // apply formulae
    double a = pow(sin(dLat / 2), 2) +
               pow(sin(dLon / 2), 2) *
                   cos(lat1) * cos(lat2);
    double rad = 6372.797560;
    double c = 2 * asin(sqrt(a));
    return rad * c;
}
int main()

{
    //
    file<> osmFile("map.osm");
    xml_document<> doc;
    doc.parse<0>(osmFile.data());
    xml_node<> *currNode = doc.first_node(); // A pointer to the first node in OSM file

    // Vectors to store the nodes and ways
    vector<node> nodeList;
    vector<way> wayList;
    // Stores the value of current node and current attribute while traversing the OSM file
    node curr_node;
    xml_attribute<> *pAttr;
    // Traversing through the OSM file to store the data of all elements named node
    for (xml_node<> *pNode = currNode->first_node("node"); pNode; pNode = pNode->next_sibling("node"))
    {

        // Parsing the ID latitude and longitude
        pAttr = pNode->first_attribute("id");
        if (!pAttr)
            cout << "ID doesn't exist for this particular node";

        else
            curr_node.id = (pAttr->value());
        pAttr = pNode->first_attribute("lat");
        if (!pAttr)
            cout << "Latitude doesn't exist";

        else
            curr_node.latitude = stold(pAttr->value());
        pAttr = pNode->first_attribute("lon");
        if (!pAttr)
            cout << "Latitude doesn't exist";

        else
            curr_node.longitude = stold(pAttr->value());

        curr_node.name = "";
        for (xml_node<> *childNode = pNode->first_node("tag"); childNode; childNode = childNode->next_sibling("tag"))
        {
            pAttr = childNode->first_attribute("k");

            if (!pAttr) // NULL checking
                break;
            string name = pAttr->value();
            if (name == "name")
            {
                curr_node.name = childNode->first_attribute("v")->value();
            }
        }
        nodeList.push_back(curr_node);
    }
    node dummy_node;
    // Storing the wayID and ID of nodes contained in its path
    for (xml_node<> *wayNode = currNode->first_node("way"); wayNode; wayNode = wayNode->next_sibling("way"))
    {
        way way_node;
        way_node.id = (wayNode->first_attribute("id")->value());
        for (xml_node<> *childNode = wayNode->first_node("nd"); childNode; childNode = childNode->next_sibling("nd"))
        {
            way_node.path.push_back({childNode->first_attribute("ref")->value(), dummy_node});
        }
        wayList.push_back(way_node);
    }

    // Storing the data of nodes as key:ID value:struct node in map as we need the latitude and longitude of all nodes in a way to
    // calculate distance
    map<string, node> temp;
    for (auto node1 : nodeList)
    {
        temp[node1.id] = node1;
    }

    // Updating the node id in ways to include their respective latitude and longitude

    for (auto &way1 : wayList)
    {

        for (auto &node1 : way1.path)

        {

            node1.second = temp[node1.first];
        }
    }

    map<string, map<string, double>> adjacencyList; // Stores the dist and adjacent vertices for all the vertices
    for (auto way1 : wayList)
    {
        for (int i = 0; i < way1.path.size() - 1; i++)
        {

            double c_dist = haversine(way1.path[i].second.latitude, way1.path[i].second.longitude, way1.path[i + 1].second.latitude, way1.path[i + 1].second.longitude);
            adjacencyList[way1.path[i].first][way1.path[i + 1].first] = c_dist;
            adjacencyList[way1.path[i + 1].first][way1.path[i].first] = c_dist;
        }
    }
    int opt;
    string id1, id2;
    do
    {
        cout << endl
             << endl;
        cout << "+-----------------------------------------------------------------------------------------+" << endl;
        cout << "| ";
        cout << "Enter the corresponding number on the left to use the corresponding functionality       |" << endl;
        cout << "| ";
        cout << "1: Display the number of nodes and ways in OSM file                                     |" << endl;
        cout << "| ";
        cout << "2: Search for a node using name                                                         |" << endl;
        cout << "| ";
        cout << "3: Find the K nearest node                                                              |" << endl;
        cout << "| ";
        cout << "4: Find the shortest path between two nodes                                             |" << endl;
        cout << "| ";
        cout << "5: -1 to exit                                                                           |" << endl;
        cout << "+-----------------------------------------------------------------------------------------+" << endl;

        cin >> opt;

        while (cin.fail())
        {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Enter a valid number between 1 to 4 or -1: ";
            cin >> opt;
        }
        switch (opt)
        {
        case 1:
            cout << "No of nodes discovered are: " << nodeList.size() << endl;

            cout << "No of ways discovered are: " << wayList.size() << endl;
            break;
        case 2:
            print_matching_name(nodeList);
            break;
        case 3:
            find_k_nearest_neighbour(nodeList);
            break;
        case 4:

            cout << "Enter the id of source node" << endl;
            cin >> id1;
            if (findNode(nodeList, id1) == -1)
            {
                cout << "No Node with given ID exist" << endl;
                break;
            }
            cout << "Enter the id of destination node" << endl;
            cin >> id2;
            if (findNode(nodeList, id2) == -1)
            {
                cout << "No Node with given ID exist" << endl;
                break;
            }
            dijkstra(adjacencyList, id1, id2);
            break;
        case -1:
            break;
        default:
            cout << "Enter a valid number between 1 to 4 or -1: ";
        }

    } while (opt != -1 || (opt > 5 and opt < 1));

    return 0;
}

//
void dijkstra(map<string, map<string, double>> &adjacencyList, string source_id, string destination_id) // Implementation of dijkstra algo using priority queue

{

    priority_queue<pair<double, string>> min_dist; // priority queue(-ve dist to make it min heap) to store the value of distance and vertex number
    map<string, double> dist;
    map<string,string>parent;
    for(auto x:adjacencyList){
        parent[x.first]="";
    }
    for (auto x : adjacencyList)
    {
        dist[x.first] = INT_MAX;
    }
    dist[source_id] = 0;
    min_dist.push({0, source_id});
    while (not min_dist.empty())
    {
        string u = min_dist.top().second;
        min_dist.pop();
        for (auto adj_nodes : adjacencyList[u])
        {
            double curr_dist = adj_nodes.second + dist[u];
            if (curr_dist < dist[adj_nodes.first])
            {
                parent[adj_nodes.first]=u;

                dist[adj_nodes.first] = curr_dist;
                min_dist.push({-curr_dist, adj_nodes.first}); //Pushing -ve distance so it works like min heap
            }
        }
    }
    // If the distance is INF then path doesn't exist otherwise return the shortest distance
    cout<<"Minimum distance: ";
    dist[destination_id] != (double)INT_MAX ? cout << dist[destination_id] <<endl: cout << "No path" << endl;
    cout<<"Shortest path: ";
    print_path(parent,destination_id);
}
void print_matching_name(vector<node> &nodeList)
{
    string substring;
    bool noName = true;
    cout << "Enter the text to search: ";
    cin >> substring;
    for (auto node : nodeList)
    {
        if (node.name != "" and check_substring(node.name, substring))
        {
            cout << node.id << ' ' << node.name << ' ' << node.latitude << ' ' << node.longitude << endl;
            noName = false;
        }
    }
    if (noName)
        cout << "No node name with given substring exist";
}
void find_k_nearest_neighbour(vector<node> &nodeList)
{
    /*
    Prints the nearest k neighbour to given node

    */
    string id;
    cout << "Enter the id of node ";
    cin >> id;
    int node_pos = findNode(nodeList, id);

    if (node_pos == -1)
    {
        cout << "No node with given ID exist" << endl;
        return;
    }
    vector<pair<double, node>> distance;
    for (int i = 0; i < nodeList.size(); i++)
    {

        double dist = haversine(nodeList[i].latitude, nodeList[i].longitude, nodeList[node_pos].latitude, nodeList[node_pos].longitude);
        distance.push_back({dist, nodeList[i]});
    }
    int k;
    cout << "Enter k: ";
    cin >> k;
    if (k >= nodeList.size())
    {
        cout << "K can't be greater than total number of nodes";
        return;
    }
    // The dist vector contain an extra 0 (dist between same node was calculated once)
    nth_element(distance.begin(), distance.begin() + k, distance.end(), compare); // will place the (k+1)th-> element at its correct position(in sorted array)
    cout << "Following are the k nodes nearest to the given node :" << endl;
    cout << endl;

    for (int i = 0; i < distance.size(); i++)
    {
        // if the distance between the given node and current node is less than kth greatest distance then print that node
        if (distance[i].first <= distance[k].first && id!=distance[i].second.id)
        {
            cout << "----------------------------------------------------" << endl;
            cout << "Distance: " << distance[i].first << endl;
            cout << "ID= " << distance[i].second.id << " "
                 << "Name= " << distance[i].second.name << endl;
            cout << "--------------------------------------------------" << endl;
            cout << endl;
        }
    }
}