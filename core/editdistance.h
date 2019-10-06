#ifndef CORE_EDITDISTANCE_H
#define CORE_EDITDISTANCE_H

#include <string>

namespace euphoria::core
{
    // Levenshtein Distance Algorithm: C++ Implementation
    // by Anders Sewerin Johansen
    // http://www.merriampark.com/ldcpp.htm
    unsigned long
    EditDistance(const std::string& source, const std::string& target);


    // https://www.codeproject.com/Articles/13525/Fast-memory-efficient-Levenshtein-algorithm
    // licensed under: The Code Project Open License (CPOL) 1.02
    unsigned long
    FastEditDistance(const std::string& the_row, const std::string& the_col);

}  // namespace euphoria::core

#endif  // CORE_EDITDISTANCE_H
