# Template Differ (DIFF)

DIFF is a library that compares generic values to extract diff information (action and index changed)

# Current limitations

The myers diff implementation currently assumes that the generic value has an overloaded == operator. (TODO: add comparator)
