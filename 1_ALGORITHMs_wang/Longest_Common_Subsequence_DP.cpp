#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

static int b[1000][1000];
static int c[1000][1000];


void LCS(string &strA, string &strB)
{
    int m = strA.size();
    int n = strB.size();
    
    // Initializing:
    for(int i=0;i<(m+1);++i)
        c[i][0]=0;
    for(int i=1;i<(n+1);++i)
        c[0][i]=0;

    // DP:
    for(int i=1;i<(m+1);++i)
        for(int j=1;j<(n+1);++j)
        {
            if(strA[i-1]==strB[j-1])
            {
                c[i][j]=c[i-1][j-1]+1;
                b[i][j]=1; //upper left
            }
            else if(c[i-1][j]>=c[i][j-1])
            {
                c[i][j]=c[i-1][j];
                b[i][j]=2;  //upper
            }
            else
            {
                c[i][j]=c[i][j-1];
                b[i][j]=3; //left
            }
        }
}

void print(string &str,int m, int n)
{
    if(m==0||n==0) return;

    if(b[m][n]==1){
        print(str, m-1, n-1);
        cout<<str[m-1];
    }
    else if(b[m][n]==2) print(str, m-1, n);
    else print(str, m, n-1);
}

int main()
{
    string strA, strB;

    while(cin>>strA>>strB)
    {
        LCS(strA,strB);
        print(strA,strA.size(),strB.size());
    }
    return 0;
}
