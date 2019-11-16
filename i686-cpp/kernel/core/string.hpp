template <int N> class string : public array<char, N> {
    using array<char, N>::array;

  public:
    using array<char, N>::operator==;
    constexpr bool operator==(const char *p) const {
        for (int a = 0; a < length(); a++) {
            if (p[a] == 0 || p[a] != this->operator[](a))
                return false;
        }
        return p[length()] == 0;
    }
    constexpr bool operator!=(const char *p) const { return !operator==(p); }
    /**
     * Create a string literal from this string object.
     */
    const char *str() const { return this->data_; }

    constexpr auto length() const {
        for (int a = 0; a < N; a++) {
            if (this->operator[](a) == 0)
                return a;
        }
        return N;
    }
    constexpr auto begin() const {
        return dual_iterator<const char>(this->data_);
    }
    constexpr auto end() const {
        return dual_iterator<const char>(this->data_ + length());
    }

    constexpr optional<string<N>> extract_word(int num,
                                               char separator = ' ') const {
        string<N> r;
        int seps = 0, i = 0;
        for (auto a : *this) {
            if (a == separator) {
                seps++;
            } else if (seps == num) {
                r[i++] = a();
                // r[a.index] = a(); // TODO fix
            }
        }
        if (seps < num)
            return {};
        return move(r);
    }
};
