#include <bits/stdc++.h>
#include <vector>
using namespace std;

int main()
{ int target=8;
    vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 34, 2, 4, 5};
    sort(arr.begin() , arr.end());
   
    int left = 0;
    int right = arr.size()-1;

    for(int i = 0;i < arr.size(); i++){
        cout<<arr[i]<<" ";
    }
    cout<<endl;

    while(left <= right){
        int mid = (left + right)/2;
        if(target == arr[mid]){
            cout<<mid;
            break;
        }

        if(target < arr[mid]){
            right = mid-1;
        }
        else left = mid+1;
    }
    return 0;
}