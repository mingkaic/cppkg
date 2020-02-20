#ifndef ESTD_TRIEBIG_HPP
#define ESTD_TRIEBIG_HPP

#include "estd/contain.hpp"
#include "estd/trie.hpp"

namespace estd
{

template <typename KEY, typename VAL, typename HASHER>
struct TrieBigNode final : public iTrieNode<KEY,VAL>
{
	// Node always recursively delete children to avoid memory leak,
	// to avoid invalid deletion, set to change of ownership/deleted child to null
	~TrieBigNode (void)
	{
		this->clear();
	}

	size_t nchildren (void) const override
	{
		return children_.size();
	}

	const iTrieNode<KEY,VAL>* next (const KEY& k) const override
	{
		return try_get(children_, k, nullptr);
	}

	iTrieNode<KEY,VAL>* next (const KEY& k) override
	{
		return try_get(children_, k, nullptr);
	}

	iTrieNode<KEY,VAL>* add (const KEY& k, iTrieNode<KEY,VAL>* node) override
	{
		children_.emplace(k, static_cast<TrieBigNode<KEY,VAL,HASHER>*>(node));
		return node;
	}

	iTrieNode<KEY,VAL>* rm (const KEY& k) override
	{
		auto val = children_.at(k);
		children_.erase(k);
		return val;
	}

	std::unordered_map<KEY,TrieBigNode<KEY,VAL,HASHER>*,HASHER> children_;

private:
	void clear_impl (void) override
	{
		for (auto cpair : children_)
		{
			delete cpair.second;
		}
		children_.clear();
	}
};

}

#endif // ESTD_TRIEBIG_HPP
