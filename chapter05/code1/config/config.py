#!/usr/bin/env python

import sys

print('#ifndef _CONFIG_CONFIG_H')
print('#define _CONFIG_CONFIG_H')
for line in sys.stdin:
    config = line.strip().split('=')
    print('#define {0}              {1}'.format(config[0], config[1]))
print('#endif')
