#ifndef JA_HANDLE_H
#define JA_HANDLE_H

#include <utility>

namespace ja {

/**
 * An RAII wrapper around for resource of any kind.
 *
 * Inspired by Standards Committee Paper N4189.
 *
 * @tparam T Type of the resource.
 * @tparam D Type of the deleter.
 */
template<typename T, typename D>
struct unique_resource {
    unique_resource(T resource, D deleter = {})
        :resource_{resource}, deleter_{deleter} {}

    unique_resource(unique_resource&& other) noexcept
        :resource_{std::move(other.resource_)}, deleter_{std::move(other.deleter_)} {
        other.resource_ = T{};
        other.deleter_ = D{};
    }

    unique_resource& operator=(unique_resource&& other) noexcept {
        if (this != &other) {
            swap(other);
        }
        return *this;
    }

    unique_resource(const unique_resource& other) = delete;

    unique_resource& operator=(const unique_resource& other) = delete;

    ~unique_resource() {
        deleter_(resource_);
    }

    /**
     * Swap resources with another instance.
     */
    void swap(unique_resource& other) noexcept {
        std::swap(resource_, other.resource_);
        std::swap(resource_, other.deleter_);
    }

    /**
     * Obtain the underlying resource.
     */
    const T& get() const {
        return resource_;
    }

    /**
     * Obtain the underlying resource.
     */
    const D& get_deleter() const {
        return deleter_;
    }
private:
    T resource_{};
    D deleter_{};
};

}

#endif

