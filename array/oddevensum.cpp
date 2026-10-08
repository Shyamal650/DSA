#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 34, 2, 4, 5};
    int sumodd = 0;
        int sumeven = 0;
    for (int i = 0; i < arr.size(); i++)
    {
        if(i%2 == 0){
            sumeven += arr[i];
        }
        else{
            sumeven += arr[i];
        }
    }
    cout<<abs(sumeven - sumodd);
    
    return 0;
}
