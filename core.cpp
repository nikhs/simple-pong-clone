#include<random>
#include<SFML/Window.hpp>
#include<SFML/Graphics.hpp>

auto monoGreen = sf::Color{0, 255 ,97} ;
constexpr int windowWidth{700}, windowHeight{350} ;
constexpr int fps{60} ;
constexpr float ballRadius{5.f}, maxBallVelocity{9.f} ;
constexpr float paddleWidth{70.f}, paddleHeight{5.f}, paddleVelocity{10.f}, paddleOffset{5.f} ;
constexpr double probability{0.4} ;
enum modes{ easy = 5, medium = 3, expert = 2  } ;

//Difficulty
constexpr float difficulty = float(modes{easy}) ;
constexpr float ballVelocity = maxBallVelocity - difficulty ;

// Generate randomness in AI movement
std::random_device rd ;
std::mt19937 rng(rd()) ;
std::bernoulli_distribution d(probability) ;

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
        shape.setFillColor(monoGreen) ;
    }
    
    void update(){
        shape.move(velocity) ;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > 0 ) velocity.x = -paddleVelocity ;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth ) velocity.x = paddleVelocity ;
        else velocity.x = 0 ;
        
    }
    
} ;

// Paddle for PC
struct AutoPaddle {
    sf::RectangleShape shape ;
    sf::Vector2f velocity{paddleVelocity, 0.f} ;
    
    float x()       { return shape.getPosition().x ; }
    float y()       { return shape.getPosition().y ; }
    float left()    { return x() - shape.getSize().x/2.f ; }
    float right()   { return x() + shape.getSize().x/2.f ; }
    float top()     { return y() - shape.getSize().y/2.f ; }
    float bottom()  { return y() + shape.getSize().y/2.f ; }
        
    AutoPaddle(float mx, float my){
        shape.setPosition(mx, my) ;
        shape.setSize({paddleWidth, paddleHeight}) ;
        shape.setOrigin(paddleWidth/2, paddleHeight/2) ;
        shape.setFillColor(monoGreen) ;
    }
    
    void update(float ballPosition){
                
        if( (velocity.x < 0.f && left() > 0)  || (velocity.x > 0.f && right() < windowWidth) ) 
            velocity.x = bool(d(rng)) ? velocity.x/difficulty : velocity.x ;
        else velocity.x = 0.f ;
        
        shape.move(velocity) ;
        
        if( ballPosition - ballRadius < left()) velocity.x = -paddleVelocity ;
        else if( ballPosition + ballRadius > right()) velocity.x = paddleVelocity ;
        else velocity.x = 0.f ;

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
        shape.setFillColor(monoGreen) ;
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

void testCollission(Ball& mBall, AutoPaddle& mPaddle){
    if(!isIntersecting(mBall, mPaddle)) return ;
    
    // Push the ball in a direction otherwise
    mBall.velocity.y = ballVelocity ;
    
    //Calculate x-velocity
    if(mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity ;
        else mBall.velocity.x = ballVelocity ;
    
}

int main(){

    sf::RenderWindow window{sf::VideoMode{windowWidth, windowHeight}, "Pong 0.1", sf::Style::Titlebar | sf::Style::Close} ;
    window.setFramerateLimit(fps) ;
    Ball ball{windowWidth/2.f, windowHeight/2.f} ;
    Paddle player{windowWidth/2.f, windowHeight-paddleOffset} ; 
    AutoPaddle opponent{windowWidth/2.f, paddleOffset} ;   

    while(true){

        window.clear(sf::Color::Black) ;
        
        // Escape on keypress
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) break ;
        
        ball.update() ;
        player.update() ;
        
        // AI reacts only if ball is moving towards it
        if(ball.velocity.y < 0)
            opponent.update(ball.x()) ;
        
        // Test for Collissions
        testCollission(ball, player) ;
        testCollission(ball, opponent) ;
        
        window.draw(ball.shape) ;
        window.draw(player.shape) ;
        window.draw(opponent.shape) ;
        window.display() ;
    }
    
    
    return 0 ;
    
}
