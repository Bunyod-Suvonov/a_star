#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

int main()
{
    sf::Clock clock;
    for(int i=0; i<100000; i++){
        int a = 5+1000;
    }
    sf::Time elapsed = clock.getElapsedTime();
    cout << elapsed.asMicroseconds();
    clock.restart();

    return 0;
}