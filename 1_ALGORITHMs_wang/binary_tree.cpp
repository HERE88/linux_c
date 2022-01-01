#include <iostream>
#include <vector>

using namespace std;

class BinTreeNode
{
    public:
        BinTreeNode()=default;
        explicit BinTreeNode(int);
        
        BinTreeNode(BinTreeNode *, int, char);

        void print(void);
    
        friend void PreorderTraversal(BinTreeNode *);
        friend void InorderTraversal(BinTreeNode *);
        friend void PostorderTraversal(BinTreeNode *);
    private:
        int data;
        BinTreeNode * left;
        BinTreeNode * right;        
};

BinTreeNode::BinTreeNode(int _data): data(_data), left(nullptr), right(nullptr)
{

}

BinTreeNode::BinTreeNode(BinTreeNode * node_p, int _data, char _dir) : data(_data), left(nullptr), right(nullptr)
{
    if(node_p)
    {
        if(_dir == 'L' || _dir == 'l')
            node_p->left = this;
        
        if(_dir == 'R' || _dir == 'r')
            node_p->right = this;        
    }
}

void BinTreeNode::print(void)
{
    cout<<data<<" ";
}


void PreorderTraversal(BinTreeNode * ptr)
{
    if(ptr == nullptr) return;

    ptr->print();
    PreorderTraversal(ptr->left);
    PreorderTraversal(ptr->right);
}

void InorderTraversal(BinTreeNode * ptr)
{
    if(ptr == nullptr) return;

    InorderTraversal(ptr->left);
    ptr->print();
    InorderTraversal(ptr->right);
}

void PostorderTraversal(BinTreeNode * ptr)
{
    if(ptr == nullptr) return;

    PostorderTraversal(ptr->left);
    PostorderTraversal(ptr->right);
    ptr->print();
}

int main()  
{
    BinTreeNode n1(11);
    //n1.print();

    // Construct the binary tree through constructor:
    BinTreeNode n2(&n1, 12, 'L');
    BinTreeNode n3(&n2, 14, 'L');
    BinTreeNode n4(&n2, 15, 'R');
    BinTreeNode n5(&n1, 13, 'R');
    BinTreeNode n6(&n5, 16, 'L');
    BinTreeNode n7(&n5, 17, 'R');

    // Traversal the binary tree:
    cout<<"PreorderTraversal============="<<endl;
    PreorderTraversal(&n1);
    cout<<endl;
    
    cout<<"InorderTraversal=============="<<endl;
    InorderTraversal(&n1);
    cout<<endl;
    
    cout<<"PostorderTraversal============"<<endl;
    PostorderTraversal(&n1);
    cout<<endl;

    
    return 0;
}
