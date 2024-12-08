```cpp
#include <QCoreApplication>
#include <QDebug>
#include <QObject>

class sender : public QObject
{
    Q_OBJECT

public:
    void do_something()
    {
        qDebug() << "Sender is doing something...";
        emit my_signal("Hello from Sender!");
    }

signals:
    void my_signal(const QString& message);
};

class receiver : public QObject
{
    Q_OBJECT

public slots:
    void on_message_received(const QString& message) { qDebug() << "Receiver received:" << message; }
};

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    sender my_sender;
    receiver my_receiver;

    // 连接信号与槽
    QObject::connect(&my_sender, &sender::my_signal, &my_receiver, &receiver::on_message_received);

    // 触发信号
    my_sender.do_something();

    return app.exec();
}
```



` emit my_signal("Hello from Sender!");` 相当于在Sender中定义了一个`vector<function<void(Qstring)>> slots_for_my_signal`，并调用`slots_for_my_signal`中保存的所有函数。



`connect(&my_sender, &sender::my_signal, &my_receiver, &receiver::on_message_received)`相当于向`my_sender->slots_for_my_signal`中`emplace_back()` `my_receiver`的`on_message_received()`成员函数。

Qt中的connect还可以指定第五个参数，使得槽函数只被绑定一次，或只触发一次。
