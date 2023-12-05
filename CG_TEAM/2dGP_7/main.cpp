#include "main.h"
#include "obj.h"

GLuint vao;

GLchar* vertexSource, * fragmentSource;
GLuint vertexShader, fragmentShader;
GLuint shaderProgramID;

GLuint CirclePosVbo;
GLuint CircleNomalVbo;

GLuint CirclePosVbo2;
GLuint CircleNomalVbo2;
class obs {
public:
    GLfloat x{}, y{}, z{-25.0f};
    GLfloat x_scale{2.0f}, y_scale{0.01f}, z_scale{50.0f};
    objRead objReader;
    GLint Object = objReader.loadObj_normalize_center("cube.obj");
};
class obss {
public:
    GLfloat x{}, y{0.25f}, z{ -1.0f };
    GLfloat x_scale{ 0.25f }, y_scale{ 0.25f }, z_scale{ 0.25f };
    objRead objReader;
    GLint Object = objReader.loadObj_normalize_center("cube.obj");
};

class light_set {
public:
    float rotate_light = 0;

    float light_x = 0;
    float light_y = 8.0f;
    float light_z = 5;

    float rotate_cube = 0;
    float cameraRotation = 0;

    float light_r = 1.0f;
    float light_g = 1.0f;
    float light_b = 1.0f;

    float camera_x = 0;
    float camera_y = 2.0f;
    float camera_z = 5.0f;
};

obs obstacle;
obss obstacle2;

GLfloat Color[4]{ 0.0f, 0.0f, 0.0f, 1.0f };

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

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Example1");

    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);

    glewInit();

    make_shaderProgram();
    InitBuffer();
    glutWarpPointer(800 / 2, 800 / 2);
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutKeyboardUpFunc(keyUp);

    glutMouseFunc(MousePoint);
    glutMotionFunc(Motion);

    glutMainLoop();

    return 0;
}


light_set light;

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
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -150.0f); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 vTransform(1.0f);
    glm::mat4 pTransform(1.0f);
    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);

    vTransform = glm::rotate(vTransform, glm::radians(light.cameraRotation), glm::vec3(0.0f, 0.0f, 1.0f)); // z축으로 회전

    pTransform = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f); //--- 투영 공간 설정: fovy, aspect, near, far

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);


    int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos"); //--- viewPos 값 전달: 카메라 위치
    glUniform3f(viewPosLocation, cameraPos.x, cameraPos.y, cameraPos.z);

    for (int i =0;i<4;i++){
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달
        if (i == 1)
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
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f,2.0f,0)); // 이동
            modelMatrix = glm::rotate(modelMatrix, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X 축 회전
        }
        modelMatrix = glm::translate(modelMatrix, glm::vec3(obstacle.x, obstacle.y, obstacle.z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(obstacle.x_scale, obstacle.y_scale, obstacle.z_scale));
        glUniform3f(objColorLocation, 0.0, 0.0, 1.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, obstacle.Object);

    }
    {
        // 모델 행렬 초기화
        glm::mat4 modelMatrix(1.0f);
        // 모델 행렬을 셰이더에 전달
        modelMatrix = glm::translate(modelMatrix, glm::vec3(obstacle2.x, obstacle2.y, obstacle2.z)); // 이동
        modelMatrix = glm::scale(modelMatrix, glm::vec3(obstacle2.x_scale, obstacle2.y_scale, obstacle2.z_scale));
        glUniform3f(objColorLocation, 1.0, 0.0, 0.0);

        glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);

        glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo2);
        glVertexAttribPointer(PosLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(PosLocation);

        glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo2);
        glVertexAttribPointer(NomalLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(NomalLocation);

        glDrawArrays(GL_TRIANGLES, 0, obstacle2.Object);

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

    glGenBuffers(1, &CirclePosVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo);
    glBufferData(GL_ARRAY_BUFFER, obstacle.objReader.outvertex.size() * sizeof(glm::vec3), &obstacle.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &CircleNomalVbo);
    glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo);
    glBufferData(GL_ARRAY_BUFFER, obstacle.objReader.outnormal.size() * sizeof(glm::vec3), &obstacle.objReader.outnormal[0], GL_STATIC_DRAW);

    glGenBuffers(1, &CirclePosVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, CirclePosVbo2);
    glBufferData(GL_ARRAY_BUFFER, obstacle2.objReader.outvertex.size() * sizeof(glm::vec3), &obstacle2.objReader.outvertex[0], GL_STATIC_DRAW);

    glGenBuffers(1, &CircleNomalVbo2);
    glBindBuffer(GL_ARRAY_BUFFER, CircleNomalVbo2);
    glBufferData(GL_ARRAY_BUFFER, obstacle2.objReader.outnormal.size() * sizeof(glm::vec3), &obstacle2.objReader.outnormal[0], GL_STATIC_DRAW);
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
        case 'a':
            obstacle2.x -= 0.15f;
            break;
        case 'd':
            obstacle2.x += 0.15f;
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
int move_check = 0;

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
                obstacle2.y = 0.25f;
                obstacle2.x += rotationChange;
            }
            else if (light.cameraRotation == 270)
            {
                obstacle2.x = 2.0f - obstacle2.x_scale;
                obstacle2.y += rotationChange;
            }
            else if (light.cameraRotation == 180)
            {
                obstacle2.y = 4.0f - obstacle2.y_scale;
                obstacle2.x -= rotationChange;
            }
            else if (light.cameraRotation == 90)
            {
                obstacle2.x = -2.0f + obstacle2.x_scale;
                obstacle2.y -= rotationChange;
            }
            // 마우스를 윈도우 중앙으로 이동
            glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
        }

        if (obstacle2.x+obstacle2.x_scale > 2.0f )
        {
            light.cameraRotation = 270.0f;
            light.camera_x = 2.0f;
            light.camera_y = 0;
        }
        else if (obstacle2.x - obstacle2.x_scale < -2.0f)
        {
            light.cameraRotation = 90.0f;
            light.camera_x = -2.0f;
            light.camera_y = 0.0f;
        }
        else if (obstacle2.y - obstacle2.y_scale <0.0f)
        {
            light.cameraRotation = 0.0f;
            light.camera_x = 0.0f;
            light.camera_y = 2.0f;
        }
        else if (obstacle2.y + obstacle2.y_scale > 4.0f)
        {
            light.cameraRotation = 180.0f;
            light.camera_x = 0.0f;
            light.camera_y = -2.0f;
        }
        InitBuffer();
        glutPostRedisplay();
    }
}

