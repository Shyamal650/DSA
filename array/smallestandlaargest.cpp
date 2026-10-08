#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> arr={-1,100,102,1,2,3,4,57,7,8,};
    int smallest=INT_MAX;
    int largest=INT_MIN;
    for(int i=0; i<arr.size();i++){
        if(arr[i]<smallest){
            smallest=arr[i];

        }
    }
  for(int i=0; i<arr.size();i++){
        if(arr[i]>largest){
            largest=arr[i];

        }
    }  
cout<<"smallest value : "<<smallest<<endl;
cout<<"largest elment value : "<<largest;
}