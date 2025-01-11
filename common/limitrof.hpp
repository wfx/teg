/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License.
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
/**
 * @file limitrof.h
 * @brief Definition of the Limitrof class for managing an adjacency matrix.
 */
/*
 * Esta super matriz no se si se justifica. Quizas algun AI la pueda llegar a usar.
   I don't know if this super matrix is justified. Perhaps some AI can use it.
 */

 #ifndef LIMITROF_HPP
 #define LIMITROF_HPP

 #pragma message("Including limitrof.hpp")

 // Rest des Codes

 #endif

#pragma once

#include <vector>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <iostream>
#include <stdexcept>

/**
 * @class Limitrof
 * @brief This class manages an adjacency matrix to determine neighborhood relationships.
 */

class Limitrof {
public:
    Limitrof() = default;

    /**
         * @brief Initializes the adjacency matrix from an XML document.
         * @param doc Pointer to the XML document.
         */
    void initialize(xmlDocPtr doc);

    /**
         * @brief Checks if two elements are neighbors.
         * @param a First index.
         * @param b Second index.
         * @return `true` if they are neighbors, otherwise `false`.
         */
    bool areNeighbors(int a, int b) const;
    static std::vector<std::vector<char>> mat_ady;  ///< Static adjacency matrix.

private:
    /**
     * @brief Prints the adjacency matrix to the console.
     */
    void printMatrix() const;

    /**
         * @brief Fallback initialization of the adjacency matrix.
         * @param mat_ady Reference to the adjacency matrix.
         */
    void initializeFallback(std::vector<std::vector<char>>& mat_ady);
};

/// Global instance of the Limitrof class.
extern Limitrof limitrof;
