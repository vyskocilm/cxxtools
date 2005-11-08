/* cxxtools/thread.h
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef CXXTOOLS_THREAD_H
#define CXXTOOLS_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include <stdexcept>

namespace cxxtools
{

class ThreadException : public std::runtime_error
{
    int m_errno;

    static std::string formatMsg(const char* method, int e);

  public:
    ThreadException(const char* method, int e)
      : std::runtime_error(formatMsg(method, e)),
        m_errno(e)
      { }

    int getErrno() const
    { return m_errno; }
};

class Thread
{
    Thread(const Thread&);
    Thread& operator=(const Thread&);

  protected:
    pthread_t      pthreadId;
    pthread_attr_t pthread_attr;

  public:
    Thread();
    virtual ~Thread();

    virtual void create() = 0;

  protected:
    void create(void * (*start_routine)(void*));
    virtual void run() = 0;
};

class AttachedThread : public Thread
{
    static void* start(void* arg);
    bool joined;

  public:
    AttachedThread()
      : joined(false)
      { }
    virtual ~AttachedThread();

    void create()  { Thread::create(start); }
    void join();
};

class DetachedThread : public Thread
{
    static void* start(void* arg);

  protected:
    // Detached threads delete themself at end.
    // Users must create them on the heap.
    ~DetachedThread()  { }

  public:
    DetachedThread();
    void create()  { Thread::create(start); }
};

template <typename function_type, typename thread_type>
class FunctionThread : public thread_type
{
    function_type& function;

  public:
    FunctionThread(function_type& f)
      : function(f)
      { }

  protected:
    virtual void run()
    {
      function();
    }
};

template <typename function_type, typename thread_type>
Thread* createThread(function_type& function)
{
  Thread* thread = new FunctionThread<function_type, thread_type>(function);
  thread->create();
  return thread;
}

template <typename object_type, typename thread_type>
class MethodThread : public thread_type
{
    object_type& object;
    void (object_type::*method)();

  public:
    MethodThread(object_type& a, void (object_type::*m)())
      : object(a),
        method(m)
      { }

  protected:
    virtual void run()
    {
      (object.*method)();
    }
};

class Condition;

class Mutex
{
    friend class Condition;

    // make copy and assignment private without implementation
    Mutex(const Mutex&);
    const Mutex& operator= (const Mutex&);

  protected:
    pthread_mutex_t m_mutex;

  public:
    Mutex();
    ~Mutex();

    void lock();
    bool tryLock();
    void unlock();
};

class RWLock
{
    pthread_rwlock_t m_rwlock;

    // make copy and assignment private without implementation
    RWLock(const RWLock&);
    const RWLock& operator= (const RWLock&);

  public:
    RWLock();
    ~RWLock();

    void rdLock();
    void wrLock();
    void unlock();
};

template <class mutex_type,
          void (mutex_type::*lock_method)() = &mutex_type::lock,
          void (mutex_type::*unlock_method)() = &mutex_type::unlock>
class LockBase
{
    mutex_type& mutex;
    bool locked;

    // make copy and assignment private without implementation
    LockBase(const LockBase&);
    const LockBase& operator= (const LockBase&);

  public:
    LockBase(mutex_type& m, bool doLock = true)
      : mutex(m), locked(false)
    {
      if (doLock)
        lock();
    }

    ~LockBase()
    {
      if (locked)
        unlock();
    }

    void lock()
    {
      if (!locked)
      {
        (mutex.*lock_method)();
        locked = true;
      }
    }

    void unlock()
    {
      if (locked)
      {
        (mutex.*unlock_method)();
        locked = false;
      }
    }

    mutex_type& getMutex()
      { return mutex; }
};

typedef LockBase<Mutex> MutexLock;
typedef LockBase<RWLock, &RWLock::rdLock> RdLock;
typedef LockBase<RWLock, &RWLock::wrLock> WrLock;

class Semaphore
{
    sem_t sem;

  public:
    Semaphore(unsigned value);
    ~Semaphore();

    void wait();
    bool tryWait();
    void post();
    int getValue();
};

class Condition
{
    pthread_cond_t cond;

    // no implementation
    Condition(const Condition&);
    const Condition& operator= (const Condition&);

  public:
    Condition();
    ~Condition();

    void signal();
    void broadcast();
    void wait(MutexLock& lock);
};

}

#endif // CXXTOOLS_THREAD_H

