#ifndef ESTD_TRIE_HPP
#define ESTD_TRIE_HPP

#include <optional>

#include "logs/logs.hpp"

namespace estd
{

template <typename KEY, typename VAL>
struct iTrieNode
{
	using KeyT = KEY;

	using ValT = VAL;

	virtual ~iTrieNode (void) = default;

	virtual size_t nchildren (void) const = 0;

	virtual const iTrieNode<KEY,VAL>* next (const KEY& k) const = 0;

	virtual iTrieNode<KEY,VAL>* next (const KEY& k) = 0;

	virtual iTrieNode<KEY,VAL>* add (const KEY& k, iTrieNode<KEY,VAL>* node) = 0;

	virtual iTrieNode<KEY,VAL>* rm (const KEY& k) = 0;

	void clear (void)
	{
		clear_impl();
		leaf_.reset();
	}

	std::optional<VAL> leaf_;

protected:
	virtual void clear_impl (void) = 0;
};

template <typename ARR>
using ArrValT = typename std::iterator_traits<
	typename ARR::iterator>::value_type;

template <typename VECKEY, typename NODE,
	typename std::enable_if<std::is_same<ArrValT<VECKEY>,
	typename NODE::KeyT>::value>::type* = nullptr> // todo: replace w/ c++2a concepts
struct Trie
{
	using KeyT = typename NODE::KeyT;

	using ValT = typename NODE::ValT;

	using NodeT = NODE;

	virtual ~Trie (void) = default;

	// assert keys only consists of lower-case characters
	void add (const VECKEY& keys, ValT val)
	{
		auto hit = keys.begin();
		auto het = keys.end();
		InternalNodeT* it = prefix_find(hit, het);
		InternalNodeT* next_it;
		if (nullptr == it)
		{
			it = root_.get();
		}
		else
		{
			for (; hit != het && nullptr != it; ++hit)
			{
				next_it = it->next(*hit);
				if (nullptr == next_it)
				{
					next_it = it->add(*hit, new NODE());
				}
				it = next_it;
			}
		}
		it->leaf_ = val;
	}

	ValT& emplace (const VECKEY& keys, ValT val)
	{
		auto hit = keys.begin();
		auto het = keys.end();
		InternalNodeT* it = prefix_find(hit, het);
		InternalNodeT* next_it;
		if (nullptr == it)
		{
			it = root_.get();
		}
		else
		{
			for (; hit != het && nullptr != it; ++hit)
			{
				next_it = it->next(*hit);
				if (nullptr == next_it)
				{
					next_it = it->add(*hit, new NODE());
				}
				it = next_it;
			}
		}
		auto lit = it;
		if (false == lit->leaf_.has_value())
		{
			lit->leaf_ = val;
		}
		return *(lit->leaf_);
	}

	void rm (const VECKEY& keys)
	{
		if (0 == keys.size())
		{
			return;
		}

		InternalNodeT* it = root_.get();
		InternalNodeT* parent = it;
		const auto* parent_next = &keys[0];
		// traverse to the first node where trie differs from word
		for (const auto& h : keys)
		{
			// move root downward to avoid parents with multiple children
			if (1 < it->nchildren())
			{
				parent = it;
				parent_next = &h;
			}
			it = it->next(h);
			if (nullptr == it)
			{
				return; // not found
			}
		}
		// assert that path from parent to it has no sibling branches
		if (it->nchildren() == 0)
		{
			delete parent->rm(*parent_next);
		}
		else
		{
			it->leaf_.reset();
		}
	}

	ValT& at (const VECKEY& keys)
	{
		auto hit = keys.begin();
		auto het = keys.end();
		auto found = prefix_find(hit, het);
		if (nullptr == found && hit != het &&
			false == found->leaf_.has_value())
		{
			logs::fatalf("key %s not found",
				fmts::to_string(keys.begin(), keys.end()).c_str());
		}
		return *found->leaf_;
	}

	const ValT& at (const VECKEY& keys) const
	{
		auto hit = keys.begin();
		auto het = keys.end();
		auto found = prefix_find(hit, het);
		if (nullptr == found && hit != het &&
			false == found->leaf_.has_value())
		{
			logs::fatalf("key %s not found",
				fmts::to_string(keys.begin(), keys.end()).c_str());
		}
		return *found->leaf_;
	}

	bool contains_exact (const VECKEY& keys) const
	{
		auto hit = keys.begin();
		auto het = keys.end();
		auto found = prefix_find(hit, het);
		return nullptr != found && hit == het &&
			found->leaf_.has_value();
	}

	bool contains_prefix (const VECKEY& prefix) const
	{
		auto hit = prefix.begin();
		auto het = prefix.end();
		auto found = prefix_find(hit, het);
		return nullptr != found && hit == het;
	}

	VECKEY best_prefix (const VECKEY& keys) const
	{
		auto hit = keys.begin();
		prefix_find(hit, keys.end());
		return VECKEY(keys.begin(), hit);
	}

	const NODE* match_prefix (const VECKEY& prefix) const
	{
		auto hit = prefix.begin();
		auto het = prefix.end();
		auto found = prefix_find(hit, het);
		if (nullptr != found && hit == het)
		{
			return static_cast<const NODE*>(found);
		}
		return nullptr;
	}

	NODE* root (void)
	{
		return root_.get();
	}

	const NODE* root (void) const
	{
		return root_.get();
	}

	void clear (void)
	{
		root_->clear();
	}

private:
	using KeyIt = typename VECKEY::const_iterator;

	using InternalNodeT = iTrieNode<KeyT,ValT>;

	InternalNodeT* prefix_find (KeyIt& hash_it, KeyIt hash_et)
	{
		if (hash_it == hash_et)
		{
			return nullptr;
		}
		InternalNodeT* out = root_.get();
		InternalNodeT* next_it;
		// traverse to the first node where trie differs from word
		for (; hash_it != hash_et && (next_it = out->next(*hash_it));
			++hash_it)
		{
			out = next_it;
		}
		return out;
	}

	const InternalNodeT* prefix_find (KeyIt& hash_it, KeyIt hash_et) const
	{
		if (hash_it == hash_et)
		{
			return nullptr;
		}
		const InternalNodeT* out = root_.get();
		const InternalNodeT* next_it;
		// traverse to the first node where trie differs from word
		for (; hash_it != hash_et && (next_it = out->next(*hash_it));
			++hash_it)
		{
			out = next_it;
		}
		return out;
	}

	std::unique_ptr<NODE> root_ = std::make_unique<NODE>();
};

}

#endif // ESTD_TRIE_HPP
