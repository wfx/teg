#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream>
#include <stdexcept>


class Limitrof {
public:
    Limitrof() = default;

    // Initializes the adjacency matrix from an XML document
    void initialize(xmlDocPtr doc);

    // Checks if two countries are neighbors
    bool areNeighbors(int a, int b) const;

    // Sets the neighbor relationship between two countries
    void setNeighbor(int a, int b, bool isNeighbor);

    // Debugging: Prints the matrix to the console
    void printMatrix() const;

private:
    std::vector<std::vector<int>> mat_ady; // Adjacency matrix

    // Parses the XML document and fills the adjacency matrix
    void parseXmlToMatrix(xmlDocPtr doc);
};


void Limitrof::initialize(xmlDocPtr doc) {
    if (!doc) {
        throw std::invalid_argument("Null xmlDocPtr passed to initialize.");
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, (const xmlChar*)"teg_theme")) {
        throw std::runtime_error("Invalid root element in XML.");
    }

    int max_id = -1;

    // Iterate over continents to find max_id
    for (xmlNodePtr continentNode = root->children; continentNode; continentNode = continentNode->next) {
        if (!xmlStrcmp(continentNode->name, (const xmlChar*)"continent")) {
            // Iterate over countries within each continent
            for (xmlNodePtr countryNode = continentNode->children; countryNode; countryNode = countryNode->next) {
                if (!xmlStrcmp(countryNode->name, (const xmlChar*)"country")) {
                    xmlChar* id_prop = xmlGetProp(countryNode, (const xmlChar*)"id");
                    if (id_prop) {
                        int id = std::stoi((char*)id_prop);
                        max_id = std::max(max_id, id);
                        xmlFree(id_prop);
                    }
                }
            }
        }
    }

    // Initialize the adjacency matrix
    mat_ady.resize(max_id + 1, std::vector<int>(max_id + 1, 0));

    // Populate the matrix with neighbor data
    for (xmlNodePtr continentNode = root->children; continentNode; continentNode = continentNode->next) {
        if (!xmlStrcmp(continentNode->name, (const xmlChar*)"continent")) {
            for (xmlNodePtr countryNode = continentNode->children; countryNode; countryNode = countryNode->next) {
                if (!xmlStrcmp(countryNode->name, (const xmlChar*)"country")) {
                    xmlChar* id_prop = xmlGetProp(countryNode, (const xmlChar*)"id");
                    int country_id = std::stoi((char*)id_prop);
                    xmlFree(id_prop);

                    // Set self-connection
                    mat_ady[country_id][country_id] = 2;

                    xmlNodePtr neighborsNode = countryNode->children;
                    while (neighborsNode) {
                        if (!xmlStrcmp(neighborsNode->name, (const xmlChar*)"neighbors")) {
                            for (xmlNodePtr neighborNode = neighborsNode->children; neighborNode; neighborNode = neighborNode->next) {
                                if (!xmlStrcmp(neighborNode->name, (const xmlChar*)"neighbor")) {
                                    xmlChar* neighbor_id_prop = xmlGetProp(neighborNode, (const xmlChar*)"id");
                                    int neighbor_id = std::stoi((char*)neighbor_id_prop);
                                    xmlFree(neighbor_id_prop);

                                    // Only set the lower triangle of the matrix
                                    if (country_id < neighbor_id) {
                                        mat_ady[country_id][neighbor_id] = 1;
                                    }
                                }
                            }
                        }
                        neighborsNode = neighborsNode->next;
                    }
                }
            }
        }
    }
}

bool Limitrof::areNeighbors(int a, int b) const {
    if (a >= mat_ady.size() || b >= mat_ady.size()) {
        throw std::out_of_range("Index out of matrix bounds.");
    }
    return mat_ady[a][b] == 1;
}

void Limitrof::printMatrix() const {
    if (mat_ady.empty()) {
        std::cerr << "Matrix is empty!\n";
        return;
    }

    std::cout << "Adjacency Matrix:\n";
    for (const auto& row : mat_ady) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}
