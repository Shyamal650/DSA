#include<iostream>
using namespace std;
int main(){
    int sp, bp;
    cout<<"enter the sellin proice :"<<endl;
    cin>>sp;
    cout<<"enter the buying price ";
    cin>>bp;
    if(sp>bp){
        cout<<"in the profit"<<endl;

    }
    else if(bp>sp){
        cout<<"in the loss "<<endl;

    }
    else{
        cout<<"no profit no loss";
    }
}