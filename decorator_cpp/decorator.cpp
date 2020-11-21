#include <iostream>
using namespace std;

class Component
{
public:
     virtual void Operation() = 0;
};

class ConcreteComponent : public Component
{
public:
     void Operation()
     {
          cout<<"I am no decoratored ConcreteComponent"<<endl;
     }
};

class Decorator : public Component
{
public:
     Decorator(Component *pComponent) : m_pComponentObj(pComponent) {}
     void Operation()
     {
          if (m_pComponentObj != NULL)
          {
               m_pComponentObj->Operation();
          }
     }
protected:
     Component *m_pComponentObj;
};

class ConcreteDecoratorA : public Decorator
{
public:
     ConcreteDecoratorA(Component *pDecorator) : Decorator(pDecorator){}
     void Operation()
     {
          AddedBehaviorA();
          Decorator::Operation();
     }
     void  AddedBehaviorA()
     {
          cout<<"This is added behavior A."<<endl;
     }
};

class ConcreteDecoratorB : public Decorator
{
public:
     ConcreteDecoratorB(Component *pDecorator) : Decorator(pDecorator){}
     void Operation()
     {
          AddedBehaviorB();
          Decorator::Operation();
     }
     void  AddedBehaviorB()
     {
          cout<<"This is added behavior B."<<endl;
     }
};

int main()
{
     Component *pComponentObj = new ConcreteComponent();

     cout<<"============================================="<<endl;
     Decorator *pDecoratorAOjb = new ConcreteDecoratorA(pComponentObj);  //--> A
     //pDecoratorAOjb->Operation();
     //cout<<"============================================="<<endl;

     //Decorator *pDecoratorBOjb = new ConcreteDecoratorB(pComponentObj);  //--> B
     //pDecoratorBOjb->Operation();
     //cout<<"============================================="<<endl;

     Decorator *pDecoratorBAOjb = new ConcreteDecoratorB(pDecoratorAOjb);  //--> BA (可以给基类的指针传入派生类的对象！！&&——虚函数的动态绑定概念！)
     pDecoratorBAOjb->Operation();
     cout<<"============================================="<<endl;

     delete pDecoratorBAOjb;
     pDecoratorBAOjb = NULL;
     //delete pDecoratorBOjb;
     //pDecoratorBOjb = NULL;
     delete pDecoratorAOjb;
     pDecoratorAOjb = NULL;
     delete pComponentObj;
     pComponentObj = NULL;

     return 0;
}