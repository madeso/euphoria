#ifndef CORE_NONCOPYABLE_H
#define CORE_NONCOPYABLE_H

#define NONCOPYABLE_CONSTRUCTOR(CLASS) CLASS(const CLASS& other) = delete
#define NONCOPYABLE_ASSIGNMENT(CLASS) void operator=(const CLASS&) = delete
#define NONCOPYABLE_MOVE_CONSTRUCTOR(CLASS) CLASS(CLASS&& other) = delete
#define NONCOPYABLE_MOVE_ASSIGNMENT(CLASS) void operator=(CLASS&&) = delete

#define NONCOPYABLE(CLASS) \
    NONCOPYABLE_CONSTRUCTOR(CLASS); \
    NONCOPYABLE_ASSIGNMENT(CLASS); \
    NONCOPYABLE_MOVE_CONSTRUCTOR(CLASS); \
    NONCOPYABLE_MOVE_ASSIGNMENT(CLASS)


namespace euphoria::core
{
    struct Noncopyable
    {
        Noncopyable() = default;
        ~Noncopyable() = default;

        NONCOPYABLE_CONSTRUCTOR(Noncopyable);
        NONCOPYABLE_ASSIGNMENT(Noncopyable);
        NONCOPYABLE_MOVE_CONSTRUCTOR(Noncopyable);
        NONCOPYABLE_MOVE_ASSIGNMENT(Noncopyable);
    };

}  // namespace euphoria::core

#endif  // CORE_NONCOPYABLE_H
