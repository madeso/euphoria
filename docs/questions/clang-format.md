# clang format questions

Is there a way to tell clang-format to treat template brackets <> and function () brackets the same as scope {} brackets?
...or in other words, it's outputting

```cpp
table<int>
drunken_bishop(
        u64 hash,
        int startx,
        int starty)
{
    return drunken_bishop(
            to_codes(to_bytes(hash), msb_first),
            starty);
}
```

but I want something similar to

```cpp
table<int>
drunken_bishop
(
    u64 hash,
    int startx,
    int starty
)
{
    return drunken_bishop
    (
        to_codes(to_bytes(hash), msb_first),
        starty
    );
}
```
