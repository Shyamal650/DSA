#include <iostream>
#include <vector>
using namespace std;
 
int main()
{
    vector<int> arr = {1, 34, 6, 678, 8, 9, 9, 5, 54, 5234, 2321312, 3};
    int largest = arr[0];
    int second = INT_MIN;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] > largest)
        {
            largest = arr[i];
        }
    }
    cout << largest<<endl;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] > second and arr[i] != largest)
        {
            second = arr[i];
        }
    }  
    stack<int>st ; 
     st.push(7) ; 
    cout<<second;
}