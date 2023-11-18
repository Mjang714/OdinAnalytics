#ifndef OXL_XL_API_CACHE_XL_OBJ_H_
#define OXL_XL_API_CACHE_XL_OBJ_H_

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <variant>

#include "xl_array.h"
#include "xl_dictionary.h"
#include "xloper_converter.h"

namespace oxl::xl_api {

// type alias for variant storing within cache
// TODO: consider using unique_ptr to allow the cache to uniquely own the
// object, unless there is a compelling case for shared ownership
using CachedObjVar = std::variant<
	std::shared_ptr<XlArray>, std::shared_ptr<XlDictionary>
>;
using CachePair = std::pair<CachedObjVar, std::size_t>;

/**
 * Functor used to generate the cached object key prefix based on type.
 *
 * Can be used with `std::visit` via its `operator()` overloads.
 */
struct HandlePrefixer {
	std::string operator()(const std::shared_ptr<XlArray>& /* val */) const
	{
		return "Array_";
	}

	std::string operator()(const std::shared_ptr<XlDictionary>& /* val */) const
	{
		return "Dictionary_";
	}

	template <typename T>
	std::string operator()(const T& /* val */) const
	{
		return "Generic_";
	}
};

class XlCacheObj {
public:
	/**
	 * Ctor.
	 *
	 * Inserts an object into the cache with an associated key name.
	 */
	XlCacheObj(const CachedObjVar& cached_obj, const std::string& key);

	/**
	 * Retrieve the cache handle associated with the object.
	 *
	 * Contains the key name and the unique hit count of the cached object.
	 */
	std::string CacheName() const;

	/**
	* checks to see if a handle given is valid dicitonary input
	*/
	static bool IsDictionary(const std::string& handle);

	/**
	 * Return `true` if the string is a handle, `false` otherwise.
	 */
	static bool IsHandle(const std::string& handle);

	/**
	 * Generate a unique handle for the cache object.
	 */
	static std::string GenHandleStr(const CachedObjVar& cached_obj);

	/**
	 * Retrieve the object variant from the cache given the object key.
	 */
	static CachedObjVar GetVariant(const std::string& key);

	/**
	 * Return the key assocaited with the object based on the handle name.
	 */
	static std::string GetKeyFromHandle(const std::string& handle_str);

private:
	std::string cache_name_;

	using CacheMap = std::map<std::string, CachePair>;

	/**
	 * Load the cache object pair without locking.
	 *
	 * @note Since no locking is done, this function is not thread-safe.
	 */
	static CachePair UnsafeLoadPair(const std::string& key);

	/**
	 * Load the cache object pair.
	 */
	static CachePair LoadPair(const std::string& key);

	/**
	 * Return a reference to the object cache.
	 *
	 * @note Locking of `CacheMutex()` must be done for any guarantee of
	 * 	consistent state observation in the presence of multiple threads.
	 */
	static CacheMap& GetCache();

	/**
	 * Return a reference to the object cache's mutex.
	 *
	 * Must be locked in a multi-threaded context to ensure state consistency.
	 */
	static std::mutex& CacheMutex();
};

}  // namespace oxl::xl_api

#endif  //OXL_XL_API_CACHE_XL_OBJ_H_
