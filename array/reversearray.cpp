#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> arr={1,2,3,4,5,6,7,8,9};
    int start=0; 
    int cz =arr.size()-1;
     while(start<cz){
        swap(arr[start],arr[end]);
        start++;
        cz--;

     }
for(int i=0; i<arr.size();i++){
    cout<<arr[i]<<" ";
}
}