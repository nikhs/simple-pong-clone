#include<SFML/Window.hpp>
#include<SFML/Graphics.hpp>

constexpr int windowWidth{600}, windowHeight{400} ;
constexpr int fps{60} ;

int main(){
    sf::RenderWindow window{{windowWidth, windowHeight}, "Pong 0.1"} ;
    window.setFramerateLimit(fps) ;

    while(true){
        window.clear(sf::Color::Black) ;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) break ;
        window.display() ;
    }
    
    
    return 0 ;
    
}
