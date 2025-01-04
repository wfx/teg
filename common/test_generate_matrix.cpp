/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
 *
 * Author: Wolfgang Morawetz teg.github@mailbox.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <vector>
#include <iostream>
#include <libxml/parser.h>
#include <libxml/tree.h>

// Adjacency matrix (global variable for simplicity)
std::vector<std::vector<int>> mat_ady;

void generateAdjacencyMatrix(const char* filename) {
    xmlDocPtr doc = xmlParseFile(filename);
    if (!doc) {
        std::cerr << "Failed to parse XML file: " << filename << "\n";
        return;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);
    if (!root || xmlStrcmp(root->name, (const xmlChar*)"teg_theme")) {
        std::cerr << "Invalid root element\n";
        xmlFreeDoc(doc);
        return;
    }

    int max_id = -1;

    // Iterate over continents
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

    xmlFreeDoc(doc);
}

// Test function to print the matrix
void printMatrix() {
    if (mat_ady.empty()) {
        std::cerr << "Matrix is empty!\n";
        return;
    }

    std::cout << "Adjacency Matrix:\n";
    for (const auto& row : mat_ady) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

int main() {
    const char* testFile = "/home/mow/Workspace/teg/client/themes/TEG/teg_theme.xml"; // Update with the correct path
    generateAdjacencyMatrix(testFile);
    printMatrix();
    return 0;
}
