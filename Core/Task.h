#ifndef TASK_H
#define TASK_H

#include <memory>

namespace XSPlayer {

class Task {
 public:
  virtual ~Task() = default;
  virtual void OnRun(void) = 0;
};

using TaskPtr = std::shared_ptr<Task>;

template <class F, class C>
class TaskImpl : public Task {
 public:
  TaskImpl(F function, C objct) : m_pFunction(function), m_pObject(objct) {}
  virtual ~TaskImpl() = default;

  virtual void OnRun(void) override {
    m_pFunction(m_pObject);
  }

 private:
  F m_pFunction;
  C m_pObject;
};

struct PlaceHolders {};

template <class F, class C>
TaskPtr CreateTask(F function, C object) {
  return std::make_shared<TaskImpl<F, C>>(function, object);
}

template <class F>
TaskPtr CreateTask(F function) {
  return std::make_shared<TaskImpl<F, PlaceHolders>>(function, PlaceHolders());
}

template <class T, class C>
class Args {
public:
    explicit Args(const T&& pData, const C& context) : m_data(pData), m_context(context) {}
    T&& GetData(void) { return std::move(m_data); }
    const T&& GetData(void) const { return std::move(m_data); }
    C GetContext() { return m_context; }
    const C& GetContext(void) const { return m_context; }
private:
    T m_data;
    C m_context;
};

}  // namespace XSPlayer

#endif  // TASK_H
