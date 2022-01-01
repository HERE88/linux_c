#include <iostream>
#include <vector>

#include <algorithm>  ////////// std::find()

using namespace std;

int find_not_double(int src[], int len)
{
    vector<int> tmp;
    //vector<int>::iterator it;
    
    for(int i = 0; i < len; ++i)
    {
        auto it = std::find(tmp.begin(), tmp.end(), src[i]);
        if(it == tmp.end())
            tmp.push_back(src[i]);           
        else
            tmp.erase(it);
    }

    for(auto i : tmp)
        cout<<i<<' ';
    cout<<endl;

    return tmp.size();
}

void find_not_double_xor(int src [ ], int len)
{
    int xor_v = 0;
    for(int i = 0; i < len; ++i)
        xor_v ^= src[i];

    cout<<xor_v<<endl;
}

void find_not_double_xor_without_space(int src [ ], int len)
{
    if(len == 1) 
        cout<<src[0]<<endl;
    else
    {
        for(int i = 1; i < len; ++i)
            src[0] ^= src[i];
    }

    cout<<src[0]<<endl;
}

int main()  
{
    int a[]= {1,2,2,3,4,5,3,5};
    int size = find_not_double(a, sizeof(a)/sizeof(a[0]));    
    cout<<"num="<<size<<endl;
    //=====================================================
    
    int b[]= {1,2,2};
    find_not_double_xor(b, sizeof(b)/sizeof(b[0]));    
    //=====================================================
    
    int c[]= {1,2,1};
    find_not_double_xor_without_space(c, sizeof(c)/sizeof(c[0]));
    //=====================================================
    
    return 0;
}
