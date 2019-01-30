#include <cassert>
#include <utility>
#include <iterator>
#include <vector>

#ifndef DIFF_MYERS_HPP
#define DIFF_MYERS_HPP

namespace diff
{

using IndexT = int16_t;
using PointT = std::pair<IndexT,IndexT>;

template <typename ARR>
using ArrValT = typename std::iterator_traits<
	typename ARR::iterator>::value_type;

// all based on the tutorial outlined in
// https://blog.jcoglan.com/2017/02/12/the-myers-diff-algorithm-part-1/

template <typename ARR>
size_t myers_diff_min_edit (ARR orig, ARR updated)
{
	auto orig_begin = std::begin(orig);
	auto updated_begin = std::begin(updated);
	IndexT n = std::distance(orig_begin, std::end(orig));
	IndexT m = std::distance(updated_begin, std::end(updated));
	IndexT max_edit = n + m;

	IndexT x, y;
	size_t ncost = 2 * max_edit + 1;
	std::vector<IndexT> costs(ncost, 0);
	for (IndexT iedit = 0; iedit < max_edit; ++iedit)
	{
		for (IndexT k = -iedit; k <= iedit; k += 2)
		{
			IndexT prevk = costs[(ncost + k - 1) % ncost];
			IndexT nextk = costs[(ncost + k + 1) % ncost];
			if (k == -iedit || (k != iedit && prevk < nextk))
			{
				x = nextk;
			}
			else
			{
				x = prevk + 1;
			}

			y = x - k;

			while (x < n && y < m &&
				*std::next(orig_begin, x) == *std::next(updated_begin, y))
			{
				++x;
				++y;
			}

			costs[(ncost + k) % ncost] = x;

			if (x >= n && y >= m)
			{
				return iedit;
			}
		}
	}
	return max_edit;
}

template <typename ARR>
std::vector<IndexT> myers_diff_trace (ARR orig, ARR updated)
{
	auto orig_begin = std::begin(orig);
	auto updated_begin = std::begin(updated);
	IndexT n = std::distance(orig_begin, std::end(orig));
	IndexT m = std::distance(updated_begin, std::end(updated));
	IndexT max_edit = n + m;

	IndexT x, y;
	size_t ncost = 2 * max_edit + 1;
	std::vector<IndexT> costs(ncost, 0);
	std::vector<IndexT> trace;
	for (IndexT iedit = 0; iedit <= max_edit; ++iedit)
	{
		trace.insert(trace.end(), costs.begin(), costs.end());
		for (IndexT k = -iedit; k <= iedit; k += 2)
		{
			IndexT prevk = costs[(ncost + k - 1) % ncost];
			IndexT nextk = costs[(ncost + k + 1) % ncost];
			if (k == -iedit || (k != iedit && prevk < nextk))
			{
				x = nextk;
			}
			else
			{
				x = prevk + 1;
			}

			y = x - k;

			while (x < n && y < m &&
				*std::next(orig_begin, x) == *std::next(updated_begin, y))
			{
				++x;
				++y;
			}

			costs[(ncost + k) % ncost] = x;

			if (x >= n && y >= m)
			{
				return trace;
			}
		}
	}
	return trace;
}

template <typename ARR>
std::vector<PointT> myers_diff_backtrace (ARR orig, ARR updated)
{
	IndexT x = std::distance(std::begin(orig), std::end(orig));
	IndexT y = std::distance(std::begin(updated), std::end(updated));
	IndexT ncost = 2 * (x + y) + 1;
	IndexT prev_x, prev_y, prev_k;

	auto traces = myers_diff_trace(orig, updated);

	std::vector<PointT> points;
	IndexT ntraces = traces.size() / ncost;
	for (IndexT iedit = ntraces - 1; iedit >= 0; --iedit)
	{
		IndexT k = x - y;
		if (k == -iedit || (k != iedit &&
			traces[iedit * ncost + (ncost + k - 1) % ncost] <
			traces[iedit * ncost + (ncost + k + 1) % ncost]))
		{
			prev_k = k + 1;
		}
		else
		{
			prev_k = k - 1;
		}

		prev_x = traces[iedit * ncost + (ncost + prev_k) % ncost];
		prev_y = prev_x - prev_k;

		while (x > prev_x && y > prev_y)
		{
			points.push_back({--x, --y});
		}

		if (iedit > 0)
		{
			points.push_back({prev_x, prev_y});
		}

		x = prev_x;
		y = prev_y;
	}
	return points;
}

enum Action {
	EQ = 0,
	ADD,
	DEL
};

template <typename T>
struct Diff
{
	T val_;
	IndexT orig_;
	IndexT updated_;
	Action action_;
};

template <typename ARR>
using DiffArrT = Diff<ArrValT<ARR>>;

template <typename ARR>
std::vector<DiffArrT<ARR>> myers_diff (ARR orig, ARR updated)
{
	std::vector<DiffArrT<ARR>> diffs;
	PointT prev{orig.size(), updated.size()};
	auto points = myers_diff_backtrace(orig, updated);
	for (PointT& next : points)
	{
		if (next.first == prev.first)
		{
			diffs.push_back(DiffArrT<ARR>{
				updated[next.second],
				-1,
				next.second,
				ADD
			});
		}
		else if (next.second == prev.second)
		{
			diffs.push_back(DiffArrT<ARR>{
				orig[next.first],
				next.first,
				-1,
				DEL
			});
		}
		else
		{
			diffs.push_back(DiffArrT<ARR>{
				orig[next.first],
				next.first,
				next.second,
				EQ,
			});
		}
		prev = next;
	}
	return std::vector<DiffArrT<ARR>>(
		diffs.rbegin(), diffs.rend());
}

}

#endif // DIFF_MYERS_HPP
