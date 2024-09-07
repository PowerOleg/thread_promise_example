#include <chrono>
#include <future>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

void accumulate(std::vector<int>::iterator first, std::vector<int>::iterator last, std::promise<int> accumulate_promise)
{
    int sum = std::accumulate(first, last, 0);
    std::cout << "1work_thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "2work_thread" << std::endl;
    accumulate_promise.set_value(sum); // Уведомить будущее
    std::cout << "3work_thread" << std::endl;
}

void do_work(std::promise<void> barrier)
{
    std::cout << "1new_work_thread" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "2new_work_thread" << std::endl;
    barrier.set_value();
    std::cout << "3new_work_thread" << std::endl;
}

int main()
{
    // Демонстрация использования Promise<int> для передачи результата между потоками.
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6 };
    std::promise<int> accumulate_promise;
    std::future<int> accumulate_future = accumulate_promise.get_future();
    std::thread work_thread(accumulate, numbers.begin(), numbers.end(), std::move(accumulate_promise));
    std::cout << "0main" << std::endl;


    std::cout << "result=" << accumulate_future.get() << '\n';//main thread is waiting here next command: "accumulate_promise.set_value(sum);"
    std::cout << "1main" << std::endl;
    work_thread.detach();//no matter what is here but anything should be
    std::cout << "2main" << std::endl;


    //to notify another thread
    // Демонстрируем использование обещания<void> для сигнализации состояния между потоками.
    std::promise<void> barrier;
    std::future<void> barrier_future = barrier.get_future();
    std::cout << "3main" << std::endl;
    std::thread new_work_thread(do_work, std::move(barrier));
    std::cout << "4main" << std::endl;
    barrier_future.wait();
    new_work_thread.join();
    std::cout << "5main" << std::endl;

    //here is the same thing like in the beggining. command: "future.get()" stops the main thread untill ".set_value()";
    std::promise<int> accumulate_promise2;//такаяже хрень как вначале...тупо future.get() всех держит пока не дождется .set_value();
    std::future<int> accumulate_future2 = accumulate_promise2.get_future();
    std::cout << "6main" << std::endl;
    std::thread work_thread2(accumulate, numbers.begin(), numbers.end(), std::move(accumulate_promise2));
    std::cout << "7main" << std::endl;
    std::cout << "result2=" << accumulate_future2.get() << '\n';
    std::cout << "8main" << std::endl;
    work_thread2.join();
    std::cout << "9main" << std::endl;
    return 0;
}