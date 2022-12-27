#pragma once

namespace euphoria::core
{

// todo(Gustav): replace with StringBuilder2
    
/** Basic string builder.
 */
struct StringBuilder
{
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
    to_string() const;

    /** Add stuff to the stream.
        @param t object to add to the stream.
        @return this
        */
    template <typename Type>
    StringBuilder&
    operator<<(const Type& t)
    {
        stream << t;
        return *this;
    }

private:
    std::ostringstream stream;
};


}

