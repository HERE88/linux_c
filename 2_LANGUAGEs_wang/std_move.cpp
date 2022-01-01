#include <iostream>
#include <vector>

#include <string>
#include <algorithm>

using namespace std;

class MEMORY
{
    public:
        explicit MEMORY(int);
        ~MEMORY();
        
        MEMORY(MEMORY &);   // copy constructor
        MEMORY& operator=(const MEMORY &);   // copy assignment
        
        MEMORY(MEMORY &&);   // move constructor
        MEMORY& operator=(MEMORY &&);   // move assignment


        void print(void) const;

    public:
        size_t size;
        char *buf;
};

MEMORY::MEMORY(int sz=512): size(sz), buf(new char [size])
{   

}

MEMORY::~MEMORY()
{
    cout<<"deconstructor called!"<<endl;
    delete [] buf;

}

MEMORY::MEMORY(MEMORY & s) : size(s.size)
{
    cout<<"copy constructor called!"<<endl;
        
    buf=new char [size];
    std::copy(s.buf, s.buf+s.size, buf);
}

MEMORY& MEMORY::operator=(const MEMORY & s)
{
    cout<<"copy assignment called!"<<endl;
    
    size=s.size;
    buf=new char [size];
    std::copy(s.buf, s.buf+s.size, buf);
 
    return *this;
}

MEMORY::MEMORY(MEMORY && s) : size(s.size), buf(s.buf)
{
    cout<<"move constructor called!"<<endl;
    
    s.size=0;
    s.buf=nullptr;
}

MEMORY& MEMORY::operator=(MEMORY && s)
{
    cout<<"move assignment called!"<<endl;
    if(this != &s)
    {
        size=s.size;
        buf=s.buf;

        s.size=0;
        s.buf=nullptr;
    }

    return *this;
}

void MEMORY::print(void) const
{
    cout<<"print size:"<<size<<endl;
}


int main() 
{
    MEMORY tmp(1024);
    tmp.print();  // 1024
    tmp.buf[0]='w';
    cout<<"tmp.buf[0]="<<tmp.buf[0]<<endl;
    cout<<"========================================="<<endl;
    
    MEMORY tmp2(tmp);
    tmp2.print();  // 1024
    cout<<"tmp2.buf[0]="<<tmp2.buf[0]<<endl;
    cout<<"========================================="<<endl;

    MEMORY tmp3;
    tmp3=tmp;
    tmp3.print();  // 1024
    cout<<"tmp3.buf[0]="<<tmp3.buf[0]<<endl;
    cout<<"========================================="<<endl;

    MEMORY tmp4(std::move(tmp));
    tmp4.print();
    cout<<"tmp4.buf[0]="<<tmp4.buf[0]<<endl;
    tmp.print();
    cout<<"========================================="<<endl;

    MEMORY tmp5=std::move(tmp4); // Note that is move constructor, not move assignment! 
    tmp5.print();
    cout<<"tmp5.buf[0]="<<tmp5.buf[0]<<endl;
    tmp4.print();
    cout<<"========================================="<<endl;

    MEMORY tmp6;
    tmp6=std::move(tmp5);
    tmp6.print();
    cout<<"tmp6.buf[0]="<<tmp6.buf[0]<<endl;
    tmp5.print();    
    cout<<"========================================="<<endl;
    
    return 0;
}
