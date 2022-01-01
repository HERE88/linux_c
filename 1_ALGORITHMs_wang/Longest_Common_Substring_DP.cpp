#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

static int c[100][100];

int maxLen;
int row,column; // Do not use if only need max length of LCSSTR.

void LCSSTR(string &strA, string &strB)
{
    int m = strA.size();
    int n = strB.size();

    maxLen = 0;
    
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
                if(c[i][j]>maxLen)
                {
                    maxLen=c[i][j];
                    row=i;
                    column=j;
                }
            }
            else
                c[i][j]=0;
        }
}

void print(string &str,int i, int j)
{
    if(i==0||j==0) return;

    if(c[i][j]){
        print(str, i-1, j-1);
        cout<<str[i-1];
    }
}

int main()
{
    string strA, strB;

    while(cin>>strA>>strB)
    {
        // Output the first substring in shorter str if more than one substring available.
        if(strA.size()<strB.size())
        {
            LCSSTR(strA,strB);
            print(strA,row,column);
        }
        else
        {
            LCSSTR(strB,strA);
            print(strB,row,column);
        }
        cout<<endl;
    }
    return 0;
}
