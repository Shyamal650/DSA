#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> arr={1,22,4,45,3464,13};
    int pos;
    int element=50;
    cout<<"the arry before insertion : "<<endl;
    for( int i=0; i<arr.size();i++){
        cout<<arr[i]<<" ";
    }
    cout<<"enter which position you want to insert: "<<endl;
    cin>>pos;
    if(pos>=arr.size()){
        cout<<"arrya position out of boundary";
    }
    else{
    arr.insert(arr.begin()+ pos -1, element);
    cout<<"array afetr insertion  : "<<endl;
    for(int i=0;i<arr.size();i+=2)
    {
        cout<<arr[i]<<" ";
    }}
    return 0;
}