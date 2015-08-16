#include<random>
#include<SFML/Window.hpp>
#include<SFML/Graphics.hpp>
#include<SFML/Audio.hpp>


auto monoGreen = sf::Color{0, 255 ,97} ;
const std::string fontPath{"res/OpenSans-Regular.ttf"}, soundCollide{"res/collide.wav"}, soundScore{"res/score.wav"} ;
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
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && left() > paddleOffset ) velocity.x = -paddleVelocity ;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && right() < windowWidth - paddleOffset ) velocity.x = paddleVelocity ;
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
                
        if( (velocity.x < 0.f && left() > paddleOffset)  || (velocity.x > 0.f && right() < windowWidth - paddleOffset) ) 
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

    sf::SoundBuffer collideBuffer, scoreBuffer ;
    sf::Sound collide, score ;

    unsigned short playerScore{0}, opponentScore{0} ; // Score 
        
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
        
        // Init Sound
        collideBuffer.loadFromFile(soundCollide) ;
        scoreBuffer.loadFromFile(soundScore) ;
        collide.setBuffer(collideBuffer) ;
        score.setBuffer(scoreBuffer) ;
       
    }
    
    void update(){
        shape.move(velocity) ;
        
        // Prevent ball from moving outside window
        if( left() < 0 ){
            collide.play() ;
            velocity.x = ballVelocity ;
        }
        else if( right() > windowWidth ){
            collide.play() ;
            velocity.x = -ballVelocity ;
        }
        
        // Scoring
        
        if( bottom() < 0 ){
            score.play() ;
            ++opponentScore ;
            shape.setPosition(windowWidth/2.f, windowHeight/2.f) ;
        }
        
        if( top() > windowHeight ){
            score.play() ;
            ++playerScore ;
            shape.setPosition(windowWidth/2.f, windowHeight/2.f) ;
        }
        
        // Reflect ball vertically -- omitted
//        if( top() < 0 ) velocity.y = ballVelocity ;
//        else if( bottom() > windowHeight ) velocity.y = -ballVelocity ; 

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
    
    mBall.collide.play() ;
    // Push the ball in a direction otherwise
    mBall.velocity.y = -ballVelocity ;
    
    //Calculate x-velocity
    if(mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity ;
        else mBall.velocity.x = ballVelocity ;
    
}

void testCollission(Ball& mBall, AutoPaddle& mPaddle){
    if(!isIntersecting(mBall, mPaddle)) return ;
    
    mBall.collide.play() ;    
    // Push the ball in a direction otherwise
    mBall.velocity.y = ballVelocity ;
    
    //Calculate x-velocity
    if(mBall.x() < mPaddle.x()) mBall.velocity.x = -ballVelocity ;
        else mBall.velocity.x = ballVelocity ;
    
}

// Init sf Text elements
void sfTextInit(sf::Text& text, sf::Font& font, std::string s, int size, sf::Color& color){
    text.setFont(font) ;
    text.setString(s) ;
    text.setCharacterSize(size) ;
    text.setColor(color) ;
}

int main(){

    // UI
    sf::Font font ;
    sf::Text pause, desc , pScore, oScore;
    
    if(!font.loadFromFile(fontPath)) 
        return 1 ; // Exit with error

    sfTextInit(pause, font, "PAUSED", 160, monoGreen) ;
    sfTextInit(desc, font, "Press <P> or <ESC> or <SPACE> to resume. <Q> to quit.", 14, monoGreen) ;

    float ux{50.f}, uy{20.f} ;    // position reference
    pause.setPosition(ux, uy);
    desc.setPosition(ux+180.f, uy+170.f) ;
    
    // Score disp
    sfTextInit(pScore, font, std::to_string(0), 30, monoGreen) ;
    sfTextInit(oScore, font, std::to_string(0), 30, monoGreen) ;
    pScore.setPosition(5.f, uy+50.f);
    oScore.setPosition(5.f, uy+100.f) ;

//    sf::Time delay{sf::seconds(1.0f)} ; // For delay

    // Window
    sf::RenderWindow window{sf::VideoMode{windowWidth, windowHeight}, "Pong 1.0", sf::Style::Titlebar | sf::Style::Close} ;
    window.setFramerateLimit(fps) ;
    sf::Event event ;
    
    Ball ball{windowWidth/2.f, windowHeight/2.f} ;
    Paddle player{windowWidth/2.f, windowHeight-paddleOffset} ; 
    AutoPaddle opponent{windowWidth/2.f, paddleOffset} ;   

    
    // Specify Running, Paused states
    bool running{true} ;

    while(window.isOpen()){

        window.clear(sf::Color::Black) ;
        
        window.pollEvent(event) ;
        switch(event.type){
            case sf::Event::Closed :
                window.close() ; break ;
            case sf::Event::LostFocus :
                running = false ; break ;
            case sf::Event::KeyPressed :
                if( event.key.code == sf::Keyboard::Escape ||
                    event.key.code == sf::Keyboard::P ||
                    event.key.code == sf::Keyboard::Space )
                    running = !running ; // Pause Game
                if( event.key.code == sf::Keyboard::Q )
                    window.close() ; //Quit Game
                break ;
            default :
                break ;
        }
        
        // Update Score
        sfTextInit(pScore, font, std::to_string(ball.playerScore), 30, monoGreen) ;
        sfTextInit(oScore, font, std::to_string(ball.opponentScore), 30, monoGreen) ;
        
        if(running){
        
            // Core gameplay mechanics
            
            ball.update() ;
            player.update() ;
        
            // AI reacts only if ball is moving towards it
            if(ball.velocity.y < 0)
                opponent.update(ball.x()) ;
                
            // Test for Collissions
            testCollission(ball, player) ;
            testCollission(ball, opponent) ;
        }
        else{
            window.draw(pause) ;
            window.draw(desc) ;
        }
        
        window.draw(ball.shape) ;
        window.draw(player.shape) ;
        window.draw(opponent.shape) ;
        window.draw(pScore) ;
        window.draw(oScore) ;      
        window.display() ;
                            
    }
    
    
    return 0 ;
    
}
