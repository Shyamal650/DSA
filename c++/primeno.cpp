#include<iostream>
using namespace std;
int main()
{
    int n;
    bool prime;
    
    cout<<"enter the value of n :";
    cin>>n;
for(int i =2; i<n; i++){
    if(n%i==0){
        prime=false;

        break;
        }
        else{
            cout<<"its prime";
        }
    }
}