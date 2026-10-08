//write a program to count the digits of a given no
#include<iostream>
using namespace std;
int main(){
    int n;
    int count =0;
    cout<<"enter the value of the number :";
    cin>>n;
    while(n!=0){
n=n/10;
count=count+1;
    }
    cout<<count;
}