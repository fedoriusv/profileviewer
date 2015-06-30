#pragma once

#include <stdio.h>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>  
#include <algorithm>
#include <unordered_map>
#include <assert.h>

#include <mutex>

#define USE_LOGGER 1

#ifdef _DEBUG
#   define ASSERT(x) assert(x)
#else
#   define ASSERT(x)
#endif //_DEBUG
