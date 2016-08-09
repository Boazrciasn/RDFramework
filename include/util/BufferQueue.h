#ifndef BUFFERQUEUE_H
#define BUFFERQUEUE_H
#include <QMutex>
#include <queue>


class BufferQueue
{

  public:
    BufferQueue(){}
    ~BufferQueue()
    {
        delete &m_buffer;
    }

    inline cv::Mat dequeue()
    {
        cv::Mat out;
        m_mutex.lock();
        if (!m_buffer.empty())
        {
            out = m_buffer.front();
            m_buffer.pop_front();
        }
        m_mutex.unlock();
        return out;
    }
    inline void enqueue(cv::Mat input)
    {
        m_mutex.lock();
        m_buffer.push_back(input);
        m_mutex.unlock();
    }
    inline int getSize()
    {
        int size;
        m_mutex.lock();
        size = m_buffer.size();
        m_mutex.unlock();
        return size;
    }
    inline bool empty()
    {
        bool empty;
        m_mutex.lock();
        empty = m_buffer.empty();
        m_mutex.unlock();
        return empty;
    }
    inline void clear()
    {
        m_mutex.lock();
        m_buffer.clear();
        m_mutex.unlock();
    }

  private:
    QMutex m_mutex;
    std::deque<cv::Mat> m_buffer;
};

#endif // BUFFERQUEUE_H
