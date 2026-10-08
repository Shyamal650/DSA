#include <iostream>
using namespace std;

int main() {
    int n;
    int sum = 0;

    cout << "Enter the value of the number: ";
    cin >> n;

    while (n != 0) {
         
        int digit = n % 10;
        n = n / 10;
        sum= sum+ digit;
       
    }

    cout << sum;

    return 0;
}