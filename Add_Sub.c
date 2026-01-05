#include <stdio.h>
#include <string.h>

static void set_sign(unsigned *a, int x){
  int b = 1&x;
  b = b << 31;
  *a = *a + b;
}

static float opposite(float x){
  unsigned a;
  memcpy(&a, &x, sizeof(int));
  a ^= 0x80000000;
  float b;
  memcpy(&b,&a,sizeof(int));
  return b;
}

static int absolute(int x){
  int b = (x&0x7FFFFFFF);
  return b;
}

static int sign(int x){
  int b = ((x>>31)&1);
  return b;
}

static int myexp(int x){
  int b = ((x >> 23) & 255) - 127;
  return b;
}

static int mantissa(int x){
  unsigned b = (x & 0x7FFFFF) | 0x800000;
  return b;
}

static unsigned positive_float_add(unsigned a, unsigned b)
{ 
  unsigned a1 = mantissa(a);
  unsigned b1 = mantissa(b);
  unsigned a2 = myexp(a);
  unsigned b2 = myexp(b);
  unsigned bigexp = a2;
  if(a2 < b2){
    a1 = a1 >> (b2 - a2);
	bigexp = b2;
  }
  else
    b1 = b1 >> (a2 - b2);
  unsigned long long ab1 = (unsigned long long)a1+b1;
  int count = 0;
  int p = 0;
  unsigned long long ab2 = ab1;
  while(ab2!=0){
    count++;
	ab2 = ab2 >> 1;
  }
  if(count>24)
    p = p + 1;
  p = p + bigexp;
  p = p + 127;
  unsigned long long new_mantissa = (ab1 >> (count - 24))&0x7FFFFF;
  unsigned final = 0;
  final = final + p;
  final = final << 23;
  final = final + new_mantissa;
  return final;
}

// a must >= b
static unsigned positive_float_sub(unsigned a, unsigned b)
{ 
  unsigned a1 = mantissa(a);
  unsigned b1 = mantissa(b);
  unsigned a2 = myexp(a);
  unsigned b2 = myexp(b);
  unsigned bigexp = a2;
  unsigned smallexp = b2;
  if(a2 < b2){
    a1 = a1 >> (b2 - a2);
	bigexp = b2;
  }
  else
    b1 = b1 >> (a2 - b2);
  unsigned ab1 = a1-b1;
  int count = 0;
  unsigned ab2 = ab1;
  while(ab2!=0){
    count++;
	ab2 = ab2 >> 1;
  }
  unsigned new_mantissa = (ab1 << (24-count)) &0x7FFFFF;
  unsigned p;
  p = count - 24;
  p = p + 127;
  unsigned final = 0;
  final = final + p;
  final = final << 23;
  final = final + new_mantissa;
  return final;
}

float add(float x, float y){
  unsigned a;
  unsigned b;
  memcpy(&a,&x,sizeof(int));
  memcpy(&b,&y,sizeof(int));
  unsigned result;
  unsigned res_sign;
  if(sign(a)!=sign(b)){
    if(absolute(a) < absolute(b)){
	  result = positive_float_sub(absolute(b),absolute(a));
	  res_sign = sign(b);
	}
	else{
	  result = positive_float_sub(absolute(a),absolute(b));
	  res_sign = sign(a);
	}
  }
  else{
    result = positive_float_add(absolute(a),absolute(b));
	res_sign = sign(b);
  }
  set_sign(&result,res_sign);
  float answer;
  memcpy(&answer, &result, sizeof(int));
  return answer;
}


float minus(float a, float b){
  return add(a,opposite(b));
}

