// ChatTracker.cpp

// This is a correct but inefficient implementation of
// the ChatTracker functionality.

#include "ChatTracker.h"
#include <string>
#include <functional>
#include <iostream>



using namespace std;

/***********************/

template <class Val>
struct Node
{
    Node(string& k, Val& v) : key(k), val(v) {}
    string& key;
    Val& val;
    Node* next;
    Node* prev;
};

/***********************/

template <class Val>
class LinkedSet
{
public:
    LinkedSet();
    void add(string& k, Val& v);
    Node<Val>* find_node(string& k);
    Val* find(string& k);
    void remove(string& k);
    void for_all(void (*pf)(Val&));
    Val* last();

private:
    Node<Val>* head;
    Node<Val>* rear;
};

/***********************/

template <class Val>
class HashTable
{
public:
    HashTable();
    void add(string& k, Val& v);
    Val* find(string& k);
    void remove(string& k);
    void for_all(void (*pf)(Val&));

private:
    static const int SIZE = 1000;
    hash<string> str_hash;

    LinkedSet<Val> slots[SIZE];
};

template <class Val>
LinkedSet<Val>::LinkedSet()
{
    string k = string();
    Val n = Val();

    head = new Node<Val>(k, n);
    rear = new Node<Val>(k, n);

    head->next = rear;
    rear->prev = head;
}

template <class Val>
void LinkedSet<Val>::add(string& k, Val& v)
{
    cout << "LinkedSet==1" << endl;
    if (find(k) != NULL)
        return;

    Node<Val>* new_node = new Node<Val>(k, v);
    new_node->next = rear;

    Node<Val>* last = rear->prev;
    rear->prev = new_node;

    last->next = new_node;
    new_node->prev = last;
}

template <class Val>
Node<Val>* LinkedSet<Val>::find_node(string& k)
{
    cout << "LinkedSet==3" << endl;
    Node<Val>* p = head->next;

    while(p != rear)
    {
        cout << "LinkedSet==4" + p->key << endl;
        if (p->key == k)
            return p;
        p = p->next;
    }

    return NULL;
}

template <class Val>
Val* LinkedSet<Val>::find(string& k)
{
    Node<Val>* p_node = find_node(k);

    if (p_node == NULL)
        return NULL;

    return &(p_node->val);
}

template <class Val>
void LinkedSet<Val>::remove(string& k)
{
    cout << "LinkedSet==3" << endl;
    Node<Val>* found = find_node(k);

    if (found == NULL)
    {
        return;
    }

    Node<Val>* prev = found->prev;
    Node<Val>* next = found->next;

    prev->next = next;
    next->prev = prev;
}

template <class Val>
void LinkedSet<Val>::for_all(void (*pf)(Val&))
{
    Node<Val>* p = head->next;

    while(p != rear)
    {
        cout << "LinkedSet==travalse:" + p->key << endl;
        (*pf)(p->val);
        p = p->next;
    }
}

template <class Val>
Val* LinkedSet<Val>::last()
{
    if (rear->prev == head)
        return NULL;

    return &(rear->prev->val);
}

/**********************************/

template <class Val>
HashTable<Val>::HashTable()
{
    //cout << "hashTable==1" << endl;
}

template <class Val>
void HashTable<Val>::add(string& k, Val& v)
{
    //cout << "hashTable==2" << endl;
    int h = str_hash(k) % SIZE;
    cout << h << endl;
    slots[h].add(k, v);
}

template <class Val>
Val* HashTable<Val>::find(string& k)
{
    //cout << "hashTable==3" << endl;
    int h = str_hash(k) % SIZE;
    cout << h << endl;
    Val* n = slots[h].find(k);

    if (n == NULL)
       return NULL;

    return n;
}

template <class Val>
void HashTable<Val>::remove(string& k)
{
    //cout << "hashTable==3" << endl;
    int h = str_hash(k) % SIZE;
    slots[h].remove(k);
}

template <class Val>
void HashTable<Val>::for_all(void (*pf)(Val&))
{
    for (int i = 0; i < SIZE; i++)
    {
        slots[i].for_all(pf);
    }
}

// This class does the real work of the implementation.

class ChatTrackerImpl
{
public:
    ChatTrackerImpl(int maxBuckets);
    void join(string user, string chat);
    int terminate(string chat);
    int contribute(string user);
    int leave(string user, string chat);
    int leave(string user);
    void add(HashTable<LinkedSet<Info>>& container, Info& info);

private:
    struct Info
    {
        Info() {}
        Info(string u, string c) : user(u), chat(c), count(0) {}
        string user;
        string chat;
        int count;
    };

    HashTable<LinkedList<Info>> m_info;
    HashTable<LinkedList<Info>> m_usersWhoLeft;
};



//If you choose to use hash tables in your implementation, 
//no hash table may ever have a number of buckets greater than 
//the argument passed to the ChatTracker constructor.
//A simple way to test that you handle collisions correctly would be 
//to specify the number of buckets to be 2 or 3 (e.g., ChatTracker ct(3); 
//and run a small correctness test that puts several times that many items in the table.

    ChatTrackerImpl::ChatTrackerImpl(int /* maxBuckets */)
{
//    // Since this implementation does not use a hash table, we don't do
//    // anything with the parameter and leave it unnamed so the compiler
//    // doesn't issue a warning about an unused parameter.
}

//A user joins a chat
//The user joins a new or existing chat. 
//Whether or not the user was associated with that chat before, 
//that chat is now the user's current chat. If the user had previously 
//joined that chat and not left it since then, the user's count of 
//contributions to that chat remains the same; otherwise, it becomes 0. 
//If the chat had previously been joined by this or any other user and 
//has not been terminated since then, 
//the chat's count of contributions is unchanged; otherwise, it becomes 0. 
void ChatTrackerImpl::join(string user, string chat)
{
    LinkedSet<Info>* p_chat = m_info.find(user);

    if (p_chat == NULL)
    {
        p_chat = new LinkedSet<Info>();
        m_info.add(user, *p_chat);
    }

    Info* p_info = p_chat->find(chat);

    if (p_info == NULL)
    {
        p_info = new Info(user, chat);
    }

    p_chat->remove(chat);
    p_chat->add(chat, *p_info);
}


//A chat is terminated:
//If the chat does not exist, this function returns 0; 
//otherwise, all users associated with the chat are no longer 
//associated with it(as if they left the chat), and 
//the function returns the chat's count of contributions. 
//If that chat was any user's current chat, then the existing chat
//the user most recently joinedand hasn't left since most recently 
//joining becomes the user's current chat; 
//if there is no such chat, then that user has no current chat.
int ChatTrackerImpl::terminate(string chat)
{
    int total = 0;
    vector<Info>::iterator p = m_info.begin();
    while (p != m_info.end())
    {
        if (p->chat == chat)
        {
            total += p->count;
            p = m_info.erase(p);
        }
        else
            p++;
    }
    p = m_usersWhoLeft.begin();
    while (p != m_usersWhoLeft.end())
    {
        if (p->chat == chat)
        {
            total += p->count;
            p = m_usersWhoLeft.erase(p);
        }
        else
            p++;
    }
    return total;
}

// A user contributes to that user's current chat:
//If the user has no current chat, return 0. Otherwise, 
//both the user's count of contributions to that user's current chat and that 
//current chat's count of contributions are increased by one. 
//Return the resulting user's count of contributions to that current chat.
int ChatTrackerImpl::contribute(string user)
{
    LinkedSet<Info>* found = m_info.find(user);

    if (found == NULL)
        return 0;

    Info* last = found->last();

    if (last == NULL)
    {
        return 0;
    }

    last->count++;

    return last->count;
}

//A user leaves the chat:
//If the user is not associated with the indicated chat, return -1.
//Otherwise, the user is no longer associated with that chat, and 
//the function returns the user's count of contributions to that chat. 
//If that chat was the user's current chat, then the existing chat 
//the user most recently joinedand hasn't left since most recenty joining 
//becomes the user's current chat; if there is no such chat, 
//then that user has no current chat.
//This function leaves the chat's count of contributions unchanged. 
int ChatTrackerImpl::leave(string user, string chat)
{
    LinkedSet<Info>* p_chat = m_info.find(user);

    if (p_chat == NULL)
    {
        return -1;
    }

    Info* p_info = p_chat->find(chat);

    if (p_info == NULL)
    {
        return -1;
    }

    add(m_usersWhoLeft, *p_info);
    p_chat->remove(chat);

    return p_info->count;
}

//A user leaves that users' current chat
//If the user has no current chat, return -1. 
//Otherwise, the user is no longer associated with that user's current chat, 
//and the function returns the user's count of contributions made to that chat.
//The existing chat the user most recently joined and hasn't left since most recently 
//joining bcomes the user's current chat; if there is no such chat, 
//then that user has no current chat.
//This function leaves the chat's count of contributions unchanged. 
int ChatTrackerImpl::leave(string user)
{
    LinkedSet<Info>* found = m_info.find(user);

    if (found == NULL)
        return -1;

    Info* last = found->last();

    if (last == NULL)
    {
        return -1;
    }

    add(m_usersWhoLeft, *last);
    m_info.remove(last->user);

    return last->count;
}
void ChatTrackerImpl::add(HashTable<LinkedSet<Info>>& container, Info& info)
{
    LinkedSet<Info>* p_chat = container.find(info.user);

    if (p_chat == NULL)
    {
        p_chat = new LinkedSet<Info>();
        container.add(info.user, *p_chat);
    }

    p_chat->add(info.chat, info);
}
//*********** ChatTracker functions **************

// These functions simply delegate to ChatTrackerImpl's functions.
// You probably don't want to change any of this code.

ChatTracker::ChatTracker(int maxBuckets)
{
    m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
    delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
    m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
    return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
    return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
    return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
    return m_impl->leave(user);
}

//If you choose to use hash tables in your implementation, 
//no hash table may ever have a number of buckets greater than 
//the argument passed to the ChatTracker constructor. 
//A simple way to test that you handle collisions correctly would be 
//to specify the number of buckets to be 2 or 3 (e.g., ChatTracker ct(3); 
//and run a small correctness test that puts several times that many items in the table.
//If you don't use any hash tables, your constructor
//may ignore the value of its parameter (as the one in ChatTracker.slow.cpp does). 
