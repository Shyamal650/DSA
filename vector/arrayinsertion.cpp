#include <bits/stdc++.h>
using namespace std;

int main() {
    int arr[] = {1, 2, 3, 4, 56, 7, 8, 9, 10};

    int n = sizeof(arr) / sizeof(arr[0]);

    for (int i = 0; i < n; i++) {
        cout << arr[i] << " "; asbbad
    }

    arr[3] = 55;
    cout << "\nArray after modification: ";
    for (int i = 0; i < n; i++) {
        cout << arr[i] << " ";
    }

    return 0;
}