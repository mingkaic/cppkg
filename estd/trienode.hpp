#ifndef ESTD_TRIENODE_HPP
#define ESTD_TRIENODE_HPP

#include <array>

#include "estd/trie.hpp"

namespace estd
{

template <typename KEY, typename VAL, size_t NKEY, typename HASHER>
struct TrieNode final : public iTrieNode<KEY,VAL>
{
	static HASHER hasher_;

	TrieNode (void)
	{
		std::fill(children_.begin(), children_.end(), nullptr);
	}

	// Node always recursively delete children to avoid memory leak,
	// to avoid invalid deletion, set to change of ownership/deleted child to null
	~TrieNode (void)
	{
		this->clear();
	}

	size_t nchildren (void) const override
	{
		return nchildren_;
	}

	const iTrieNode<KEY,VAL>* next (const KEY& k) const override
	{
		return children_.at(hasher_(k));
	}

	iTrieNode<KEY,VAL>* next (const KEY& k) override
	{
		return children_.at(hasher_(k));
	}

	iTrieNode<KEY,VAL>* add (const KEY& k, iTrieNode<KEY,VAL>* node) override
	{
		size_t i = hasher_(k);
		if (i >= NKEY)
		{
			logs::fatalf("cannot add entry %d, with only keysize %d", i, NKEY);
		}
		++nchildren_;
		return children_[i] = static_cast<TrieNode<KEY,VAL,NKEY,HASHER>*>(node);
	}

	iTrieNode<KEY,VAL>* rm (const KEY& k) override
	{
		size_t i = hasher_(k);
		if (i >= NKEY)
		{
			logs::fatalf("cannot rm entry %d, with only keysize %d", i, NKEY);
		}
		auto out = children_[i];
		if (out)
		{
			children_[i] = nullptr;
			--nchildren_;
		}
		return out;
	}

	std::array<TrieNode<KEY,VAL,NKEY,HASHER>*,NKEY> children_;

	size_t nchildren_;

private:
	void clear_impl (void) override
	{
		for (auto& child : children_)
		{
			if (nullptr != child)
			{
				delete child;
			}
			child = nullptr;
		}
		nchildren_ = 0;
	}
};

template <typename KEY, typename VAL, size_t NKEY, typename HASHER>
HASHER TrieNode<KEY,VAL,NKEY,HASHER>::hasher_;

}

#endif // ESTD_TRIENODE_HPP
