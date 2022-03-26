#pragma once

/** Random number engine based on Xoshiro256** by David Blackman and Sebastiano Vigna. */
class Xoshiro256ss
{
    public:
        using result_type = unsigned long long;

        constexpr explicit Xoshiro256ss(result_type seed) noexcept
            : m_state{SplitMix(&seed), SplitMix(&seed), SplitMix(&seed), SplitMix(&seed)}
        {
        }

        constexpr auto Fork() noexcept -> Xoshiro256ss
        {
            return Xoshiro256ss{this->operator()()};
        }

        constexpr auto operator()() noexcept -> result_type
        {
            result_type result = Rotate(m_state[1] * 5, 7) * 9;
            result_type t = m_state[1] << 17;
            m_state[2] ^= m_state[0];
            m_state[3] ^= m_state[1];
            m_state[1] ^= m_state[2];
            m_state[0] ^= m_state[3];
            m_state[2] ^= t;
            m_state[3] = Rotate(m_state[3], 45);
            return result;
        }

        constexpr friend bool operator==(const Xoshiro256ss& lhs, const Xoshiro256ss& rhs) noexcept
        {
            return lhs.m_state[0] == rhs.m_state[0] && lhs.m_state[1] == rhs.m_state[1] &&
                   lhs.m_state[2] == rhs.m_state[2] && lhs.m_state[3] == rhs.m_state[3];
        }

        constexpr friend bool operator!=(const Xoshiro256ss& lhs, const Xoshiro256ss& rhs) noexcept
        {
            return !(lhs == rhs);
        }

        static constexpr auto min() noexcept { return result_type(0);  }
        static constexpr auto max() noexcept { return result_type(-1); }

    private:
        result_type m_state[4];

        constexpr auto SplitMix(result_type* x) noexcept -> result_type
        {
            result_type z = (*x += 0x9e3779b97f4a7c15uLL);
            z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9uLL;
            z = (z ^ (z >> 27)) * 0x94d049bb133111ebuLL;
            return z ^ (z >> 31);
        }

        constexpr auto Rotate(result_type x, int k) noexcept -> result_type
        {
            return (x << k) | (x >> (64 - k));
        }
};