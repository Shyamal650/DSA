#include<iostream>
using namespace std;
int main(){
    int n;
    cout<<"enter the value of the number :";
    cin>>n;
    if (n%3==0 and n%5==0){
        cout<<"divisible by both";
    }
    else if(n%3==0 or n%5==0){
        if(n%3==0){
            cout<<"diviisible by three";
        }
        else{
            cout<<"divisible by five";
        }
    }
    else{
        cout<<"divisible by none";
    }
}