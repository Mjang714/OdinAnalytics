#ifndef OXL_XL_API_CACHE_XL_OBJ_H_
#define OXL_XL_API_CACHE_XL_OBJ_H_

#include <format>
#include <memory>
#include <map>
#include <variant>
#include <string>

#include "xl_array.h"
#include "xl_dictionary.h"
#include "xloper_converter.h"


namespace oxl::xl_api
{
	//use namspace for varaint types that can be stored within the caching component
	using CachedObjVar = std::variant<std::shared_ptr<XlArray>, std::shared_ptr<XlDictionary>>;

	class XlCacheObj
	{
		public:
			XlCacheObj() = default;
			XlCacheObj(const CachedObjVar& cached_obj, const std::string& key);

			static bool IsHandle(const std::string& handle);

			/// <summary>
			/// generated a unique handle for the cached object
			/// </summary>
			/// <param name="cahced_obj"></param>
			/// <returns></returns>
			static std::string GenHandleStr(const CachedObjVar& cahced_obj);

			/// <summary>
			/// retrieves the object from the cache as an std variant
			/// </summary>
			/// <param name="key"></param>
			/// <returns></returns>
			static CachedObjVar GetVariant(const std::string& key );

			/// <summary>
			/// retireves the cached item name and the iteration of the cached object 
			/// </summary>
			/// <returns></returns>
			std::string CacheName() const;

			/// <summary>
			/// Gets the key to retrieve the chaced obj var from the map absed ont eh handle name
			/// </summary>
			/// <param name="handle_str"></param>
			/// <returns></returns>
			static std::string GetKeyFromHandle(const std::string& handle_str);

		private:
			static std::pair<CachedObjVar, int> LoadPair(const std::string& key);
			static std::map < std::string, std::pair<CachedObjVar, int>>& GetCache();
			std::string cache_name_;
	};
}
#endif //OXL_XL_API_CACHE_XL_OBJ_H_
