//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP

template<typename T>
class List
{
private:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}
    };

    Elem *head, *tail, *temp;

public:
    List() : head(0), tail(0), temp(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data)
    {
        Elem *elem = new Elem(data, head);
        head = elem;
        if (!tail) { tail = head; }
    }

    void addLast(T *data)
    {
        Elem *elem = new Elem(data, 0);
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
    }

    void remove(T* elem) {
        Elem* prev = nullptr;
        for(setFirst();notEnd(); next()) {
            if(temp->data == elem) {
                if(temp == head) {
                    if(head==tail) head = tail = nullptr;
                    else head = head->next;
                    delete temp;
                }
                else if(temp == tail) {
                    prev->next = nullptr;
                    tail = prev;
                    delete temp;
                }
                else {
                    prev->next = temp->next;
                    delete temp;
                }
                return;
            }
            prev = temp;
        }
    }

    T *removeFirst()
    {
        if (!head) { return 0; }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        delete elem;
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }

    void setFirst() {
        temp = head;
    }

    void next() {
        temp = temp->next;
    }

    bool notEnd() const {
        if(temp) return true;
        return false;
    }

    T *peekCurr() {
        if(temp) return temp->data;
        return nullptr;
    }

};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
