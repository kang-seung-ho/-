#include "main.h"
#include "obj.h"

GLuint vao;

GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;

GLuint WallPosVbo;
GLuint WallNomalVbo;

GLuint MainPosVbo2;
GLuint MainNomalVbo2;

GLuint spherePosVbo;
GLuint sphereNomalVbo;
//종료 조건 넣고
//


std::default_random_engine engine(std::random_device{}());

std::uniform_real_distribution<GLfloat> random_scale(0.25f, 0.5f);
std::uniform_real_distribution<GLfloat> random_move(-0.1f, 0.1f);
std::uniform_real_distribution<GLfloat> random_color(0.0f, 1.0f);


class obs {
public:
    GLfloat x{}, y{}, z{-45.0f};
    GLfloat x_scale{2.0f}, y_scale{0.0001f}, z_scale{50.0f};
    objRead objReader;
    GLint Object = objReader.loadObj_normalize_center("cube.obj");
};
class obss {
public:
    GLfloat x{}, y{0.25f}, z{ -1.0f };
    GLfloat x_scale{ 0.25f }, y_scale{ 0.25f }, z_scale{ 0.25f };
    objRead objReader;
    int jump_scale{};
    GLint Object = objReader.loadObj_normalize_center("cube.obj");
};

class object_won {
public:
    GLfloat x{}, y{ 0.25f }, z{ -100.0f };
    GLfloat x_scale{ 0.25f }, y_scale{ 0.25f }, z_scale{ 0.25f };
    GLfloat x_move{}, y_move{};
    GLfloat r{}, g{}, b{}, a{ 1.0 };
    GLuint vvbo{}, nvbo{};

    void init() {
        this->z = -100.0f;
        this->x = 0.0f;
        this->y = 2.0f;


        this->r = random_color(engine);
        this->g = random_color(engine);
        this->b = random_color(engine);

        float size = random_scale(engine);

        this->x_scale = size;
        this->y_scale = size;
        this->z_scale = size;

        this->x_move = random_move(engine);
        this->y_move = random_move(engine);

        this->vvbo = spherePosVbo;
        this->nvbo = sphereNomalVbo;
    }

    void move() {
        this->x += this->x_move;
        this->y += this->y_move;

        this->z += 1.0f;

        if (this->x + this->x_scale + this->x_move > 2.0f || this->x - this->x_scale + this->x_move < -2.0f)
        {
            this->x_move *= -1;
        }
        if (this->y + this->y_scale + this->y_move > 4.0f || this->y - this->y_scale + this->y_move < 0.0f)
        {
            this->y_move *= -1;
        }
    }

};

class light_set {
public:
    float rotate_light = 0;

    float light_x = 0;
    float light_y = 8.0f;
    float light_z = 6.0f;

    float rotate_cube = 0;
    float cameraRotation = 0;

    float light_r = 1.0f;
    float light_g = 1.0f;
    float light_b = 1.0f;

    float camera_x = 0;
    float camera_y = 2.0f;
    float camera_z = 5.0f;
};

obs wall;
obss main_character;

std::vector<object_won> objects;

objRead sphereReader;
GLint sphereObject = sphereReader.loadObj_normalize_center("sphere.obj");

GLfloat Color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };
GLvoid update(int value);

void make_shaderProgram();
void make_vertexShaders();
void make_fragmentShaders();
void drawScene();
void Reshape(int w, int h);
void InitBuffer();
char* filetobuf(const char*);

GLvoid keyboard(unsigned char key, int x, int y);
GLvoid keyUp(unsigned char, int, int);
GLvoid handleEvent(unsigned char key, bool state);
GLvoid Motion(int x, int y);
GLvoid MousePoint(int button, int state, int x, int y);
GLvoid j_ok(int value);
GLvoid jump();
GLvoid object_ok(int value);

bool checkCollision(object_won& , obss& );

int playerHP{};
int move_check{};
int jump_check = 3;
int sever_level = 0;
light_set light;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    glutSetCursor(GLUT_CURSOR_NONE);
    glewInit();

    make_shaderProgram();
    InitBuffer();
    glutWarpPointer(800 / 2, 800 / 2);
    glutTimerFunc(60, update, 1);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardUpFunc(keyUp);
    glutMouseFunc(MousePoint);
    glutMotionFunc(Motion);

    glutMainLoop();

    return 0;
}

void drawScene()
{

    glClearColor(Color[0], Color[1], Color[2], Color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    int PosLocation = glGetAttribLocation(shaderProgramID, "vPos");
    int NomalLocation = glGetAttribLocation(shaderProgramID, "vNormal");

    glEnableVertexAttribArray(PosLocation);
    glEnableVertexAttribArray(NomalLocation);

    int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    glUniform3f(lightPosLocation, light.light_x, light.light_y, light.light_z);
    int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    glUniform3f(lightColorLocation, light.light_r, light.light_g, light.light_b);
    unsigned int lighton = glGetUniformLocation(shaderProgramID, "light");
    glUniform1i(lighton, 1);
    int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색

    int modelMatrixLocation = glGetUniformLocation(shaderProgramID, "modelMatrix");
    int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
    int projLoc = glGetUniformLocation(shaderProgramID, "projection"); //--- 버텍스 세이더에서 투영 변환 행렬 변수값을 받아온다.

    glm::vec3 cameraPos = glm::vec3(light.camera_x, light.camera_y, light.camera_z); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -200.0f); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 vTransform(1.0f);
    glm::mat4 pTransform(1.0f);

    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);

    vTransform = glm::rotate(vTransform, glm::radians(light.cameraRotation), glm::vec3(0.0f, 0.0f, 1.0f)); // z축으로 회전

    pTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 200.0f); //--- 투영 공간 설정: fovy, aspect, near, far

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);



    int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos 값 전달: 카메라 위치
    glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

    for (int i = 0; i < 4; i++) {
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달
        if (light.cameraRotation == 180) {
            if (i == 0) {

                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 4.0f, 0)); // 이동
            }
            else if (i == 1)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 2.0f, 0)); // 이동
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
            else if (i == 2)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
            else if (i == 3)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 2.0f, 0.0f)); // 이동
                modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
        }
        else
        {
            if (i == 0) {

            }
            else if (i == 1)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 2.0f, 0.0f)); // 이동
                modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
            else if (i == 2)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 4.0f, 0)); // 이동
                modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
            else if (i == 3)
            {
                modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 2.0f, 0)); // 이동
                modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
            }
        }
        modelMatrix = glm::translate(modelMatrix, glm::vec3(wall.x, wall.y, wall.z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(wall.x_scale, wall.y_scale, wall.z_scale));
        glUniform4f(objColorLocation, 0.0, 0.0, 1.0,1.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, WallPosVbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, WallNomalVbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, wall.Object);

    }
    {
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달
        modelMatrix = glm::translate(modelMatrix, glm::vec3(main_character.x, main_character.y, main_character.z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(main_character.x_scale, main_character.y_scale, main_character.z_scale));
        glUniform4f(objColorLocation, 1.0, 0.0, 0.0,1.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, MainPosVbo2);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, MainNomalVbo2);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, main_character.Object);

    }
    for (int i = 0; i < objects.size(); i++){
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달
        modelMatrix = glm::translate(modelMatrix, glm::vec3(objects[i].x, objects[i].y, objects[i].z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(objects[i].x_scale, objects[i].y_scale, objects[i].z_scale));
        glUniform4f(objColorLocation, objects[i].r, objects[i].g, objects[i].b, objects[i].a);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, objects[i].vvbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, objects[i].nvbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, sphereObject);
    }

    glDisableVertexAttribArray(PosLocation);
    glDisableVertexAttribArray(NomalLocation);

    glutSwapBuffers();
}


void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void InitBuffer()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &WallPosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, WallPosVbo);
    glBufferData(GL_ARRAY_BUFFER, wall.objReader.outvertex.size() * sizeof(glm::vec3), &wall.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &WallNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, WallNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, wall.objReader.outnormal.size() * sizeof(glm::vec3), &wall.objReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &MainPosVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, MainPosVbo2);
    glBufferData(GL_ARRAY_BUFFER, main_character.objReader.outvertex.size() * sizeof(glm::vec3), &main_character.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &MainNomalVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, MainNomalVbo2);
    glBufferData(GL_ARRAY_BUFFER, main_character.objReader.outnormal.size() * sizeof(glm::vec3), &main_character.objReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &spherePosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, spherePosVbo);
    glBufferData(GL_ARRAY_BUFFER, sphereReader.outvertex.size() * sizeof(glm::vec3), &sphereReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &sphereNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, sphereNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, sphereReader.outnormal.size() * sizeof(glm::vec3), &sphereReader.outnormal[0], GL_STATIC_DRAW);


}

void make_shaderProgram()
{
    make_vertexShaders();
    make_fragmentShaders();

    shaderProgramID = glCreateProgram();
    glAttachShader(shaderProgramID, vertexShader);
    glAttachShader(shaderProgramID, fragmentShader);
    glLinkProgram(shaderProgramID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
    glCompileShader(vertexShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cout << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

void make_fragmentShaders()
{
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
    glCompileShader(fragmentShader);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cout << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
        return;
    }
}

char* filetobuf(const char* file)
{
    FILE* fptr;
    long length;
    char* buf;
    fptr = fopen(file, "rb");
    if (!fptr)
        return NULL;
    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}



GLvoid update(int value) {

    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i].z > 5.0f)
        {
            objects[i].init();
        }
        objects[i].move();

        if (checkCollision(objects[i], main_character)) {
            // 충돌 발생 시 구를 숨깁니다.
            objects[i].z = -200.0f; // 구의 위치를 화면 밖으로 이동
            playerHP -= 10;
            std::cout << playerHP << std::endl;
        }
    }


    if (light.cameraRotation == 0)
    {
        main_character.y = 0.25f + 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;

    }
    else if (light.cameraRotation == 270)
    {
        main_character.x = 2.0f - main_character.x_scale - 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;

    }
    else if (light.cameraRotation == 180)
    {
        main_character.y = 4.0f - main_character.y_scale - 0.1f * main_character.jump_scale;
        light.light_y = -4.0f;
    }
    else if (light.cameraRotation == 90)
    {
        main_character.x = -2.0f + main_character.x_scale + 0.1f * main_character.jump_scale;
        light.light_y = 8.0f;
    }

    InitBuffer();
    glutPostRedisplay();

    glutTimerFunc(30, update, 1);
}

GLvoid keyboard(unsigned char key, int x, int y)
{
    handleEvent(key, true);
    glutPostRedisplay();
}
GLvoid keyUp(unsigned char key, int x, int y)
{
    handleEvent(key, false);
    glutPostRedisplay();
}

GLvoid handleEvent(unsigned char key, bool state)
{
    if (state)
    {
        switch (key) {
        case 'z':
            jump();
            break;
        case 'c':
            if (sever_level != 1) {
                sever_level = 1;

                glutTimerFunc(90, object_ok, 1);
            }
            break;
        }
    }
}
bool left_button = false;
GLvoid MousePoint(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_UP) {
           left_button = false;
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

        }
        else {
           left_button = true;
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
        }
    }
}

GLvoid Motion(int x, int y) {
    {
        if (left_button) {
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
            GLfloat mouseX2 = static_cast<GLfloat>(x) - (glutGet(GLUT_WINDOW_WIDTH) / 2.0f);

            // 윈도우 중심(400)에서 마우스 위치의 거리 계산
            GLfloat distanceFromCenter = mouseX2; // 윈도우 중심을 기준으로 거리 계산
            GLfloat rotationChange = distanceFromCenter * 0.05f; // 필요에 따라 감도 조절

            if (light.cameraRotation == 0)
            {
                main_character.y = 0.25f + 0.1f * main_character.jump_scale;
                main_character.x += rotationChange;
                light.light_y = 8.0f;

            }
            else if (light.cameraRotation == 270)
            {
                main_character.x = 2.0f - main_character.x_scale - 0.1f * main_character.jump_scale;
                main_character.y += rotationChange;
                light.light_y = 8.0f;

            }
            else if (light.cameraRotation == 180)
            {
                main_character.y = 4.0f - main_character.y_scale - 0.1f * main_character.jump_scale;
                main_character.x -= rotationChange;
                light.light_y = -4.0f;
            }
            else if (light.cameraRotation == 90)
            {
                main_character.x = -2.0f + main_character.x_scale + 0.1f * main_character.jump_scale;
                main_character.y -= rotationChange;
                light.light_y = 8.0f;
            }
            // 마우스를 윈도우 중앙으로 이동
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
        }

        if (main_character.x + main_character.x_scale > 2.0f)
        {
            light.cameraRotation = 270.0f;
            light.camera_x = 2.0f;
            light.camera_y = 0;
            jump_check = 3;
            main_character.jump_scale = 0;
        }
        else if (main_character.x - main_character.x_scale < -2.0f)
        {
            light.cameraRotation = 90.0f;
            light.camera_x = -2.0f;
            light.camera_y = 0.0f;
            jump_check = 3;
            main_character.jump_scale = 0;
        }
        else if (main_character.y - main_character.y_scale < 0.0f)
        {
            light.cameraRotation = 0.0f;
            light.camera_x = 0.0f;
            light.camera_y = 2.0f;
            jump_check = 3;
            main_character.jump_scale = 0;
        }
        else if (main_character.y + main_character.y_scale > 4.0f)
        {
            light.cameraRotation = 180.0f;
            light.camera_x = 0.0f;
            light.camera_y = -2.0f;
            main_character.jump_scale = 0;
            jump_check = 3;
        }
        InitBuffer();
        glutPostRedisplay();
    }
}

GLvoid jump() {
    if (light.cameraRotation == 0)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, j_ok, 1);
        }
    }
    else if (light.cameraRotation == 270)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, j_ok, 1);
        }
    }
    else if (light.cameraRotation == 180)
    {
        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, j_ok, 1);
        }
    }
    else if (light.cameraRotation == 90)
    {

        if (jump_check == 3) {
            jump_check = 0;
            main_character.jump_scale = 0;
            glutTimerFunc(60, j_ok, 1);
        }
    }
}

bool checkCollision(object_won& sphere, obss& wall) {
    // AABB - 원 충돌
    float closestX = std::max(wall.x - wall.x_scale, std::min(sphere.x, wall.x + wall.x_scale));
    float closestY = std::max(wall.y - wall.y_scale, std::min(sphere.y, wall.y + wall.y_scale));
    float closestZ = std::max(wall.z - wall.z_scale, std::min(sphere.z, wall.z + wall.z_scale));

    // 원의 중심과 가장 가까운 점 사이의 거리를 계산
    float distanceX = sphere.x - closestX;
    float distanceY = sphere.y - closestY;
    float distanceZ = sphere.z - closestZ;

    // 거리가 원의 반지름보다 작으면 교차점생김
    float radius = sphere.x_scale;
    return (distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ) < (radius * radius);
}


GLvoid j_ok(int value) {

    if (jump_check != 3) {
    if (main_character.jump_scale < 15 && jump_check == 0) {
        main_character.jump_scale += 1;
        if (main_character.jump_scale == 15)
        {
            jump_check = 1;
        }
    }
    else if (main_character.jump_scale > 0 && jump_check == 1) {
        main_character.jump_scale -= 1;
        if (main_character.jump_scale == 0)
        {
            jump_check = 3;
        }
    }
    InitBuffer();
    glutPostRedisplay();
    glutTimerFunc(30, j_ok, 1);
    }
}



GLvoid object_ok(int value) {

    if (objects.size() < 5) {
        object_won new_object;
        new_object.init(); // 객체 초기화

        objects.push_back(new_object);


        InitBuffer();
        glutPostRedisplay();
        glutTimerFunc(480, object_ok, 1);
    }
}