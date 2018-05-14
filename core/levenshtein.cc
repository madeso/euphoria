#include "core/levenshtein.h"

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "core/assert.h"

unsigned long
LevenshteinDistance(const std::string& source, const std::string& target)
{
  // Step 1

  const unsigned long n = source.length();
  const unsigned long m = target.length();
  if(n == 0)
  {
    return m;
  }
  if(m == 0)
  {
    return n;
  }

  // Good form to declare a TYPEDEF

  typedef std::vector<std::vector<unsigned long>> Tmatrix;

  Tmatrix matrix(n + 1);

  // Size the vectors in the 2.nd dimension. Unfortunately C++ doesn't
  // allow for allocation on declaration of 2.nd dimension of vec of vec

  for(unsigned long i = 0; i <= n; i++)
  {
    matrix[i].resize(m + 1);
  }

  // Step 2

  for(unsigned long i = 0; i <= n; i++)
  {
    matrix[i][0] = i;
  }

  for(unsigned long j = 0; j <= m; j++)
  {
    matrix[0][j] = j;
  }

  // Step 3

  for(unsigned long i = 1; i <= n; i++)
  {
    const char s_i = source[i - 1];

    // Step 4

    for(unsigned long j = 1; j <= m; j++)
    {
      const char t_j = target[j - 1];

      // Step 5

      int cost;
      if(s_i == t_j)
      {
        cost = 0;
      }
      else
      {
        cost = 1;
      }

      // Step 6

      const unsigned long above = matrix[i - 1][j];
      const unsigned long left  = matrix[i][j - 1];
      const unsigned long diag  = matrix[i - 1][j - 1];
      unsigned long cell = std::min(above + 1, std::min(left + 1, diag + cost));

      // Step 6A: Cover transposition, in addition to deletion,
      // insertion and substitution. This step is taken from:
      // Berghel, Hal ; Roach, David : "An Extension of Ukkonen's
      // Enhanced Dynamic Programming ASM Algorithm"
      // (http://www.acm.org/~hlb/publications/asm/asm.html)

      if(i > 2 && j > 2)
      {
        unsigned long trans = matrix[i - 2][j - 2] + 1;
        if(source[i - 2] != t_j)
        {
          trans++;
        }
        if(s_i != target[j - 2])
        {
          trans++;
        }
        if(cell > trans)
        {
          cell = trans;
        }
      }

      matrix[i][j] = cell;
    }
  }

  // Step 7

  return matrix[n][m];
}

unsigned long
FastLevenshteinDistance(const std::string& sRow, const std::string& sCol)
{
  const unsigned long RowLen = sRow.length();
  const unsigned long ColLen = sCol.length();

  if(RowLen == 0)
  {
    return ColLen;
  }

  if(ColLen == 0)
  {
    return RowLen;
  }

  std::unique_ptr<unsigned long[]> v0{new unsigned long[RowLen + 1]};
  std::unique_ptr<unsigned long[]> v1{new unsigned long[RowLen + 1]};

  for(unsigned long RowIdx = 0; RowIdx <= RowLen; RowIdx++)
  {
    v0[RowIdx] = RowIdx;
    v1[RowIdx] = 0;
  }

  for(unsigned long ColIdx = 1; ColIdx <= ColLen; ColIdx++)
  {
    v1[0] = ColIdx;

    for(unsigned long RowIdx = 1; RowIdx <= RowLen; RowIdx++)
    {
      const int cost = sRow[RowIdx - 1] == sCol[ColIdx - 1] ? 0 : 1;

      const unsigned long m_min = v0[RowIdx] + 1;
      const unsigned long b     = v1[RowIdx - 1] + 1;
      const unsigned long c     = v0[RowIdx - 1] + cost;

      v1[RowIdx] = std::min(std::min(m_min, b), c);
    }

    v0.swap(v1);
  }

  return v0[RowLen];
}
