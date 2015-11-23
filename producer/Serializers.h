#pragma once
#include "SimpleSerializer.h"

template<class T>
class Serializers {
	T serializer;
public:
	ISerializer *currentSerializer() { return &serializer; }
};