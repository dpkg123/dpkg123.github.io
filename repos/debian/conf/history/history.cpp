#include "history.hpp"

int main() {
    std::string year, month, day, time;

    std::cout << "please enter the year.\n";
    std::cin >> year;
    std::cout << "please enter the month.\n";
    std::cin >> month;
    std::cout << "please enter the day.\n";
    std::cin >> day;
    std::cout << "please enter the time.\n";
    std::cin >> time;

    std::string date = year + "-" + month + "-" + day + "T" + time;
    std::string cmd = "GIT_AUTHOR_DATE=" + date + " GIT_COMMITTER_DATE=" + date + " git commit -m '[skip ci]test'";

    system("git add .");
    system(cmd.c_str());
    system("git push -u origin main -f");

    std::cout << "Cool\n";

    return 0;
}
