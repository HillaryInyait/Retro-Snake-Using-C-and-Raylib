#include<iostream>
#include <raylib.h>
#include<deque>
#include<raymath.h>

using namespace std;

Color green = {173 , 204, 86, 255};
Color darkGreen = {43 , 51, 24, 255};
int cellSize= 15;
int cellCount= 25;
float previousTime = 0;
int offset=38;

bool EvenTriggered(float interval){
    float currentTime = GetTime();
    if (currentTime - previousTime >= interval)
    {
        previousTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for(unsigned int i=0;i<=deque.size();i++){
        if (Vector2Equals(element,deque[i]))
        {
            return true;    
        }
    }
    return false;
}

class Snake{

    public:
    deque<Vector2> body = {Vector2{6,7},Vector2{5,7},Vector2{4,7}};
    Vector2 direction = {1,0};
    bool addSegment = false;

    void Draw(){
        for(unsigned int i =0; i< body.size();i++ ){
            float x = body[i].x;
            float y=body[i].y;
            Rectangle rec = {x * cellSize + offset, y * cellSize + offset, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(rec, 0.5 , 6, darkGreen);
        }
    }

    void Reset(){
        body = {Vector2{6,7},Vector2{5,7},Vector2{4,7}};
    }

    void Update(){
        body.push_front(Vector2Add(body[0],direction));
        if (addSegment)
        {
            addSegment= false;
        }
        else
        {
            body.pop_back();
        }
        
    }

    };

class Food {
        public:
            Vector2 position;
            Texture2D texture;

            Food(deque<Vector2> snakeBody){
                Image image = LoadImage("src/Assets/food.png");
                ImageResize(&image,cellSize,cellSize);
                texture = LoadTextureFromImage(image);
                UnloadImage(image);
                position = GenerateRandomCell();
            }

            ~Food(){
                UnloadTexture(texture);
            }

            void Draw(){
                DrawTexture(texture, position.x * cellSize + offset, position.y * cellSize+offset , WHITE);
            }

            Vector2 GenerateRandomCell(){
                float x =GetRandomValue(0, cellCount-1);
                float y =GetRandomValue(0, cellCount-1);
                Vector2 z = {x,y};
                return z;
            }

            Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
                Vector2 z = GenerateRandomCell();
                while (ElementInDeque(z,snakeBody))
                {
                    z= GenerateRandomCell();
                }    
                return z; 
            }
};

class Game{
    public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running= true;
    int score=0;
    Sound eatSound;
    Sound wallSound;

    Game(){
        InitAudioDevice();
        eatSound = LoadSound("src/Assets/eat.mp3");
        wallSound = LoadSound("src/Assets/wall.mp3");
    }
    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw(){
        snake.Draw();
        food.Draw();
    }
    void Update(){
        if(running){
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWIthBody();
        }
    }

    void CheckCollisionWithFood(){
        if (Vector2Equals(snake.body[0],food.position))
        {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment= true;
            score++;
            PlaySound(eatSound);
        }  
    }

    void CheckCollisionWithEdges(){
        if(snake.body[0].x == cellCount || snake.body[0].x == -1){
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1){
            GameOver();
        }
    }
    void CheckCollisionWIthBody(){
        deque<Vector2> tempDeque = snake.body;
        tempDeque.pop_front();
        if(ElementInDeque(snake.body[0],tempDeque)){
            GameOver();
        };
    }
    void GameOver(){
        running= false;
        snake.Reset();
        Draw();
        score=0;
        PlaySound(wallSound);
    }
};

int main()
{
    InitWindow(cellSize*cellCount+ 2*offset, cellSize*cellCount+2 *offset , "Retro Snake");
    SetTargetFPS(60);
    Game game = Game();

    while (WindowShouldClose() == false)
    {
        BeginDrawing();

        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0,-1};
            game.running=true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0,1};
            game.running=true;
        }
        if (IsKeyPressed(KEY_LEFT ) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1,0};
            game.running=true;
        }
        if (IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1,0};
            game.running=true;
        }
        
        //Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-4,(float)offset-4,(float)cellSize*cellCount+8,(float)cellSize*cellCount+8},4,darkGreen);
        DrawText("Retro Snake",offset-4,10,20,darkGreen);
        DrawText(TextFormat("%i",game.score),offset-4,offset+cellSize*cellCount+5,20,darkGreen);
        game.Draw();

        if (EvenTriggered(0.2))
        {
            game.Update();
        }
        
        EndDrawing();        
    }
    
    CloseWindow();
    return 0;
}