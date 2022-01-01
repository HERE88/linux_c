#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

int main()
{
    string str[2];
    int low,flag;

    while(cin>>str[0]>>str[1])
    {
        flag = 0;
        low=(str[0].size()<str[1].size())?0:1;
        
        for(int i=str[low].size(); i>0; --i)
        {
            if(flag) break;
            for(int j=0; j<(str[low].size()-i+1);++j)
                if(str[(low+1)%2].find(str[low].substr(j,i))!=string::npos)  //fail
                {
                     cout<<str[low].substr(j,i)<<endl;
                     flag=1;
                     break;
                }
                   
        }
    }
    return 0;
}
