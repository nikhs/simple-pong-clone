#include<SFML/Window.hpp>
#include<SFML/Graphics.hpp>

constexpr int windowWidth{600}, windowHeight{400} ;
constexpr int fps{60} ;
constexpr float ballRadius{10.f}, ballVelocity{4.f} ;
constexpr float paddleWidth{60.f}, paddleHeight{20.f}, paddleVelocity{10.f} ;

// Paddle Object
struct Paddle{
    sf::RectangleShape shape ;
    sf::Vector2f velocity{paddleVelocity, 0.f} ;
    
    float x()       { return shape.getPosition().x ; }
    float y()       { return shape.getPosition().y ; }
    float left()    { return x() - shape.getSize().x/2.f ; }
    float right()   { return x() + shape.getSize().x/2.f ; }
    float top()     { return y() - shape.getSize().y/2.f ; }
    float bottom()  { return y() + shape.getSize().y/2.f ; }
    
    Paddle(float mx, float my){
        shape.setPosition(mx, my) ;
        shape.setSize({paddleWidth, paddleHeight}) ;
        shape.setOrigin(paddleWidth/2, paddleHeight/2) ;
        shape.setFillColor(sf::Color::Yellow) ;
    }
    
    void update(){
        shape.move(velocity) ;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0 ) velocity.x = -paddleVelocity ;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth ) velocity.x = paddleVelocity ;
        else velocity.x = 0 ;
        
    }
    
} ;

// Ball object
struct Ball{
    sf::CircleShape shape ;
    sf::Vector2f velocity{ballVelocity, ballVelocity} ;
    
    float x()       { return shape.getPosition().x ; }
    float y()       { return shape.getPosition().y ; }
    float left()    { return x() - shape.getRadius() ; }
    float right()   { return x() + shape.getRadius() ; }
    float top()     { return y() - shape.getRadius() ; }
    float bottom()  { return y() + shape.getRadius() ; }
    
    Ball(float mx, float my){
        shape.setRadius(ballRadius) ;
        shape.setPosition(mx, my) ;
        shape.setOrigin(ballRadius, ballRadius) ;
        shape.setFillColor(sf::Color::Red) ;
    }
    
    void update(){
        shape.move(velocity) ;
        
        // Prevent ball from moving outside window
        if( left() < 0 ) velocity.x = ballVelocity ;
        else if( right() > windowWidth ) velocity.x = -ballVelocity ;
        
        if( top() < 0 ) velocity.y = ballVelocity ;
        else if( bottom() > windowHeight ) velocity.y = -ballVelocity ;
    }
} ;

// Detect Collissions
template<class T1, class T2>
bool isIntersecting(T1& mA, T2& mB){
    return mA.left() <= mB. right() && mA.right() >= mB.left()
        && mA.top() <= mB.bottom() && mA.bottom() >= mB.top() ;
}

void testCollission(Ball& mBall, Paddle& mPaddle){
    if(!isIntersecting(mBall, mPaddle)) return ;
    
    // Push the ball in a direction otherwise
    mBall.velocity.y = -ballVelocity ;
    
    //Calculate x-velocity
    if(mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity ;
        else mBall.velocity.x = ballVelocity ;
    
}

int main(){

    sf::RenderWindow window{{windowWidth, windowHeight}, "Pong 0.1"} ;
    window.setFramerateLimit(fps) ;
    Ball ball{windowWidth/2.f, windowHeight/2.f} ;
    Paddle player{windowWidth/2.f, windowHeight-10} ;    

    while(true){

        window.clear(sf::Color::Black) ;
        
        // Escape on keypress
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) break ;
        
        ball.update() ;
        player.update() ;
        
        // Test for Collissions
        testCollission(ball, player) ;
        
        window.draw(ball.shape) ;
        window.draw(player.shape) ;
        window.display() ;
    }
    
    
    return 0 ;
    
}
