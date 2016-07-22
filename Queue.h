/**
 *  @file Queue.h
 *  @brief Declaration for a generically typed Queue class.
 *  @author Robert MacGregor
 */

#ifndef _INCLUDE_QUEUE_H_
#define _INCLUDE_QUEUE_H_

#include <iostream>

using namespace std;

/**
 *  @brief A generically typed Queue class. It is essentially just a LinkedList
 *  with Stack semantics.
 *  @param storedType The type to store in our Queue.
 */
template <typename storedType>
class Queue
{
    // Public Methods
    public:
        /**
         *  @brief Parameter-less constructor.
         */
        Queue(void) : head(NULL), tail(NULL)
        {
        }

        /**
         *  @brief Standard destructor.
         */
        ~Queue(void)
        {
            Link *current = this->head;

            while (current)
            {
                Link *next = current->getNext();
                delete current;
                current = next;
            }
        }

        /**
         *  @brief Adds a value to this Queue.
         *  @param value The value to add to the end of the Queue.
         *  @throw bad_alloc Thrown when there was a failure to allocate memory in
         *  the heap for the new Link.
         */
        void add(storedType value)
        {
            if (this->isEmpty())
            {
                this->head = this->tail = new Link(value, NULL);
                return;
            }

            Link *newLink = new Link(value, NULL);

            this->tail->setNext(newLink);
            this->tail = newLink;
        }

        /**
         *  @brief Pops a value from the end of our Queue.
         *  @return The stored value at the end of the Queue.
         */
        storedType pop(void)
        {
            // FIXME (Robert MacGregor#9): Error state?
            if (this->isEmpty())
                return 0;

            // If there's only one element, just delete head and set both to NULL
            storedType result;
            if (this->head == this->tail)
            {
                result = this->head->getData();
                delete this->head;

                this->head = this->tail = NULL;
                return result;
            }

            result = this->head->getData();
            Link *temp = this->head;

            this->head = this->head->getNext();
            delete temp;

            return result;
        }

        /**
         *  @brief Returns whether or not this Queue is empty.
         *  @return A boolean representing whether or not this Queue is empty.
         */
        bool isEmpty(void) const
        {
            return !(this->head && this->tail);
        }

        /**
         *  @brief Stream insertion operator to put a Queue into a stream.
         *  @param stream The std::ostream to write into.
         *  @param input The Queue to write into the stream.
         *  @return A reference to the input stream.
         */
        friend ostream& operator <<(ostream &stream, const Queue<storedType> &input)
        {
            if (!input.isEmpty())
            {
                stream << input.head->getData();

                Link *current = input.head;
                while ((current = current->getNext()) != NULL)
                    stream << ", " << current->getData();
            }

            return stream;
        }

    // Private Members
    private:
        /**
         *  A node in the Queue class. It will point to the next node in the
         *  list, or NULL if it happens to be the tail of the Queue.
         */
        struct Link
        {
            /**
             *  @brief Constructor accepting a value and a pointer to the next Link.
             *  @param value The value to store in this new Link.
             *  @param pointer A pointer to the next Link.
             */
            Link(storedType value, Link *next) : data(value), pNext(next)
            {

            }

            /**
             *  @brief Returns the data contained in this Link.
             *  @return The value contained in this Link.
             */
            storedType getData(void) const
            {
                return data;
            }

            /**
             *  @brief Returns a pointer to the next Link.
             *  @return A pointer to the next Link.
             */
            Link* getNext(void) const
            {
                return pNext;
            }

            /**
             *  @brief Points this Link to another Link somewhere on the heap to refer to
             *  as next.
             *  @param pointer The new pointer value to point to.
             */
            void setNext(Link* pointer)
            {
                pNext = pointer;
            }

            //! The data that this Link contains.
            storedType data;
            //! A pointer to the next Link. NULL if this is tail.
            Link *pNext;
        };

        //! A pointer to the head.
        Link *head;
        //! A pointer to the tail.
        Link *tail;
};
#endif // _INCLUDE_QUEUE_H_

/*
10
10, 20
10, 20, 30
10, 20, 30, 40
10, 20, 30, 40, 50
10, 20, 30, 40, 50, 60
10, 20, 30, 40, 50, 60, 70
10, 20, 30, 40, 50, 60, 70, 80
10, 20, 30, 40, 50, 60, 70, 80, 90
10, 20, 30, 40, 50, 60, 70, 80, 90
20, 30, 40, 50, 60, 70, 80, 90
30, 40, 50, 60, 70, 80, 90
40, 50, 60, 70, 80, 90
50, 60, 70, 80, 90
60, 70, 80, 90
70, 80, 90
80, 90
90
Job Done

*/
