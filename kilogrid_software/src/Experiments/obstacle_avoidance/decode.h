#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cmath>

#define GRADIENT_MASK 63
#define TYPE_MASK 192


int getGradientValue(int a)
{
  int gradient_value = 0;
  gradient_value << std::dec << a;
  gradient_value = (gradient_value & GRADIENT_MASK);
  return gradient_value;
}

int getType(int a)
{
  int type = 0;
  type << std::dec << a;
  type = (type & TYPE_MASK);
  type = type / pow(10, 6);
  return type;
}