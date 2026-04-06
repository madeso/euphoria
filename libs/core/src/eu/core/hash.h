#pragma once

namespace eu::core
{

/** \addtogroup hash Hash-util
 * \brief Helper macros for specializing the std::hash.
 * This group provides a util \ref HashCombiner to simplify the creation of hash functions
 * for user-defined types (using std::hash) and macros for specialization of std::hash for custom types.
 *
 * Example usage:
 * ```
 * HASH_DEF_BEGIN(MyType)
 *     HASH_DEF(member1)
 *     HASH_DEF(member2)
 * HASH_DEF_END()
 * ```
 *  @{
*/


/// A utility for combining hash values of multiple objects into a single hash value.
/// \note
/// Do not use when hashing is critical.
struct HashCombiner
{
	std::size_t result = 17;

	/// Combines the hash of the given object with the current result.
	/// 
	/// src: https://stackoverflow.com/a/17017281/180307
	/// numbers from: https://stackoverflow.com/a/1646913/180307
	/// \note
	/// A word of warning, this is (a variation of) the Berstein hash, and because nobody
	/// knows why it does well in tests it is not advisable when hashing is critical.
	/// See https://github.com/mignon-p/jsw-libs/blob/master/Tutorials/jsw_tut_hashing.md
	template<typename T>
	HashCombiner& combine(const T& t)
	{
		result = result * 31 + std::hash<T>{}(t);
		return *this;
	}
};


/** Begins the definition of a specialization of std::hash for the given TYPE.
 * Should be closed by the \ref HASH_DEF_END macro
 * @param TYPE The user-defined type for which the hash specialization is being created.
 * \hideinlinesource
 */
#define HASH_DEF_BEGIN(TYPE) \
	template<> \
	struct std::hash<TYPE> \
	{ \
		std::size_t operator()(const TYPE& x) const \
		{ \
			return eu::core::HashCombiner \
			{ \
			}

/** Adds the hash of a member variable to the hash combination.
 * \hideinlinesource
 * 
 * This macro should be used within a \ref HASH_DEF_BEGIN and \ref HASH_DEF_END block to combine
 * the hash of a specific member variable of the type.
 * 
 *
 * @param NAME The name of the member variable to include in the hash.
 */
#define HASH_DEF(NAME) .combine(x.NAME)

/** Ends the definition of the std::hash specialization.
 *
 * This macro should be used to close the hash function definition started with
 * \ref HASH_DEF_BEGIN. It returns the final combined hash value.
 * 
 * \hideinlinesource
 */
#define HASH_DEF_END() \
	.result; \
	} \
	} \
	;

/**
 * @}
*/

}  //  namespace klotter

