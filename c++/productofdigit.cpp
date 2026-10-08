#include<iostream>
using namespace std;
int main()
{
    int n;
    int product=1;
    cout<<"enter the value of n :";
    cin>>n;
    while(n!=0){
        int digit=n%10;
        n=n/10;
        product=digit*product;
    }
    cout<<product;
}