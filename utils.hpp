#pragma once
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>

inline constexpr const size_t FNV_PRIME_32 = 16777619;
inline constexpr const size_t FNV_OFFSET_32 = 2166136261U;
template <typename Collection, size_t len>
class Span
{
    Collection * collection;
    size_t start;
    public:
    //Span(Collection& coll): collection(&coll), start(0) {}
    Span(Collection& coll, size_t start): collection(&coll), start(start) {
    }
    Span(const Span<Collection, len>& o): collection(o.collection), start(o.start)
    {
    }
    Span(const Span<Collection, len>&& o): collection(o.collection), start(o.start)
    {
    }
    auto& operator[](size_t index) noexcept
    {
        return (*collection)[start + index];
    }
    const auto& get(size_t index) const noexcept
    {
        return (*collection)[start + index];
    }
    size_t size() const noexcept
    {
        return len;
    }
};
struct LexToken {
    size_t index = 0;
    size_t priority = 0;
    std::string_view attribute;
    std::string_view text;
    LexToken(size_t index, size_t priority, std::string_view attribute, std::string_view text): priority(priority), attribute(attribute), text(text) {}
    friend std::ostream& operator<<(std::ostream& o, const LexToken& tok)
    {
        o<<"<"<<tok.attribute<<":"<<tok.priority<<"->"<<tok.text<<">";
        return o;
    }
};
struct SimpleLexToken {
    size_t index = 0;
    size_t priority = 0;
    std::string_view text;
    SimpleLexToken(size_t index, size_t priority, std::string_view text): priority(priority), text(text) {}
    friend std::ostream& operator<<(std::ostream& o, const SimpleLexToken& tok)
    {
        o<<"<"<<tok.priority<<"->"<<tok.text<<">";
        return o;
    }
};
inline std::string read_file(std::string filePath)
{
  std::fstream file = std::fstream(filePath);
  std::string val((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  return val;
}