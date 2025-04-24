#ifndef JA_SCOPE_GUARD_H
#define JA_SCOPE_GUARD_H

#include <functional>
#include <tuple>

namespace ja {

template<typename... Ts>
struct scope_guard {
    scope_guard(Ts... callables) 
        :callables_{callables...} {}

    scope_guard(const scope_guard&) = delete;
    scope_guard& operator=(const scope_guard&) = delete;

    ~scope_guard() {
        std::apply([](Ts... callables) {
            (std::invoke(callables), ...);
        }, callables_);
    }

    std::tuple<Ts...> callables_{};
};

}

#endif

