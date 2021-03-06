#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

auto print_stuff_from_queue(std::queue<std::string>& kolejka,
                            std::mutex& mtx,
                            std::condition_variable& cv,
                            int const id) -> void

{
    auto s = std::string{};

    while (true) {
        std::unique_lock<std::mutex> lck{mtx};
        cv.wait(lck);

        if (kolejka.empty()) {
            lck.unlock();
            continue;
        }

        s = kolejka.front();

        kolejka.pop();

        if (s.empty()) {
            std::cout << "thread No. " << id << " exiting\n";
            break;

        } else {
            std::cout << "from thread No. " << id << " : " << s << "\n";
        }
    }
}


auto main() -> int
{
    auto kolejka = std::queue<std::string>{};

    auto threads_vector = std::vector<std::thread>{};

    std::mutex mtx;

    std::condition_variable cv;

    const int liczba_watkow = 4;

    for (auto i = int{0}; i < liczba_watkow; i++) {
        threads_vector.push_back(std::thread(print_stuff_from_queue,
                                             std::ref(kolejka),
                                             std::ref(mtx),
                                             std::ref(cv),
                                             i));
    }

    auto j = int{0};

    auto s = std::string{};

    while (j < liczba_watkow) {
        std::getline(std::cin, s);

        if (s.empty()) {
            j++;
        }

        {
            std::unique_lock<std::mutex> lck{mtx};
            kolejka.push(std::move(s));
        }

        cv.notify_one();
    }

    for (auto& each : threads_vector) {
        each.join();
    }

    return 0;
}
