#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> arr={12,3,4,5,5,6,7,8,8,8,99,000,1456};
    int target=8;
    for(int i=0; i<arr.size();i++){
        if(arr[i]==target){
            cout<<"elemrnt found at "<<i;
        }
        
    }
    return -1;
}