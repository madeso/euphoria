#pragma once


namespace euphoria::core
{


struct StringBuilder
{
    // non-null terminated string, null-teminated if "completed"
    std::vector<char> data;

    // has add_ been called without a clear?
    [[nodiscard]]
    bool has_content() const;

    void clear();

    StringBuilder& add_char(char c);
    StringBuilder& add_string(const std::string& str);
    StringBuilder& add_view(const std::string_view& str);

    /** Complete the builder and return the resulting string.
     * No strings can be added after this operation.
    */
    std::string to_string();
};


}

