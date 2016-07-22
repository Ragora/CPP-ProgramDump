#include "LinkedList.h"

// Link Implementations
LinkedList::Link::Link(int value, Link *pointer) : data(value), pNext(pointer) { }

int LinkedList::Link::getData() { return this->data; }

LinkedList::Link* LinkedList::Link::getNext(void) { return this->pNext; }
void LinkedList::Link::setNext(LinkedList::Link *pointer) { this->pNext = pointer; }

// Linked List Implementations
LinkedList::LinkedList(void) : head(NULL), tail(NULL) { }

LinkedList::~LinkedList(void)
{
    if (this->isEmpty())
        return;

    Link *pCurrent = head;

    while (pCurrent)
    {
        Link *temp = pCurrent;

        pCurrent = pCurrent->getNext();
        delete temp;
    }
}

bool LinkedList::addToHead(int value)
{
    try
    {
        Link *newLink = new Link(value, NULL);

        if (!this->head)
        {
            this->head = this->tail = newLink;
            return true;
        }

        newLink->setNext(this->head);
        this->head = newLink;
    }
    catch (bad_alloc &e) { return false; }

    return true;
}

bool LinkedList::addToTail(int value)
{
    try
    {
        Link *newLink = new Link(value, NULL);

        if (!tail)
        {
            this->head = this->tail = newLink;
            return true;
        }

        this->tail->setNext(newLink);
        this->tail = newLink;
    }
    catch(bad_alloc &e) { return false; }

    return true;
}

bool LinkedList::addAt(int position, int value)
{
    if (position < 1 || this->isEmpty())
        return addToHead(value);

    Link *pCurrent = head;
    Link *pLast = NULL;

    for (int iteration = 1; iteration <= position && pCurrent; iteration++)
    {
        pLast = pCurrent;
        pCurrent = pCurrent->getNext();
    }

    // We got a valid position, so let's insert
    if (pCurrent)
    {
        // Make our new link point to current.
        try
        {
            Link *link = new Link(value, pCurrent);

            // Make our previous point to the new link if it exists (insertion at zero?)
            if (pLast)
                pLast->setNext(link);

            // If we ended up inserting at head, then adjust the head pointer accordingly.
            if (pCurrent == head)
                head = link;

            return true;
        }
        catch (bad_alloc &e) { return false; }
    }

    // We must be inserting at the tail at this point
    return addToTail(value);
}

bool LinkedList::getDataAtHead(int &value)
{
    if (!this->head)
        return false;

    value = this->head->getData();
    return true;
}

bool LinkedList::getDataAtTail(int &value)
{
    if (!this->tail)
        return false;

    value = this->tail->getData();
    return true;
}

bool LinkedList::getDataAt(int position, int &value)
{
    if (position < 1)
        return false;

    Link *pWorking = this->head;
    for (int iteration = 1; iteration <= position && pWorking; iteration++)
        pWorking = pWorking->getNext();

    if (pWorking)
    {
        value = pWorking->getData();
        return true;
    }

    return false;
}

bool LinkedList::removeDataAtHead(void)
{
    if (!this->head)
        return false;

    Link *detached = this->head;
    delete detached; // We don't need the memory anymore, just the pointer value

    // Move everything up one
    this->head = this->head->getNext();

    // If our head happened to be the tail, in which case there was only one element, we're now empty
    if (detached == this->tail)
        this->tail = NULL; // head will also be NULL since getNext() should have returned NULL

    return true;
}

bool LinkedList::removeDataAtTail(void)
{
    if (!this->tail)
        return false;

    // Loop until we find the tail in the list
    Link *pLast = NULL;
    Link *pCurrent = this->head;
    while ((pCurrent = pCurrent->getNext()) != this->tail)
        pLast = pCurrent;

    // Now perform the removal
    pLast->setNext(NULL);
    delete pCurrent;

    return true;
}

bool LinkedList::removeDataAtPosition(int &position)
{
    if (position < 1)
        return false;

    Link *pCurrent = this->head;
    Link *pLast = NULL;

    for (int iteration = 0; iteration < position && pCurrent; iteration++)
    {
        pLast = pCurrent;
        pCurrent = pCurrent->getNext();
    }

    // We got a valid position, so kill it
    if (pCurrent)
    {
        if (pLast)
            pLast->setNext(pCurrent->getNext());

        delete pCurrent;
        return true;
    }

    return false;
}

bool LinkedList::isEmpty(void)
{
    return !(this->head && this->tail);
}
