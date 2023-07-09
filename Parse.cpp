/*
    Name: Vikas Vijaykumar Bastewad
    Roll No. 20CS10073
    Group: 3
*/
#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include "rapidxml.hpp"
#include <vector>
#include <math.h>
#include <string>
#include <map>
#include <cstdlib>  //// cstdlib is needed for atoi(),stoi() 
using namespace std;
using namespace rapidxml;
xml_document<> doc;
xml_node<> *root_node = NULL;

// function for convert the latitudes and longitudes from degree to radians
long double toRadians(const long double degree)
{
    // cmath library in C++ defines the constant M_PI as the value of pi accurate to 1e-30
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}
// function for calculating the crow fly distance between two nodes using the Haversine Formula
long double distance(long double lat1, long double long1, long double lat2, long double long2)
{
    // Convert the latitudes and longitudes from degree to radians.

    lat1 = toRadians(lat1);
    long1 = toRadians(long1);
    lat2 = toRadians(lat2);
    long2 = toRadians(long2);

    // Haversine Formula
    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);

    ans = 2 * asin(sqrt(ans));

    // Radius of Earth in Kilometers, R = 6371
    long double R = 6371;

    // Calculate the result
    ans = ans * R;

    return ans;
}

int main(void)
{
    cout << "\nParsing the map data (map.osm)....." << endl;
    // Read the sample .osm file
    ifstream theFile("map.osm");
    vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
    buffer.push_back('\0');

    // Parse the buffer
    doc.parse<0>(&buffer[0]);

    // Find out the root node
    root_node = doc.first_node();
    int countNodes = 0;
    int countWays = 0;
    xml_attribute<> *pAttribute;

    vector<string> NodeId;
    vector<string> NodeLatitude;
    vector<string> NodeLongitude;
    vector<string> NameOfPlace;
    // Iterate over the nodes
    for (xml_node<> *node = root_node->first_node("node"); node; node = node->next_sibling("node"))
    {
        countNodes++;
        pAttribute = node->first_attribute("id");
        if (pAttribute != NULL)
            NodeId.push_back(pAttribute->value()); // inserting the value of id in the vector

        pAttribute = node->first_attribute("lat");
        if (pAttribute != NULL)
            NodeLatitude.push_back(pAttribute->value()); // inserting the value of latitude in the vector

        pAttribute = node->first_attribute("lon");
        if (pAttribute != NULL)
            NodeLongitude.push_back(pAttribute->value()); // inserting the value of longitude in the vector

        for (xml_node<> *nodeName = node->first_node("tag"); nodeName; nodeName = nodeName->next_sibling("tag"))
        {
            xml_attribute<> *pTagType = nodeName->first_attribute("k");
            string strValue = pTagType->value();
            xml_attribute<> *pTag = nodeName->first_attribute("v");
            if (strValue == "name")
            {
                NameOfPlace.push_back(pTag->value());
            }
        }
    }
    xml_attribute<> *wayAttribute;
    vector<string> WayId;
    vector<long double> nodeRef;

    // Iterate over the nodes of Ways
    for (xml_node<> *way = root_node->first_node("way"); way; way = way->next_sibling("way"))
    {
        countWays++;
        wayAttribute = way->first_attribute("id");
        if (wayAttribute != NULL)
            WayId.push_back(wayAttribute->value()); // inserting the value of way id in the vector

        for (xml_node<> *wayRef = way->first_node("nd"); wayRef; wayRef = wayRef->next_sibling("nd"))
        {
            xml_attribute<> *wayNode = wayRef->first_attribute("ref");
            if (wayNode != NULL)
                nodeRef.push_back(stold(wayNode->value())); // inserting the value of reference node in the vector
        }
    }

    // Printing the number of Nodes and Ways
    cout << "Number of Nodes= " << countNodes << endl;
    cout << "Number of Ways= " << countWays << endl;

    // Search for a particular place
    cout << "Enter the name of the place for Search\n";
    string Input;
    getline(cin, Input);
    int flag = 0;
    cout << "The search results are:\n";
    for (int i = 0; i < NameOfPlace.size(); i++)
    {
        if (NameOfPlace[i] == Input) // if the input string matches then print it else No result
        {
            cout << NameOfPlace[i];
            flag = 1;
        }
    }
    if (flag == 0)
        cout << "No result!\n";
    cout << endl;

    //************ Question 2 ***************************

    cout << "Enter the id of Node from where you want to find the 'k' closest Nodes\n";
    string id;
    cin >> id;
    cout << "Enter the value of 'k': ";
    int k;
    cin >> k;

    // getting the index of the input id in the vector NodeId
    int getIndex;
    for (int i = 0; i < NodeId.size(); i++)
    {
        if (NodeId[i] == id)
        {
            getIndex = i;
            break;
        }
    }

    int N = countNodes; // storing the value of number of total nodes in the map

    // vectors for storing the type casted value of the string Id, latitude,longitude
    vector<long double> ConvertNodeId;
    vector<long double> ConvertNodeLatitude;
    vector<long double> ConvertNodeLongitude(N);

    // vector for storing the Crow fly distance between the nodes
    vector<long double> CrowFlyDistance(N);

    for (int i = 0; i < NodeId.size(); i++) // converting the string to long double
    {
        ConvertNodeId.push_back(stold(NodeId[i]));
        ConvertNodeLatitude.push_back(stold(NodeLatitude[i]));
        ConvertNodeLongitude[i] = stold(NodeLongitude[i]);
    }

    for (int i = 0; i < N; i++)
    {
        CrowFlyDistance[i] = distance(ConvertNodeLatitude[getIndex], ConvertNodeLongitude[getIndex], ConvertNodeLatitude[i], ConvertNodeLongitude[i]);
    }

    // Vector to store element with respective present index
    vector<pair<long double, int>> vp;
    // Inserting element in pair vector to keep track of previous indexes
    for (int i = 0; i < N; ++i)
    {
        vp.push_back(make_pair(CrowFlyDistance[i], i));
    }
    // Sorting pair vector
    sort(vp.begin(), vp.end());
    cout << "\nThe " << k << " closest nodes to a given node using the crow fly distance are: " << endl;

    // Printing the k closest nodes to a given node
    for (int i = 1; i <= k; i++)
    {
        cout << "\nCrow fly Distance= " << vp[i].first << " km\t"
             << " Id of the node= " << NodeId[vp[i].second] << "\t"
             << " Latitude = " << NodeLatitude[vp[i].second] << "\t"
             << " Longitude = " << NodeLongitude[vp[i].second] << endl;
    }

    //******************* Question 3 ******************************************//

    // map for storing the longitude and latitude corresponding to the node id
    map<long double, pair<long double, long double>> Node;
    for (int i = 0; i < NodeId.size(); i++)
    {
        Node[ConvertNodeId[i]] = {ConvertNodeLatitude[i], ConvertNodeLongitude[i]};
    }

    // Initializing the vector of vectors  to store the node reference corresponding to the ways
    vector<vector<long double>> WayVectorRef;

    for (int i = 0; i < WayId.size(); i++)
    {
        // Vector to store column elements
        vector<long double> v1;
        for (int j = 0; j < nodeRef.size(); i++)
        {
            v1.push_back(nodeRef[i]);
        }
        WayVectorRef.push_back(v1);
    }

    vector<long double> HopDist; // Vector for storing the 1-hop distance between the consecutive nodes
    map<string, pair<long double, long double>> AdjacencyList;
    // map<id of way, pair<id of node, distance b/w nodes>>
    // Adjacency list is indexed with the id of ways and it has a value pair of id of node and distance between
    // two adjacent nodes
    for (int i = 0; i < WayId.size(); i++)
    {

        for (int j = 0; j < WayVectorRef[i].size(); j++)
        {
            long double index1 = WayVectorRef[i][j];
            long double index2 = WayVectorRef[i][j + 1];
            long double ThisDistance = distance(Node[index1].first, Node[index1].second, Node[index2].first, Node[index2].second);
            HopDist.push_back(ThisDistance);
            AdjacencyList[WayId[i]] = {WayVectorRef[i][j], ThisDistance};
        }
    }
    // I was not able to apply shortest path algorithm
}