#ifndef JA_ANGLE_H
#define JA_ANGLE_H

#include <concepts>
#include <format>
#include <numbers>
#include <sstream>

namespace ja {

/**
 * For representing angles.
 */
template<std::floating_point T = float>
struct angle {
    constexpr angle() = default;

    /**
     * Obtain angle in radians.
     */
    [[nodiscard]] constexpr T radians() const {
        return radians_;
    }

    /**
     * Obtain angle in degrees.
     */
    [[nodiscard]] constexpr T degrees() const {
        return radians_ * (static_cast<T>(180.0) / std::numbers::pi_v<T>);
    }
private:
    constexpr explicit angle(T radians)
        :radians_{radians} {}

    template<std::floating_point U>
    friend constexpr angle<U> radians(U angle);

    template<std::floating_point U>
    friend constexpr angle<U> degrees(U angle);

    T radians_{};
};

/**
 * Create angle in radians.
 */
template<std::floating_point T = float>
[[nodiscard]] constexpr angle<T> radians(T angle) {
    return ja::angle<T>{angle};
}

/**
 * Create angle in degrees.
 */
template<std::floating_point T = float>
[[nodiscard]] constexpr angle<T> degrees(T angle) {
    return ja::angle<T>{angle * (std::numbers::pi_v<T> / static_cast<T>(180.0))};
}

}

namespace ja::literals {

[[nodiscard]] constexpr angle<float> operator""_rad(long double angle) {
    return radians<float>(angle);
}

[[nodiscard]] constexpr angle<float> operator""_deg(long double angle) {
    return degrees<float>(angle);
}

}

template<std::floating_point T>
struct std::formatter<ja::angle<T>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end())
            return it;
 
        switch (*it) {
            case 'r':
                in_radians_ = true;
                ++it;
                break;
            case 'd':
                in_radians_ = false;
                ++it;
                break;
        }

        if (it != ctx.end() && *it != '}') {
            throw std::format_error{"invalid format specifier. use 'd' for degrees and 'r' for radians"};
        }
 
        return it;
    }

    auto format(const ja::angle<T>& angle, std::format_context& ctx) const {
        std::ostringstream oss{};
        if (in_radians_) {
            oss << angle.radians() << " rad";
        } else {
            oss << angle.degrees() << "°";
        }
        return std::ranges::copy(std::move(oss).str(), ctx.out()).out;
    }

    bool in_radians_{};
};

#endif

