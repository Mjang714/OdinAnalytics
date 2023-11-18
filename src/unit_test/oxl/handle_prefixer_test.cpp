/**
 * @file unit_test/handle_prefixer_test.cpp
 * @author Derek Huang
 * @brief oxl `HandlePrefixer` unit tests
 * @copyright MIT License
 */

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "oa/testing/gtest.h"
#include "oxl/xl_api/cache_xl_obj.h"
#include "oxl/xl_api/xl_array.h"
#include "oxl/xl_api/xl_dictionary.h"

namespace {

/**
 * Base traits class template for the handle prefix.
 *
 * `type` member holds the type, `value` holds the handle prefix string literal.
 *
 * @tparam T type
 */
template <typename T>
struct HandlePrefixTraits {
    using type = T;
    static inline constexpr const char* value = "Generic_";
};

/**
 * Macro for simplifying creating `HandlePrefixTraits` specializations.
 *
 * @param T Specialization type
 * @param prefix Handle prefix
 */
#define HANDLE_PREFIX_TRAITS(T, prefix) \
    template <> \
    struct HandlePrefixTraits<T> { \
        using type = T; \
        static inline constexpr const char* value = prefix; \
    }

// convenience type aliases
using XlArrayPtr = std::shared_ptr<oxl::xl_api::XlArray>;
using XlDictionaryPtr = std::shared_ptr<oxl::xl_api::XlDictionary>;

// HandlePrefixTraits specializations for the above type aliases
HANDLE_PREFIX_TRAITS(XlArrayPtr, "Array_");
HANDLE_PREFIX_TRAITS(XlDictionaryPtr, "Dictionary_");

/**
 * Base `HandlePrefixer` test fixture class.
 */
class HandlePrefixerTest : public ::testing::Test {};

/**
 * Test fixture template for `HandlePrefixer` testing.
 *
 * @tparam Traits `HandlePrefixTraits` specialization
 */
template <typename Traits>
class HandlePrefixerTypedTest : public HandlePrefixerTest {};

// create typed test suite using given types
using HandlePrefixerTypedTestTypes = ::testing::Types<
    // resolve to specific handle prefixes
    HandlePrefixTraits<XlArrayPtr>,
    HandlePrefixTraits<XlDictionaryPtr>,
    // resolve to generic handle prefix
    HandlePrefixTraits<double>,
    HandlePrefixTraits<std::string>,
    HandlePrefixTraits<unsigned int>,
    HandlePrefixTraits<const char* const>
>;
TYPED_TEST_SUITE(HandlePrefixerTypedTest, HandlePrefixerTypedTestTypes);

/**
 * Check that `HandlePrefixer` returns the right handle prefix based on type.
 */
TYPED_TEST(HandlePrefixerTypedTest, Test)
{
    EXPECT_EQ(
        TypeParam::value,
        oxl::xl_api::HandlePrefixer{}(typename TypeParam::type{})
    );
}

}  // namespace
