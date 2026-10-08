#include<iostream>
using namespace std;
int main(){
    int length,breadth, area,perimeter;
    area=length*breadth;
    perimeter=2*(length+breadth);
    cout<<"enter the value of lenght :"<<endl;
    cin>>length;
    cout<<"enter the value of breadth :"<<endl;
    cin>>breadth;
    if(area>perimeter){
        cout<<"area is bigger :";
    }
    else if(area<perimeter){
        cout<<"perimeter is bigger :";
    }
    else{
        cout<<"both are same :";
    }
}