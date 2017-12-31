#ifndef CORE_LEVENSHTEIN_H
#define CORE_LEVENSHTEIN_H

#include <string>

// Levenshtein Distance Algorithm: C++ Implementation
// by Anders Sewerin Johansen
// http://www.merriampark.com/ldcpp.htm
unsigned long
LevenshteinDistance(const std::string& source, const std::string& target);


// https://www.codeproject.com/Articles/13525/Fast-memory-efficient-Levenshtein-algorithm
unsigned long
FastLevenshteinDistance(const std::string& sRow, const std::string& sCol);


#endif  // CORE_LEVENSHTEIN_H
