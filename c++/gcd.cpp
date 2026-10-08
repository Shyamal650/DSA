#include<iostream>
using namespace std;
int main()
{int lcm;
    int n1=100;
    int n2=2;
    int a=n1;
    int b=n2;
    while(b!=0)
{
    int rem= a%b;
    a=b;
    b=rem;
}
cout<<n1<<endl;
cout<<(n1*n2)/a;
}