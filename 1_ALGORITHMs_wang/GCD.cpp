#include <iostream>
#include <vector>

using namespace std;

int gcd(int m, int n)
{
    return (!n) ? m : gcd(n,m%n);
}

int gcd_non_recur(int m, int n)
{
    int tmp;
    while(n)
    {
        tmp = m%n; //discipline：the number after % equals to 0 !
        m=n;
        n=tmp;
    }
    return m;
}

int main()  {
    int a,b;

    while(cin>>a>>b)
    {
        cout<<"gcd="<<gcd(a,b)<<','<<gcd_non_recur(a,b)<<endl;
    }
    
    return 0;
}
