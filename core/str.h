#ifndef CORE_STR_H
#define CORE_STR_H

#include <string>
#include <sstream>

namespace euphoria::core
{
    /** Basic string builder.
 */
    class Str
    {
        public:
        /** Generate a string.
  @see toString()
  @return the generated string.
   */
        operator std::string() const;

        /** Generate a string.
  @see operator std::string()
  @return the generated string.
   */
        std::string
        ToString() const;

        /** Add stuff to the stream.
  @param t object to add to the stream.
  @return this
   */
        template <typename Type>
        Str&
        operator<<(const Type& t)
        {
            stream_ << t;
            return *this;
        }

        private:
        std::ostringstream stream_;
    };

}  // namespace euphoria::core

#endif  // CORE_STR_H
