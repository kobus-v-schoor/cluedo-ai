#! /usr/bin/env python3

# This will generate C++ code that can build a graph for the map using the
# contents of map.txt

with open('map.txt', 'r') as f:
    for line in f.readlines():
        line = line.strip()
        if not line:
            print("")
        elif line.startswith('#'):
            nl = line.strip('#')
            nl = '//' * (len(line) - len(nl)) + ' ' + nl.strip()
            print(nl)
        else:
            left, right = line.split()
            print("graph[{}].push_back({});".format(left, right))
            print("graph[{}].push_back({});".format(right, left))
