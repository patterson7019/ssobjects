/********************************************************************
        Copyright (c) 2006, Lee Patterson & Ant Works Software

        created  :  3/20/2000 1:00am
        filename :  linkedlist.hpp
        author   :  Lee Patterson (workerant@users.sourceforge.net)
        
        purpose  :  Template double linked list class for storing any type of data

        notes    :  A linked list is either a master list, or a copy of a list. A
                    master list is the original list. A copy is a copy of the
                    master. A copied list points to the same data, but you can
                    travers the copy and not mess up the position information of
                    the master list. A copy should be treated as a read only
                    version of the master. You are able to force a copy to be a
                    master using forceMasterList() and clearMasterList methods;
                    if in the case of the master being only a temporary list
                    being passed to a method call, and this method call needs to
                    keep a copy of the list. Use this feature carefully!

                    The following actions are NOT allowed to be performed on a
                    copied list:

                    1) You are not allowed to remove the data that the list
                       points to. Use empty() instead of purge() to clear the
                       copied list. The LinkedList destructor knows when to purge or
                       empty the list.

                    2) You are not allowed to add items to the list. Since this
                       is a copy, a purge() call will not work, and you will end
                       up with memory leaks.

                    You ARE allowed to remove items in the list, as this does
                    not delete the data pointed to by the list. This is allowed
                    so you are able to for example, as a check list, remove
                    things from the list that have been worked with or
                    completed. Be aware that if you delete the items returned by
                    the remove methods, the master list will still point to the
                    data you just deleted.

*********************************************************************/
//TODO: RemoveHead and removeTail need to reassign the current pointer
//TODO  when the current pointer is at the head or tail. See removeCurrent()

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>
#include <assert.h>

namespace ssobjects
{

class ListNode
{
    public: //methods
        ListNode() :
          m_pvItem(NULL),/*m_pHead(NULL),m_pTail(NULL),*/m_pPrev(NULL),m_pNext(NULL),m_pCurrent(NULL),m_id(0) {}
    public: //attributes
        void*   m_pvItem;
//        ListNode* m_pHead;
//        ListNode* m_pTail;
        ListNode* m_pPrev;
        ListNode* m_pNext;
        ListNode* m_pCurrent;
        int m_id;

    private:
        //unused overloads
        ListNode(const ListNode&);
        ListNode& operator=(const ListNode&);
};

enum ListDirection {listPREV,listNEXT};

template <class T>
class LinkedList
{
  protected:
    unsigned long m_nNumEntries;
    unsigned long m_uniqueID;
    ListNode* m_pHead;
    ListNode* m_pTail;
    ListNode* m_pCurrent;

    // When m_bMasterList is NOT set, this is a copy of a list. As such, only list
    // attribues should be modified. The data that the list points to should not
    // be modified, as the master list should take care of that.
    bool m_bMasterList;

  public:
    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList& l)
      : m_nNumEntries(l.m_nNumEntries),
        m_uniqueID(l.m_uniqueID),
        m_pHead(NULL),
        m_pTail(NULL),
        m_pCurrent(NULL),
        m_bMasterList(false)
    {
      //make a copy of the list
      if(l.m_nNumEntries)
      {
        ListNode* pCurrentNode = l.m_pHead;

        //get the head item of the list
        ListNode* pHeadNode = new ListNode;
        pHeadNode->m_pvItem = pCurrentNode->m_pvItem;
        m_pHead = m_pTail = m_pCurrent = pHeadNode;

        //get the rest of the items in the list
        ListNode* pPrevNode = pHeadNode;
        while((pCurrentNode = pCurrentNode->m_pNext))
        {
          ListNode* pNewNode = new ListNode;
          pNewNode->m_pPrev = pPrevNode;
          pNewNode->m_pvItem = pCurrentNode->m_pvItem;

          // figure out if we are on the current node
          if(l.m_pCurrent  == pCurrentNode)
            m_pCurrent = pNewNode;

          pPrevNode->m_pNext = pNewNode;
          pPrevNode = pNewNode;

          m_pTail = pNewNode;
        }
      }
    }

    LinkedList& operator=(const LinkedList& l)
    {
      if(&l==this)        //object assigned to itself
        return *this;

      purge();
      m_nNumEntries = l.m_nNumEntries;
      m_uniqueID    = l.m_uniqueID;   //TODO: we should probably make this unique. should have a static id and thread safe if

      m_pHead = m_pTail = m_pCurrent = NULL;
      m_bMasterList = false;

      //make a copy of the list
      if(l.m_nNumEntries)
      {
        ListNode* pCurrentNode = l.m_pHead;

        //get the head item of the list
        ListNode* pHeadNode = new ListNode;
        pHeadNode->m_pvItem = pCurrentNode->m_pvItem;
        m_pHead = m_pTail = m_pCurrent = pHeadNode;

        //get the rest of the items in the list
        ListNode* pPrevNode = pHeadNode;
        while((pCurrentNode = pCurrentNode->m_pNext))
        {
          ListNode* pNewNode = new ListNode;
          pNewNode->m_pPrev = pPrevNode;
          pNewNode->m_pvItem = pCurrentNode->m_pvItem;

          // figure out if we are on the current node
          if(l.m_pCurrent  == pCurrentNode)
            m_pCurrent = pNewNode;

          pPrevNode->m_pNext = pNewNode;
          pPrevNode = pNewNode;

          m_pTail = pNewNode;
        }
      }
      return *this;
    }

    // See comments in notes section above when using these methods. They are
    // not be used casually.
    void forceMasterList() {m_bMasterList = true;}
    void clearMasterList() {m_bMasterList = false;}

    //adding and removing from the list
    //note: you can't add a const item, as when stored in list as a ListNode, it is 
    //  stored as void* and that breaks const rules. Also, if you retrieve a const
    //  list item, you wouldn't be able to change it's contents because of the const.
    void addTail(T* pItem);         
    void addHead(T* pItem);

    void empty();
    void purge();
    T* removeHead();
    T* removeTail();
    T* removeCurrent(const ListDirection direction = listNEXT);

    unsigned long getNumEntries(){return m_nNumEntries;}
    T* getHead();
    T* getTail();
    T* getNext();
    T* getPrev();
    T* getCurrent();
    T* getAt(unsigned long index);
};

template <class T>
LinkedList<T>::LinkedList()
        : m_nNumEntries(0),m_uniqueID(0),m_pHead(NULL),m_pTail(NULL),
          m_pCurrent(NULL),m_bMasterList(true)
{
}

template <class T>
LinkedList<T>::~LinkedList()
{
  if(m_bMasterList)
    purge();
  else
    empty();
}

//
//  Removes all nodes from the list, and deletes.
//  the objects accociated to the node.
//
template <class T>
void LinkedList<T>::purge()
{
  // You can only delete data pointed to by a list via a master list.
  // You should use the empty() method to clear this list.
  assert(m_bMasterList);  //see comment above
  if(!m_bMasterList)
    empty();

  if(m_nNumEntries)
  {
    T* o;
    ListNode* pNode = m_pHead;
    ListNode* pNext;
    while (pNode)
    {
      m_nNumEntries--;
      pNext = pNode->m_pNext;
      o=static_cast<T*>(pNode->m_pvItem);
      delete o;
      delete pNode;
      pNode = pNext;  
    }
    m_pHead=m_pTail=m_pCurrent=NULL;
    m_nNumEntries=0;
  }
}

//
//  Removes all nodes from the list, and deletes.
//  Objects are left untouched.
//
template <class T>
void LinkedList<T>::empty()
{
  if(m_nNumEntries)
  {
    ListNode* pNode = m_pHead;
    ListNode* pNext;
    while (pNode)
    {
      m_nNumEntries--;
      pNext = pNode->m_pNext;
      delete pNode;
      pNode = pNext;  
    }
    m_pHead=m_pTail=m_pCurrent=NULL;
    m_nNumEntries=0;
  }
}

template <class T>
void LinkedList<T>::addTail(T* pItem)
{
    assert(m_bMasterList);  // you can only add data to a master list, copies are read only

    ListNode* pTail = m_pTail;
    ListNode* pNode = new ListNode;

    pNode->m_pvItem = pItem;
    pNode->m_id=m_uniqueID++;
    pNode->m_pPrev = pTail;

    if(!pTail)
        m_pHead=pNode;
    else
        pTail->m_pNext = pNode;

    m_pTail = pNode;

    //if this is the first item in the list, make it the current item
    m_nNumEntries++;
    if(1 == m_nNumEntries)
      m_pCurrent = m_pTail;
}

template <class T>
void LinkedList<T>::addHead(T* pItem)
{
    assert(m_bMasterList);  // you can only add data to a master list, copies are read only

    ListNode* pHead = m_pHead;
    ListNode* pNode = new ListNode;

    pNode->m_pvItem = pItem;
    pNode->m_id=m_uniqueID++;
    
    //setup next & prev for new item
    pNode->m_pPrev = NULL;
    pNode->m_pNext = pHead;

    m_pHead = pNode;        //new head item
    if(!m_pTail)
        m_pTail=m_pHead;
    
    //setup next & prev for adjacent items

    if(pHead)
        pHead->m_pPrev = pNode;

    //if this is the first item in the list, make it the current item
    m_nNumEntries++;
    if(1 == m_nNumEntries)
      m_pCurrent = m_pHead;
}

template <class T>
T* LinkedList<T>::getHead()
{
    if(!m_pHead) return NULL;
    T* pItem = static_cast<T*>(m_pHead->m_pvItem);
    m_pCurrent = m_pHead;
    return pItem;
}

template <class T>
T* LinkedList<T>::getTail()
{
    if(!m_pTail) return NULL;
    T* pItem = static_cast<T*>(m_pTail->m_pvItem);
    m_pCurrent = m_pTail;
    return pItem;
}

template <class T>
T* LinkedList<T>::getNext()
{
    if(!m_pCurrent) return NULL;
    m_pCurrent = m_pCurrent->m_pNext;
    if(!m_pCurrent) return NULL;

    T* pItem = static_cast<T*>(m_pCurrent->m_pvItem);
    return pItem;
}

template <class T>
T* LinkedList<T>::getPrev()
{
    if(!m_pCurrent) return NULL;
    m_pCurrent = m_pCurrent->m_pPrev;
    if(!m_pCurrent) return NULL;

    T* pItem = static_cast<T*>(m_pCurrent->m_pvItem);
    return pItem;
}

template <class T>
T* LinkedList<T>::getCurrent()
{
    if(!m_pCurrent) return NULL;
    
    T* pItem = static_cast<T*>(m_pCurrent->m_pvItem);
    return pItem;
}

//
//RETURNS: the item you just removed
//    
template <class T>
T* LinkedList<T>::removeHead()
{
    T* o=NULL;
    ListNode* pNode = m_pHead;
    if(pNode)
    {
        o=static_cast<T*>(pNode->m_pvItem);
        if(pNode->m_pNext)
            pNode->m_pNext->m_pPrev=NULL;
        m_pHead=pNode->m_pNext;
        if(m_pHead==NULL)
            m_pTail=NULL;       //nothing left on the list

        delete pNode;
        m_nNumEntries--;
    }
    return o;
}

//
//RETURNS: the item you just removed
//    
template <class T>
T* LinkedList<T>::removeTail()
{
    T* o=NULL;
    ListNode*  pNode = m_pTail;
    if(pNode)
    {
        o=static_cast<T*>(pNode->m_pvItem);
        if(pNode->m_pPrev)
            pNode->m_pPrev->m_pNext=NULL;
        m_pTail=pNode->m_pPrev;
        if(m_pTail==NULL)
            m_pHead=NULL;       //nothing left on the list
        
        delete pNode;
        m_nNumEntries--;
    }
    return o;
}

//
//Removes the current node from the list, but does not free the 
//object itself. The current pointer will be moved either to the next
//item in the list, or prev, as specified by direction.
//
//RETURNS: the item you just removed
//    
template <class T>
T* LinkedList<T>::removeCurrent(const ListDirection direction)
{
    ListNode* pNode = m_pCurrent;

    if(!pNode)
        return NULL;

    T* pItem = NULL;
    pItem = static_cast<T*>(m_pCurrent->m_pvItem);
    m_nNumEntries--;
    if(!m_nNumEntries)
    {
        m_pHead=NULL;
        m_pTail=NULL;
        m_pCurrent = NULL;
        delete pNode;
        return pItem;
    }

    if (pNode->m_pPrev)
    {
        pNode->m_pPrev->m_pNext = pNode->m_pNext;
        if (pNode->m_pNext)
            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
        else
            m_pTail=pNode->m_pPrev;         //this was the last item in the list
    }
    else 
    {
        m_pHead=pNode->m_pNext;         //this was the first item in the list
        if(pNode->m_pNext)
            pNode->m_pNext->m_pPrev = NULL;
    }
    switch(direction)
    { 
        case listPREV: 
            //move current pointer to the prev item. 
            m_pCurrent = pNode->m_pPrev; 
            break; 

        case listNEXT: 
            //move current pointer to the next item. 
            m_pCurrent = pNode->m_pNext; 
            break; 

        default: assert(0); //should be PREV or NEXT only
    }

    delete pNode;
    return pItem;
}

template <class T>
T* LinkedList<T>::getAt(unsigned long index)
{
    if(m_nNumEntries)
    {
        ListNode* pNode = m_pHead;
        ListNode* pNext;
                while(pNode)
        {
            if(!index)
            {
                //we have reached the desired item
                T* pItem = static_cast<T*>(pNode->m_pvItem);
                return pItem;
            }
            index--;
            pNext = pNode->m_pNext;
            pNode = pNext;
        }
    }
    return NULL;
}

};

#endif //LINKEDLIST_H
