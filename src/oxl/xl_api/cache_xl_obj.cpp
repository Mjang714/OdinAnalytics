#include "cache_xl_obj.h"

namespace oxl::xl_api
{	
	XlCacheObj::XlCacheObj(const CachedObjVar& cached_obj, const std::string& key) : cache_name_(key)
	{
		auto &map = GetCache();
		int version = 0;

		if (map.contains(key))
		{
			auto [old_cached_obj, stored_version] = LoadPair(key);
			version = ++stored_version;
		}

		map[key] = std::pair<CachedObjVar, int> (cached_obj, version);
	}

	std::map < std::string, std::pair<CachedObjVar, int>>& XlCacheObj::GetCache()
	{
		static std::map<std::string, std::pair<CachedObjVar, int>> data_cache_;
		return data_cache_;
	}

	bool XlCacheObj::IsHandle(const std::string& handle)
	{
		const auto& map = XlCacheObj::GetCache();
		std::string key = GetKeyFromHandle(handle);

		if (map.contains(key))
		{
			return true;
		}

		return false;
	}

	CachedObjVar XlCacheObj::GetVariant(const std::string& key)
	{
		return LoadPair(key).first;
	}

	std::pair<CachedObjVar, int> XlCacheObj::LoadPair(const std::string& key)
	{
		return GetCache().find(key)->second;
	}

	std::string XlCacheObj::CacheName() const
	{
		const auto& [cache_variant, version] = LoadPair(cache_name_);

		std::string handle_name = std::format("{}:{}", cache_name_, version);

		return handle_name;
	}

	std::string XlCacheObj::GetKeyFromHandle(const std::string& handle_str)
	{
		size_t colon_index = handle_str.find(":");
		return handle_str.substr(0, colon_index);
	}

	std::string XlCacheObj::GenHandleStr(const CachedObjVar& cached_obj)
	{

		std::string cell_loc = XLoperObj::CellName();

		auto start = cell_loc.find_first_of("[");
		auto end = cell_loc.find_first_of("]");

		//erase workbook name
		std::string base_str = cell_loc.erase(start, end + 1);
		std::string prefix = "";
		if (std::holds_alternative<std::shared_ptr<XlArray>>(cached_obj))
		{
			prefix = "Array_";
		}

		else if (std::holds_alternative<std::shared_ptr<XlDictionary>>(cached_obj))
		{
			prefix = "Dictionary_";
		}

		else
		{
			prefix = "Generic_";
		}

		return prefix + base_str;
	}
}