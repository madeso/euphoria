#pragma once


namespace eu
{
    // todo(Gustav): replace with one single algorithm

    // Levenshtein Distance Algorithm: C++ Implementation
    // by Anders Sewerin Johansen
    // http://www.merriampark.com/ldcpp.htm
    int
    calc_edit_distance(const std::string& source, const std::string& target);


    // https://www.codeproject.com/Articles/13525/Fast-memory-efficient-Levenshtein-algorithm
    // licensed under: The Code Project Open License (CPOL) 1.02
    int
    calc_edit_distance_fast(const std::string& the_row, const std::string& the_col);
}
