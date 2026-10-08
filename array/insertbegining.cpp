#include<iostream>
#include<vector>
using namespace std;
int main(){
    vector<int> arr= {1,2,3,4,5,6,7,8,9};
    int element =343;
    cout<<"the element of the arry is : "<<endl;
    int n= arr.size();
    cout<<n<<endl;
    for( int i=0;i<n; i++){
        cout<<arr[i]<<" ";
    }

    arr.insert(arr.begin(),element);
    cout<<"array after insertion"<<endl;
    for(int i=0;i<arr.size();i++){
        cout<<arr[i]<<" ";
    }
    return 0;
}