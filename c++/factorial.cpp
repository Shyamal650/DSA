#include<iostream>
using namespace std;
int main()
{
    int n;
    int factorial=1;
    cout<<"enter the value of n :";
    cin>>n;
    while(n>0){
    factorial=n*factorial;
    n= n-1;
    
    }
    cout<<factorial;
}