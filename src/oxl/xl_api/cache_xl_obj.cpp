#include "cache_xl_obj.h"

#include <cstdint>
#include <format>
#include <map>
#include <mutex>
#include <string>
#include <variant>

namespace oxl::xl_api {

XlCacheObj::XlCacheObj(const CachedObjVar& cached_obj, const std::string& key)
  : cache_name_(key)
{
	auto& map = GetCache();
	std::size_t version = 0;

	std::lock_guard lock{CacheMutex()};
	if (map.contains(key))
	{
		auto [old_cached_obj, stored_version] = UnsafeLoadPair(key);
		version = ++stored_version;
	}

	map[key] = {cached_obj, version};
}

XlCacheObj::CacheMap& XlCacheObj::GetCache()
{
	static CacheMap data_cache_;
	return data_cache_;
}

std::mutex& XlCacheObj::CacheMutex()
{
	static std::mutex mt;
	return mt;
}

bool XlCacheObj::IsHandle(const std::string& handle)
{
	auto key = GetKeyFromHandle(handle);
	std::lock_guard lock{CacheMutex()};
	return GetCache().contains(key);
}

bool XlCacheObj::IsDictionary(const std::string& handle) {
	if (!IsHandle(handle)) {
		return false;
	}

	/*get the cached obj and see if it is dictionary not checking the cache 
	handle str incase I ever change that naming scheme*/
	else {
		std::string key = oxl::xl_api::XlCacheObj::GetKeyFromHandle(handle);
		auto cached_obj = GetVariant(key);
		return std::holds_alternative<std::shared_ptr<XlDictionary>>(cached_obj);
	}
}

CachedObjVar XlCacheObj::GetVariant(const std::string& key)
{
	return LoadPair(key).first;
}

CachePair XlCacheObj::UnsafeLoadPair(const std::string& key)
{
	return GetCache().find(key)->second;
}

CachePair XlCacheObj::LoadPair(const std::string& key)
{
	std::lock_guard lock{CacheMutex()};
	return UnsafeLoadPair(key);
}

std::string XlCacheObj::CacheName() const
{
	std::lock_guard lock{CacheMutex()};
	const auto& [cache_variant, version] = UnsafeLoadPair(cache_name_);
	return std::format("{}:{}", cache_name_, version);
}

std::string XlCacheObj::GetKeyFromHandle(const std::string& handle_str)
{
	return handle_str.substr(0, handle_str.find(":"));
}

std::string XlCacheObj::GenHandleStr(const CachedObjVar& cached_obj)
{
	std::string cell_loc = XLoperObj::CellName();

	auto start = cell_loc.find_first_of("[");
	auto end = cell_loc.find_first_of("]");

	// erase workbook name to get base string
	auto base_str = cell_loc.erase(start, end + 1);
	// return prefix + base string
	return std::visit(HandlePrefixer{}, cached_obj) + base_str;
}

}  // namespace oxl::xl_api
