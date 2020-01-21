#ifndef ESTD_CONFIG_HPP
#define ESTD_CONFIG_HPP

#include <string>
#include <vector>

#include "estd/contain.hpp"

namespace estd
{

struct iConfig
{
	virtual ~iConfig (void) = default;

	virtual std::vector<std::string> get_names (void) const = 0;

	virtual void* get_obj (const std::string& cfg_name) = 0;
};

struct ConfigMap final : public iConfig
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

	std::vector<std::string> get_names (void) const
	{
		std::vector<std::string> names;
		names.reserve(entries_.size());
		std::transform(entries_.begin(), entries_.end(),
			std::back_inserter(names),
			[](const std::pair<std::string,ConfigEntry>& entry)
			{
				return entry.first;
			});
		return names;
	}

	void* get_obj (const std::string& cfg_name)
	{
		if (has(entries_, cfg_name))
		{
			auto& entry = entries_.at(cfg_name);
			return entry.data_;
		}
		logger_.error(fmts::sprintf(
			"failed to find config name %s", cfg_name.c_str()));
		return nullptr;
	}

	template <typename T>
	void add_entry (const std::string& cfg_name)
	{
		entries_.emplace(cfg_name, ConfigEntry{
			new T(),
			[](void* ptr) { delete static_cast<T*>(ptr); },
		});
	}

	template <typename T>
	void add_entry (const std::string& cfg_name,
		std::function<void(T*)> del)
	{
		entries_.emplace(cfg_name, ConfigEntry{
			new T(),
			[del](void* ptr) { del(static_cast<T*>(ptr)); },
		});
	}

	void rm_entry (const std::string& cfg_name)
	{
		if (has(entries_, cfg_name))
		{
			{
				auto& entry = entries_.at(cfg_name);
				if (entry.deletion_)
				{
					entry.deletion_(entry.data_);
				}
			}
			entries_.erase(cfg_name);
		}
	}

	struct ConfigEntry
	{
		void* data_;

		std::function<void(void*)> deletion_;
	};

	std::unordered_map<std::string,ConfigEntry> entries_;
};

}

#endif // ESTD_CONFIG_HPP
