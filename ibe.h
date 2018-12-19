//
//  ibe.h
//  index_boolean_expression
//
//  Created by 王鑫凯 on 2018/12/16.
//  Copyright © 2018 王鑫凯. All rights reserved.
//


#ifndef ibe_h
#define ibe_h

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

class DNF
{
public:
    vector<int> Find(char *filename);
    DNF(char *filename);
    void traverse();
    
private:
    struct MATCH
    {
        int c_id;
        bool belong;
        double we;
    };
    
    struct node
    {
        MATCH value;
        node *next;
        node(){next=NULL;}
        node(MATCH v){value=v;next=NULL;}
    };
    //键值对
    struct KEY
    {
        string key;
        string value;
        bool belong;
        double UB;
        double ws;
        node *head;
        node *tail;
        KEY() {head=NULL;tail=NULL;}
        bool operator==(const KEY &thekey) const
        {
            if (thekey.key==key&&thekey.value==value)
                return true;
            else return false;
        };
    };
    
    // 值为size为k的键值对结合
    
    typedef vector<KEY> PLIST;
    PLIST *inverted_list;
    int kmax;
    
    int isexist(const KEY &nkey,int k);
    int isexistinplists(PLIST PLISTS ,const KEY &nkey,int k);
    int min(int a ,int b){return a<b?a:b;}
    
public:
    PLIST GetPostingLists(PLIST assignment,int K);
    node** initialize(PLIST PLISTS);
    void SortByCurrentEntries(PLIST &PLISTS,node **currententries);
    void SkipTo(PLIST PLISTS,node **currententries,int i,int NextID);
};


DNF::DNF(char *filename)
{
    ifstream fin(filename);
    int num_of_conj=0;
    int k;
    int key_idx;
    KEY key_tmp;
    MATCH match_tmp;
    //读取最大的K
    fin >> kmax;
    inverted_list=new PLIST[kmax+1];
    fin.get();
    fin.get();
    //开始读取dnf的每一个conjunction
    while (fin.peek()!=EOF&&fin.peek()!=13)
    {
        num_of_conj++;
        
        fin >> k; //读取当前conj的size,表示有k个项
        fin.get(); //把k后面换行给读掉
        fin.get();
        
        //读k个项
        while(fin.peek()!=EOF&& fin.peek()!=13)
        {
            //创建一个临时的KEY 建立好之后插入到inverted_list[k]里;
            fin >> key_tmp.key >> key_tmp.belong >> key_tmp.value;
            //建立一个临时的MATCH,之后链接在KEY中tail的后面
            //cout << key_tmp.key << key_tmp.belong << key_tmp.value << endl;
            
            match_tmp.c_id=num_of_conj;
            match_tmp.belong=key_tmp.belong;
            
            //key_tmp是否已经在inverted_list[k]中 返回tmp在inverted_list[k]中的位置
            key_idx=isexist(key_tmp,k);
            if (key_idx!=-1)
                inverted_list[k][key_idx].tail=inverted_list[k][key_idx].tail->next=new node(match_tmp);
            else
            {
                //在inverted_list[k]的末尾插入新的键值对key_tmp;
                key_tmp.head=key_tmp.tail=new node(match_tmp);
                inverted_list[k].push_back(key_tmp);
            }
            //读取分隔每个key的换行，加入接下来读的还是换行说明这个disjunctive已经读结束了
            if (fin.peek()!=EOF)
            {
                fin.get();
                fin.get();
            }
            
        }
        
        if (k==0)
        {
            key_tmp.key="Z";
            key_tmp.belong=true;
            key_tmp.value="0";
            
            match_tmp.c_id=num_of_conj;
            match_tmp.belong=key_tmp.belong;
            
            key_idx=isexist(key_tmp,k);
            if (key_idx!=-1)
                inverted_list[k][key_idx].tail=inverted_list[k][key_idx].tail->next=new node(match_tmp);
            else
            {
                //在inverted_list[k]的末尾插入新的键值对key_tmp;
                key_tmp.head=key_tmp.tail=new node(match_tmp);
                inverted_list[k].push_back(key_tmp);
            }
        }
        //读取分隔每个项的换行
        if (fin.peek()!=EOF)
        {
            fin.get();
            fin.get();
        }
    }
}
            
//在inverted_list[k]中查找是否有nkey
int DNF::isexist(const KEY &nkey,int k)
{
    //cout << "the size is " << inverted_list[k].size() <<endl;
    for (int i=0;i<inverted_list[k].size();++i)
    {
        if (inverted_list[k][i]==nkey)
            return i;
    }
    return -1;
}
//在PLISTS[k]中查找是否有nkey
int DNF::isexistinplists(PLIST PLISTS,const KEY &nkey,int k)
{
    //cout << "the size is " << inverted_list[k].size() <<endl;
    for (int i=0;i<PLISTS.size();++i)
    {
        if (PLISTS[i]==nkey)
            return i;
    }
    return -1;
}

void DNF::traverse()
{
    node *tmp;
    for (int i=0;i<=kmax;++i)
    {
        cout << "k = " << i <<endl;
//      cout << "the size is " << inverted_list[i].size() << endl;
        for (int j=0;j<inverted_list[i].size();++j)
        {
            cout << '(' << inverted_list[i][j].key << ',' <<  inverted_list[i][j].value << ')' << ':';
            tmp=inverted_list[i][j].head;
            
            while (tmp!=NULL)
            {
                cout << tmp->value.c_id << ' ' << tmp->value.belong << ' ' << '\t';
                tmp=tmp->next;
            }
            cout << endl;
        }
        cout << endl;
    }
}
//获得基于S的倒序列表
typename DNF::PLIST DNF::GetPostingLists(PLIST assignment,int K)
{
    PLIST PLISTS;
    int i=K;
    for(int j=0;j<inverted_list[i].size();++j){
        if(i==0){
            if (inverted_list[0][j].key=="Z") {
                PLISTS.push_back(inverted_list[0][j]);
            }
        }
        for(int m=0;m<assignment.size();m++){
            if(inverted_list[i][j].key==assignment[m].key && inverted_list[i][j].value==assignment[m].value&&isexistinplists(PLISTS, assignment[m], i)==-1)
                PLISTS.push_back(inverted_list[i][j]);
        }
    }
    return PLISTS;
}

//初始化PLIST的currententries
typename DNF::node** DNF::initialize(PLIST PLISTS)
{
    node **currententries;
    currententries=new node*[PLISTS.size()];
    for (int i=0; i<PLISTS.size(); ++i) {
        currententries[i]=PLISTS[i].head;
    }
    return currententries;
}

//按照currententries给PLIST排序
void DNF::SortByCurrentEntries(PLIST &PLISTS, node **currententries)
{
    for(int i=PLISTS.size()-1;i>0;--i){
        for(int j=0;j<i;j++){
            if(currententries[j]->value.c_id>currententries[j+1]->value.c_id){
                swap(PLISTS[j], PLISTS[j+1]);
                swap(currententries[j],currententries[j+1]);
            }
            if(currententries[j]->value.c_id==currententries[j+1]->value.c_id){
                if(currententries[j]->value.belong==1 && currententries[j+1]->value.belong==0){
                    swap(PLISTS[j], PLISTS[j+1]);
                    swap(currententries[j],currententries[j+1]);
                }
            }
        }
    }
}

//currententries跳至NextID
void DNF::SkipTo(PLIST PLISTS,node **currententries, int i,int NextID)
{
    while (currententries[i]->value.c_id<NextID) {
        if (currententries[i]->next==nullptr) {
            currententries[i]->value.c_id=INT_MAX;
            break;
        }
        currententries[i]=currententries[i]->next;
    }
}

//实现DNF查找的函数
vector<int> DNF::Find(char *filename)
{
    //获得assignment输入
    ifstream fin(filename);
    int k_of_s=0;
    //k_of_s是assignment的k
    fin>>k_of_s;
    fin.get();
    PLIST assignment;
    PLIST PLISTS;
    //使用currententries这一node指针数组储存当前指向
    node **currentries;
    int RejectID,NextID,k_replace;
    vector<int> Output;
    KEY key_tmp;
    while (fin.peek()!=EOF&&fin.peek()!=13){
        fin>>key_tmp.key>>key_tmp.belong>>key_tmp.value;
        fin.get();
        assignment.push_back(key_tmp);
    }
    //主循环
    for(int K=min(kmax,k_of_s);K>=0;--K){
        PLISTS=GetPostingLists(assignment,K);
        currentries=initialize(PLISTS);
        k_replace=K;
        if (K==0)
            k_replace=1;
        if (PLISTS.size()<k_replace)
            continue;
        while (currentries[k_replace-1]->value.c_id!=INT_MAX) {
            SortByCurrentEntries(PLISTS,currentries);
            if (currentries[0]->value.c_id==currentries[k_replace-1]->value.c_id) {
                if (currentries[0]->value.belong==0) {
                    RejectID=currentries[0]->value.c_id;
                    for (int i=0; i<PLISTS.size(); ++i) {
                        if(currentries[i]->value.c_id<=RejectID)
                            SkipTo(PLISTS,currentries,i,RejectID+1);
                        else
                            break;
                    }
                    //自己加的，否则nullptr在中部会导致退出循环
                    SortByCurrentEntries(PLISTS, currentries);
                    continue;
                }
                else{
                    Output.push_back(currentries[k_replace-1]->value.c_id);
                }
                NextID=currentries[k_replace-1]->value.c_id+1;
            }
            else{
                NextID=currentries[k_replace-1]->value.c_id;
            }
            for (int i=0; i<=k_replace-1; i++) {
                SkipTo(PLISTS, currentries, i, NextID);
            }
            if (currentries[0]->value.c_id==INT_MAX) {
                break;
            }
        }
    }
    return Output;
    
    
    
    //code for test
//    for (int i=0; currentries[i]!=nullptr; i++) {
//        cout<<currentries[i]->value.c_id<<endl;
//        cout<<currentries[i]->value.belong<<endl;
//    }
//    node *tmp;
//    for (int i=0;i<=kmax;++i)
//    {
//        cout << "k = " << i <<endl;
//        //      cout << "the size is " << inverted_list[i].size() << endl;
//        for (int j=0;j<PLISTS[i].size();++j)
//        {
//            cout << '(' << PLISTS[i][j].key << ',' <<  PLISTS[i][j].value << ')' << ':';
//            tmp=PLISTS[i][j].head;
//
//            while (tmp!=NULL)
//            {
//                cout << tmp->value.c_id << ' ' << tmp->value.belong << ' ' << '\t';
//                tmp=tmp->next;
//            }
//            cout << endl;
//        }
//        cout << endl;
//    }
//
//    
    
}


#endif /* ibe_h */
