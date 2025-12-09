#ifndef ODINANALYTICS_HELPERS_LIBRARY_ERROR_UTILS_H_
#define	ODINANALYTICS_HELPERS_LIBRARY_ERROR_UTILS_H_

#include <exception>
#include <sstream>
#include "oa/dllexport.h"

namespace oa::helpers::library_error_utils {
    // user-defined library error
    class OA_HELPERS_API library_error : public std::exception {
    private:
        // helper traits to indicate a type works with stringstream
        template <typename T, typename = void>
        struct can_stream {
            static constexpr bool value = false;
        };

        // true specialization
        template <typename T>
        struct can_stream<T,std::void_t<decltype(std::declval<std::stringstream>() << std::declval<T>())>> {
            static constexpr bool value = true;
        };

    public:
        // tag type to indicate that message is done
        struct end_type {};

        // nested class to hold the stringstream
        class builder {
        public:
            // allow streaming values into stringstream
            template <
                typename T,
                typename = std::enable_if_t<can_stream<T>::value> >
            auto& operator<<(T&& v)
            {
                ss_ << std::forward<T>(v);
                return *this;
            }

            // return new exception using formatted message
            auto operator<<(end_type) const
            {
                // note: in C++20 we can use std::move(ss_).str() for move
                return library_error{ ss_.str() };
            }

        private:
            std::stringstream ss_;
        };

        // start library message formatting
        static builder begin()
        {
            return {};
        }

        // end library message formatting
        static end_type end() noexcept
        {
            return {};
        }

        // ctor
        library_error(std::string msg) noexcept : what_{ std::move(msg) } {}

        // get message
        const char* what() const noexcept override
        {
            return what_.c_str();
        }

    private:
        std::string what_;
    };
}

#endif // ODINANALYTICS_HELPERS_LIBRARY_ERROR_UTILS_H_



