//CDA3103-Summer C001
//Coding Assignment 1
//Zachary Gill
//2015/07/03

#include <stdlib.h>
#include <stdio.h>

unsigned int getS(unsigned int);
unsigned int getE(unsigned int);
unsigned int getM(unsigned int);

void printBin(unsigned int n)
{
  for (int i = 31; i >= 0; i--)
    printf("%d", ((n & (1 << i)) >> i));
  printf("\n");
}

int main()
{
  unsigned int f1 = 0x420F0000;
  unsigned int f2 = 0x41A40000;
  unsigned int result;

  printf("Example from homework 5:\nX     = ");
  printBin(f1);
  printf("Y     = ");
  printBin(f2);

  int s1 = getS(f1);
  int s2 = getS(f2);
  int e1 = getE(f1) - 127;
  int e2 = getE(f2) - 127;
  int m1 = getM(f1);
  int m2 = getM(f2);
  m1 = m1 + (1 << 23);
  m2 = m2 + (1 << 23);

  //align binary points
  int exp = e1;
  if (e1 < e2) {
    exp = e2;
    m1 >>= (e2 - e1);
  }
  else if (e1 > e2) {
    exp = e1;
    m2 >>= (e1 - e2);
  }
  
  //add alligned components
  int carry = 0;
  unsigned int sum = 0;
  int msb = 0;
  for (int i = 0; i < 32; i++) {
    int mask = 1;
    mask <<= i;
    int f1b = m1 & mask;
    int f2b = m2 & mask;
    f1b >>= i;
    f2b >>= i;

    int tmpSum = f1b + f2b + carry;
    switch (tmpSum) {
      case 0:
        sum >>= 1;
        msb--;
        break;
      case 1:
        sum >>= 1;
        sum += (1 << 31);
        msb = 31;
        carry = 0;
        break;
      case 2:
        sum >>= 1;
        msb--;
        carry = 1;
        break;
      default:
        carry += tmpSum / 2;
        sum >>= 1;
        msb--;
        if (tmpSum % 2) {
          sum += (1 << 31);
          msb = 31;
        }
    }
  }

  //normalize sum
  exp += 23 - msb;
  exp += 127;
  if (exp < 0 || exp > 256)
    return 1; //overflow

  while ((sum >> 31) == 0)
    sum <<= 1;
  sum <<= 1;
  sum >>= 9;
  
  result = sum;
  result += ((s1 == 1 && s2 == 1) << 31);
  result += (exp << 23);

  printf("X + Y = ");
  printBin(result);
  
  return 0;
}

unsigned int getS(unsigned int f)
{
  return f >> 31;
}

unsigned int getE(unsigned int f)
{
  unsigned int mask = 0x7f800000; //0 11111111 00000000000000000000000
  unsigned int tmp = f & mask;
  return tmp >> 23;
}

unsigned int getM(unsigned int f)
{
  unsigned int mask = 0x007fffff; //0 00000000 11111111111111111111111
  return f & mask;
}