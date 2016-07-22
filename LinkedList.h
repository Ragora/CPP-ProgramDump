/**
 *  @file LinkedList.h
 *  @brief Implementation of a basic uni-directional one-indexed LinkedList class
 *  for the Data Structures and Algorithms class.
 *  @author Robert MacGregor
 */

#ifndef _LINKED_LIST_
#define _LINKED_LIST_

#include <stdlib.h>
#include <iostream>
#include <iterator>

using namespace std;

/**
 *  A basic LinkedList implementation for the Data Structures and Algorithms
 *  class. A LinkedList will provide a dynamically resizable data structure
 *  for storing arbitrary amounts of data based around sticking nodes in the
 *  heap and having them point to each other. However, this implementation will
 *  provide O(N) access to all elements on a one-indexed number system rather
 *  than the O(1) zero-indexed access of a traditional array.
 */
template <typename storedType>
class LinkedList
{
 // Private Members
    private:
        /**
         *  A node in the LinkedList class. It will point to the next node in the
         *  list, or NULL if it happens to be the tail of the LinkedList.
         */
        struct Link
        {
            /**
             *  @brief Constructor accepting a value and a pointer to the next Link.
             *  @param value The value to store in this new Link.
             *  @param pointer A pointer to the next Link.
             */
            Link(storedType *value, Link *pointer) : data(value), pNext(pointer)
            {

            }

            /**
             *  @brief Returns the data contained in this Link.
             *  @return The value contained in this Link.
             */
            storedType *getData() { return this->data; }

            /**
             *  @brief Returns a pointer to the next Link.
             *  @return A pointer to the next Link.
             */
            Link* getNext(void) { return this->pNext; }


            /**
             *  @brief Pointers this Link to another Link somewhere on the heap.
             *  @param pointer The new pointer value to point to.
             */
            void setNext(Link *pointer) { this->pNext = pointer; }

            //! The data that this Link contains.
            storedType *data;
            //! A pointer to the next Link. NULL if this is tail.
            Link *pNext;
        };

        //! A pointer to the head of the LinkedList.
        Link *head;
        //! A pointer to the tail of the LinkedList.
        Link *tail;

    // Public Methods
    public:

        /**
         *  @brief Parameter-less constructor.
         */
        LinkedList(void) : head(NULL), tail(NULL) { }

        /**
         *  @brief Standard destructor.
         */
        ~LinkedList(void)
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

        /**
         *  @brief Adds value to the head of this LinkedList. It effectively prepends
         *  value onto the LinkedList as a whole.
         *  @param value The vlaue to insert at the head.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the memory for the insertion operation could not be allocated.
         */
        bool addToHead(storedType value)
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

        /**
         *  @brief Adds value to the tail of this LinkedList. It effectively appends
         *  value onto the LinkedList as a whole.
         *  @param value The value to insert at the tail.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the memory for the insertion operation could be allocated.
         */
        bool addToTail(storedType *value)
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


        /**
         *  @brief Adds value at position in the LinkedList. It will be inserted before
         *  the element that already exists at the given location, if there is any.
         *  @param position The position to insert at.
         *  @param value The value to insert into the LinkedList.
         *  @return A boolean representing the success of the operation.
         *  @retval false Returned if the memory for the insertion operation could not be allocated.
         *  @note This method is O(N) complexity because of the implementation. It must iterate to
         *  find the desired Link to insert our value before in the LinkedList.
         */
        bool addAt(int position, storedType *value)
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

        /**
         *  @brief Gets the value currently stored at the head and assigns it to value.
         *  @param value A reference to the value to be assigned to as a return.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         */
        bool getDataAtHead(storedType &value)
        {
            if (!this->head)
                return false;

            value = *this->head->getData();
            return true;
        }

        /**
         *  @brief Gets the value currently stored at the tail and assigns it to value.
         *  @param value A reference to the value to be assigned to as a return.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         */
        bool getDataAtTail(storedType &value)
        {
            if (!this->tail)
                return false;

            value = *this->tail->getData();
            return true;
        }

        /**
         *  @brief Gets the value currently stored at the position desiginated by position
         *  and assigns it to value.
         *  @param position The position in the LinkedList to read from.
         *  @param value A reference to the value to be assigned to as a return.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         *  @retval false Returned if position is out of range (position < 1 || position >= length)
         *  @note This operation is O(N) complexity because of the implementation. It must iterate to
         *  the given position to find the desired Link's data.
         */
        bool getDataAt(int position, storedType &value)
        {
            if (position < 1)
                return false;

            Link *pWorking = this->head;
            for (int iteration = 1; iteration <= position && pWorking; iteration++)
                pWorking = pWorking->getNext();

            if (pWorking)
            {
                value = *pWorking->getData();
                return true;
            }

            return false;
        }

        /**
         *  @brief Removes the head from the LinkedList, moving all the elements up by one.
         *  @note When the head is removed, the Link after head becomes the new head.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         */
        bool removeDataAtHead(void)
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

        /**
         *  @brief Removes the tail from the LinkedList, moving all the elements down by one.
         *  @note When the tail is removed, the Link before tail becomes the new tail.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         *  @note This operation is O(N) complexity because of the implementation. It must
         *  iterate to the end of the LinkedList, therefore the more Links you have, the slower
         *  this method will become.
         */
        bool removeDataAtTail(void)
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

        /**
         *  @brief Removes the Link at the location specified by position.
         *  @param position The position of the Link to attempt to remove.
         *  @return A boolean representing whether or not the operation was successful.
         *  @retval false Returned if the LinkedList is empty.
         *  @retval false Returned if position is out of bounds (position < 1 || position > length)
         *  @note This operation is O(N) complexity because of the implementation. It must iterate
         *  to the given position to find the desired Link.
         */
        bool removeDataAtPosition(int &position)
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

        /**
         *  @brief Returns whether or not the LinkedList is empty.
         *  @return A boolean representing whether or not the LinkedList is empty.
         */
        bool isEmpty(void)
        {
            return !(this->head && this->tail);
        }

        /**
         *  @brief Stream insertion operator to print out the LinkedList contents
         *  in a comma delineated format.
         *  @param os The input std::ostream to write to.
         *  @param list The LinkedList to write into the stream.
         *  @return A reference to the std::ostream we wrote to.
         *  @note This operation is O(N) complexity because we must iterate over the
         *  entire LinkedList to print out its contents
         */
        friend ostream& operator<<(ostream &os, LinkedList const& list)
        {
            Link* pWorking = list.head;

            while (pWorking != NULL)
            {
                //os << (*pWorking).getData() << ", ";
                os << *(*pWorking).getData();
                pWorking = (*pWorking).getNext();
            }

            return os;
        }

    //! Internal iterator for iterator access on the LinkedList.
    class iterator : public std::iterator<input_iterator_tag, storedType>
    {
        // Private Members
        private:
            //! The current link pointer.
            Link *pCurrent;

        // Public Methods
        public:
            //! Constructor accepting a pointer to a Link.
            iterator(Link *pointer) : pCurrent(pointer) { }
            //! Copy constructor.
            iterator(const iterator &iter) : pCurrent(iter.pCurrent) { }
            //! Prefix increment operator.
            iterator& operator ++() { pCurrent = pCurrent->getNext(); return *this; }
            //! Postfix increment operator.
            iterator operator ++(int) { iterator temp(*this); operator++(); return temp; }
            //! Equals operator.
            bool operator ==(const iterator& rhs) { return pCurrent == rhs.pCurrent; }
            //! Not equals operator.
            bool operator !=(const iterator& rhs) { return pCurrent != rhs.pCurrent; }
            //! Derference operator.
            storedType *operator*() { return pCurrent->data; }
    };

    /**
     *  @brief Returns an iterator to the end of the LinkedList.
     *  @return An iterator to the end of the LinkedList.
     */
    iterator end(void)
    {
        return iterator(NULL);
    }

    /**
     *  @brief Returns an iterator to the beginning of the LinkedList.
     *  @return An iterator to the beginning of the LinkedList.
     */
    iterator begin(void)
    {
        return iterator(this->head);
    }
};

#endif // _LINKED_LIST_
