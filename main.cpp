#include<iostream>
#include<GLUT/glut.h>
#include<string>
#include<cstdlib>
using namespace std;


float paddleSpeed = 0.2f;
float ballX = 0.0f, ballY = 0.0f;
float ballDX = 0.009f, ballDY = 0.009f;
float ballIncRate = 0.00018f;
float leftPaddleY = 0.0f, rightPaddleY = 0.0;
int leftScore = 0, rightScore = 0;

bool isGameOver = false;
int gameDuration = 60;
long int startTime = 0;

string flashSide = "";
int flashTimer = 0;

float borderColor[4][3];
long int lastBorderUpdateTime = 0;


void overLayPrint(int arr[4][2]){
    glBegin(GL_QUADS);
        for (int i=0; i<4; i++){
            glVertex2f(arr[i][0], arr[i][1]);
        }
    glEnd();
}

void drawFlashOverlay(){
    
    if(flashTimer > 0){
        float alpha = 0.2f * (flashTimer / 30.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if(flashSide == "left"){
            glColor4f(0, 1, 0, alpha);
            int arr[4][2] = { {-1, -1}, {0, -1}, {0, 1}, {-1, 1} };
            overLayPrint(arr);
        } else if(flashSide == "right"){
            glColor4f(0, 1, 0, alpha);
            int arr[4][2] = { {0, -1}, {1, -1}, {1, 1}, {0, 1} };
            overLayPrint(arr);
        }
        
        glDisable(GL_BLEND);
        flashTimer --;
        if(flashTimer == 0){
            flashSide = "";
        }
    }
}


void drawBoundaryBox(){
    glColor3f(0, 0, 0);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-1.0f, -1.0f);
        glVertex2f(1.0f, -1.0f);
        glVertex2f(1.0f, 1.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();
}

void drawBorderDots(){
    glPointSize(10.0f);
    glBegin(GL_POINTS);
        for(float i = -1.0f; i<=1.0; i+= 0.05f){
            //top
            glColor3fv(borderColor[0]);
            glVertex2f(i, 1.0f);
            
            //bottom
            glColor3fv(borderColor[1]);
            glVertex2f(i, -1.0f);
            
            //left
            glColor3fv(borderColor[2]);
            glVertex2f(-1.0f, i);
            
            //right
            glColor3fv(borderColor[3]);
            glVertex2f(1.0f, i);
            
        }
    glEnd();
}

void drawText(float x, float y, const string& str){
    glRasterPos2f(x, y);
    for(char c: str){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
    
}

void drawPaddle(float x, float y){
    glBegin(GL_QUADS);
        glVertex2f(x, y-0.2f);
        glVertex2f(x+0.05f, y-0.2f);
        glVertex2f(x+0.05f, y+0.2f);
        glVertex2f(x, y+0.2f);
    glEnd();
}

void drawBall(){
    glColor3f(0, 0, 0);
    glBegin(GL_QUADS);
        glVertex2f(ballX-0.02f, ballY-0.02f);
        glVertex2f(ballX+0.02f, ballY-0.02f);
        glVertex2f(ballX+0.02f, ballY+0.02f);
        glVertex2f(ballX-0.02f, ballY+0.02f);
    glEnd();
}

void display(){
    glClearColor(0.95f, 0.95f, 0.95f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    drawFlashOverlay();
    drawBoundaryBox();
    drawBorderDots();
    
    if(isGameOver){
        glColor3f(1, 0, 0);
        drawText(-0.2f, 0.1f, "Game is Over!");
        drawText(-0.4f, -0.1f, "Final Score - Left: " + to_string(leftScore) + " / Right: " + to_string(rightScore) );
    }else{
        
        glColor3f(0.2f, 0.8f, 0.2f);
        drawPaddle(-1.0f, leftPaddleY);
        glColor3f(0.2f, 0.2f, 0.8f);
        drawPaddle(0.95f, rightPaddleY);
        
        drawBall();
        
        glColor3f(0, 0, 0);
        drawText(-0.95f, 0.9f, "Left: "+ to_string(leftScore));
        drawText(0.6f, 0.9f, "Right: "+ to_string(rightScore));
        
        int remain = max(0, (int)(gameDuration - (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000) );
        
        int sp = static_cast<int>(((fabs(ballDX)-0.009f)/(0.02f-0.009f)) * 100.0f);
        drawText(-0.22f, 0.9f, "Speed: "+to_string(fabs(ballDX)) +" / "+ to_string(sp)+"%");
        drawText(-0.12f, 0.8f, "Time: "+to_string(remain) +"sec");
        
    }
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int){
    if(!isGameOver){
        if(key == 'w' && leftPaddleY <0.8f){
            leftPaddleY += paddleSpeed;
        }else if(key == 's' && leftPaddleY > -0.8f){
            leftPaddleY -= paddleSpeed;
        }
    }
}


void specialInput(int key, int, int){
    if(!isGameOver){
        if(key == GLUT_KEY_UP && rightPaddleY <0.8f){
            rightPaddleY += paddleSpeed;
        }else if(key == GLUT_KEY_DOWN && rightPaddleY > -0.8f){
            rightPaddleY -= paddleSpeed;
        }
    }
}

void incBallSpeed() {
    if (fabs(ballDX) <= 0.02f) {
        ballDX += (ballDX < 0 ? -ballIncRate : ballIncRate);
        ballDY += (ballDY < 0 ? -ballIncRate : ballIncRate);
    }
}
void updateBall(){
    
    ballX += ballDX;
    ballY += ballDY;
    
    
    if(ballY > 0.98 || ballY < -0.98){
        ballDY = -ballDY;
    }
    
    //for left
    if(ballX < -0.93f && ballY < leftPaddleY+0.2f && ballY > leftPaddleY-0.2f){
        ballDX = -ballDX;
        incBallSpeed();
    }
    
    //for right
    if(ballX > 0.93f && ballY < rightPaddleY+0.2f && ballY > rightPaddleY-0.2f){
        ballDX = -ballDX;
        incBallSpeed();
    }
    
    //scoreing and overlay
    if(ballX < -1.05){
        flashTimer = 30;
        rightScore++;
        ballX = 0; ballY = 0; ballDX = fabs(ballDX);
        flashSide = "right";
        incBallSpeed();
    }else if(ballX > 1.05f){
        flashTimer = 30;
        leftScore++;
        ballX = 0; ballY = 0; ballDX = -fabs(ballDX);
        flashSide = "left";
        incBallSpeed();
    }
    
}

void timer(int){
    if(!isGameOver){
        long int time = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000;
        if(time >= gameDuration){
            isGameOver = true;
        }else{
            updateBall();
        }
    }
    
    // update border colors
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    
    if(currentTime - lastBorderUpdateTime  >= 1000){
        for(int i=0; i<4; i++){
            borderColor[i][0] = static_cast<float>(rand()%256) / 255.0f;
            borderColor[i][1] = static_cast<float>(rand()%256) / 255.0f;
            borderColor[i][2] = static_cast<float>(rand()%256) / 255.0f;
        }
        lastBorderUpdateTime = currentTime;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

int main(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Ping Pong Game | ID: 41220200217");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    
    startTime = glutGet(GLUT_ELAPSED_TIME);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialInput);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    
    
    return 0;
}















