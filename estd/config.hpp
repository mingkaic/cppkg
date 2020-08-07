#ifndef ESTD_CONFIG_HPP
#define ESTD_CONFIG_HPP

#include <string>
#include <vector>
#include <functional>

#include "estd/contain.hpp"

namespace estd
{

template <typename K=std::string, typename HASH=std::hash<K>>
struct iConfig
{
	virtual ~iConfig (void) = default;

	virtual std::vector<K> get_keys (void) const = 0;

	virtual void* get_obj (const K& cfg_key) = 0;
};

template <typename K=std::string, typename HASH=std::hash<K>>
struct ConfigMap final : public iConfig<K,HASH>
{
	~ConfigMap (void)
	{
		for (auto& epairs : entries_)
		{
			auto& entry = epairs.second;
			if (entry.deletion_)
			{
				entry.deletion_(entry.data_);
			}
		}
	}

	std::vector<K> get_keys (void) const override
	{
		std::vector<K> names;
		names.reserve(entries_.size());
		std::transform(entries_.begin(), entries_.end(),
			std::back_inserter(names),
			[](const std::pair<K,ConfigEntry>& entry)
			{
				return entry.first;
			});
		return names;
	}

	void* get_obj (const K& cfg_key) override
	{
		if (has(entries_, cfg_key))
		{
			auto& entry = entries_.at(cfg_key);
			return entry.data_;
		}
		logs::errorf("failed to find config name %s", cfg_key.c_str());
		return nullptr;
	}

	template <typename T>
	void add_entry (const K& cfg_key)
	{
		if (false == estd::has(entries_, cfg_key))
		{
			entries_.emplace(cfg_key, ConfigEntry{
				new T(),
				[](void* ptr) { delete static_cast<T*>(ptr); },
			});
		}
	}

	template <typename T>
	void add_entry (const K& cfg_key, std::function<void(T*)> del)
	{
		if (false == estd::has(entries_, cfg_key))
		{
			entries_.emplace(cfg_key, ConfigEntry{
				new T(),
				[del](void* ptr) { del(static_cast<T*>(ptr)); },
			});
		}
	}

	void rm_entry (const K& cfg_key)
	{
		if (has(entries_, cfg_key))
		{
			{
				auto& entry = entries_.at(cfg_key);
				if (entry.deletion_)
				{
					entry.deletion_(entry.data_);
				}
			}
			entries_.erase(cfg_key);
		}
	}

	struct ConfigEntry
	{
		void* data_;

		std::function<void(void*)> deletion_;
	};

	std::unordered_map<K,ConfigEntry,HASH> entries_;
};

}

#endif // ESTD_CONFIG_HPP
