//
// Created by zhao kai on Fri May 15 2020
//

#ifndef __STR_HELPER_H__
#define __STR_HELPER_H__

#include <string>
#include <vector>
#include <sstream>

namespace cutils {

    class AlphaNum {
    public:
        // No bool ctor -- bools convert to an integral type.
        // A bool ctor would also convert incoming pointers (bletch).

        AlphaNum(int x) // NOLINT(runtime/explicit)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(unsigned int x) // NOLINT(runtime/explicit)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(long x) // NOLINT(*)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(unsigned long x) // NOLINT(*)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(long long x) // NOLINT(*)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(unsigned long long x) // NOLINT(*)
                : piece_(std::to_string(x)) {
        }

        AlphaNum(float f) // NOLINT(runtime/explicit)
                : piece_(std::to_string(f)) {
        }

        AlphaNum(double f) // NOLINT(runtime/explicit)
                : piece_(std::to_string(f)) {
        }

        AlphaNum(const char *c_str) : piece_(c_str) {} // NOLINT(runtime/explicit)
        AlphaNum(std::string pc) : piece_(pc) {}       // NOLINT(runtime/explicit)

        template<typename Allocator>
        AlphaNum( // NOLINT(runtime/explicit)
                const std::basic_string<char, std::char_traits<char>, Allocator> &str)
                : piece_(str) {
        }

        // Use string literals ":" instead of character literals ':'.
        AlphaNum(char c) = delete; // NOLINT(runtime/explicit)

        AlphaNum(const AlphaNum &) = delete;

        AlphaNum &operator=(const AlphaNum &) = delete;

        std::string::size_type size() const { return piece_.size(); }

        const char *data() const { return piece_.data(); }

        std::string Piece() const { return piece_; }

        // vector<bool>::reference and const_reference require special help to
        // convert to `AlphaNum` because it requires two user defined conversions.
        template<
                typename T,
                typename std::enable_if<
                        std::is_class<T>::value &&
                        (std::is_same<T, std::vector<bool>::reference>::value ||
                         std::is_same<T, std::vector<bool>::const_reference>::value)>::type * =
                nullptr>
        AlphaNum(T e) : AlphaNum(static_cast<bool>(e)) {} // NOLINT(runtime/explicit)

    private:
        std::string piece_;
    };

    inline std::string StrCat() { return std::string(); }

    inline std::string StrCat(const AlphaNum &a) {
        return std::string(a.data(), a.size());
    }

    template<typename T=std::string>
    inline std::string StrJoin(T container, std::string seperate) {
        std::stringstream ss_;
        for (const auto &item : container) {
            ss_ << item << seperate;
        }
        std::string ss = ss_.str();
        ss.erase(ss.find_last_of(seperate));
        return ss;
    }

    inline std::string StrCat(const AlphaNum &a, const AlphaNum &b) {
        std::string result = a.Piece() + b.Piece();
        return result;
    }

    inline std::string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c) {
        std::string result = a.Piece() + b.Piece() + c.Piece();
        return result;
    }

    inline std::string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
                              const AlphaNum &d) {
        std::string result = a.Piece() + b.Piece() + c.Piece() + d.Piece();
        return result;
    }

    inline std::string StrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
                              const AlphaNum &d, const AlphaNum &e) {
        std::string result = a.Piece() + b.Piece() + c.Piece() + d.Piece() + e.Piece();
        return result;
    }

    inline std::string StrCat(std::initializer_list<const AlphaNum> tt) {
        std::stringstream result;
        for (const auto &arg:tt) {
            result << arg.Piece();
        }
        return result.str();
    }

} // namespace cutils

#endif // __STR_HELPER_H__
