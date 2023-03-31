#pragma once

// RingBuffer——Disruptor底层数据结构实现，核心类，是线程间交换数据的中转地；
// Sequencer——序号管理器，负责消费者/生产者各自序号、序号栅栏的管理和协调；
// Sequence——序号，声明一个序号，用于跟踪ringbuffer中任务的变化和消费者的消费情况；
// SequenceBarrier——序号栅栏，管理和协调生产者的游标序号和各个消费者的序号，确保生产者不会覆盖消费者未来得及处理的消息，确保存在依赖的消费者之间能够按照正确的顺序处理；
// EventProcessor——事件处理器，监听RingBuffer的事件，并消费可用事件，从RingBuffer读取的事件会交由实际的生产者实现类来消费；它会一直侦听下一个可用的序号，直到该序号对应的事件已经准备好。
// EventHandler——业务处理器，是实际消费者的接口，完成具体的业务逻辑实现，第三方实现该接口；代表着消费者。
// Producer——生产者接口，第三方线程充当该角色，producer向RingBuffer写入事件。

// Disruptor——对外暴露的门面类，提供start()，stop()，消费者事件注册，生产者事件发布等api；
// RingBuffer——对生产者提供下一序号获取、entry元素获取、entry数据更改等api；
// EventHandler——消费者的接口定义，提供onEvent()方法，负责具体业务逻辑实现；
// EventHandlerGroup——业务处理器分组，管理多个业务处理器的依赖关系，提供then()、before()、after()等api。

// 消费者序号数值必须小于生产者序号数值；
// 消费者序号数值必须小于其前置（依赖关系）消费者的序号数值；
// 生产者序号数值不能大于消费者中最小的序号数值，以避免生产者速度过快，将还未来得及消费的消息覆盖。


namespace lithe
{

template <typename T, int SIZE>
class RingBuffer
{
public:
    RingBuffer();
    T take();
    void put(T& elem);
private:
    T data_[SIZE];
    static const int start_ = 0;
    static const int end_ = SIZE - 1;
    int writeIndex_;
    int readIndex_;
};



} // namespace lithe
