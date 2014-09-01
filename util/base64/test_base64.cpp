#include "./base64.h"
#include <iostream>
#include <string>
using namespace std;

int main()
{
  string input_str("123456");  //MTIzNDU2
  string base64_str, output_str;

  cout<<"origin text: \n"<<input_str<<endl;

  Base64Encode(input_str, &base64_str);
  cout<<"encode: \n"<<base64_str<<endl;

  Base64Decode(base64_str, &output_str);
  cout<<"decode: \n"<<output_str<<endl;
  return 0;
}
