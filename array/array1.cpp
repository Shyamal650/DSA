#include<iostream>
using namespace std;
int main()
{
    int arr[] = {1, 2, 3, 4, 5};
    cout<<"size of the array is : "<<sizeof(arr)/sizeof(arr[0])<<endl;
     cout << "Elements of the array are: ";
    for(int i = 0; i < 5; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;
    return 0;
}